#include "server.h"
#include "tools.h"
#include <signal.h>
#include <stdexcept>
#include <iostream>
#include <pthread.h>
#include <unistd.h>

using namespace std;

server::server() : server("127.0.0.1", 9090) {}

server::server(const char *ip, short port) {
	servaddr.sin_family = AF_INET;
	/*
	if (inet_pton(AF_INET, ip, &servaddr.sin_addr) < 0)
		throw invalid_argument("Error: IP address cannot be resolved");
		*/
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(port);
	bzero(&servaddr.sin_zero, sizeof(servaddr.sin_zero));

	if ((servsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw invalid_argument("Error: fail to get sockfd!");
	maxfdp1 = servsock + 1;
}

int server::start() {

	signal(SIGPIPE, SIG_IGN);

	int flag = 1;
	socklen_t flag_size = sizeof(flag);
	setsockopt(servsock, SOL_SOCKET, SO_REUSEADDR, &flag, flag_size);
	if (bind(servsock, (SA *)&servaddr, sizeof(servaddr)) < 0)
		throw invalid_argument("Error: fail to bind!");
	if (listen(servsock, 1000) < 0)
		throw invalid_argument("Error: fail to listen!");
	fprintf(stdout, "start listening on port %d...\n",
			ntohs(servaddr.sin_port));

	fd_set rset, allset;
	FD_ZERO(&rset);
	FD_ZERO(&allset);
	FD_SET(servsock, &allset);

	pthread_t pid;
	int nresponse, connsock;
	socklen_t sin_size = sizeof(servaddr);


	while (1) {
		rset = allset;
		nresponse = select(maxfdp1, &rset, NULL, NULL, NULL);

		if (FD_ISSET(servsock, &rset)) {
			connsock = accept(servsock, (SA *)&connaddr, &sin_size);
			fprintf(stdout, "remote host %s:%d connected...\n",
					inet_ntoa(connaddr.sin_addr), connaddr.sin_port);
			FD_SET(connsock, &allset);
			maxfdp1 = connsock >= maxfdp1 ? connsock + 1 : maxfdp1;
			clntsock.push_back(connsock);
		}

		void *status;
		for (size_t i = 0; i < clntsock.size() && nresponse; ++i) {
			if (FD_ISSET(clntsock[i], &rset)) {
				pthread_create(&pid, NULL, accept_req, (void *)&clntsock[i]);
				pthread_join(pid, &status);

				if (status == (void *)-1) {
					getpeername(clntsock[i], (SA *)&connaddr, &sin_size);
					fprintf(stdout, "remote host %s:%d disconnected...\n",
							inet_ntoa(connaddr.sin_addr), connaddr.sin_port);
					close(clntsock[i]);
					// printf("socket %d closed\n", clntsock[i]);
					FD_CLR(clntsock[i], &allset);
					clntsock.erase(clntsock.begin() + i);
					--i;
				}
				nresponse--;
			}
		} // end for

	}
	return 0;
}

void server::route(int fd, char *msg) {
	httpRequest req_msg(msg);
	string url = req_msg.getUrl();
	fprintf(stderr, "%s\n", url.c_str());

	httpResponse res;
	// if is root
	if (url == "/" || url == "/index.html") {
		send_resp(fd, 200, "OK", "text/html", "res/index.html");
		return;
	} else if (url == "/favicon.ico") {
		send_resp(fd, 200, "OK", "image/webp", "picture/favicon.ico");
		return;
	}

	// e.g ip:port/non-exist.html
	size_t epos = url.find('/', 0 + 1);
	if (epos == string::npos) {
		send_resp(fd, 404, "Not Found", "text/html", "res/default.html");
	}

	string level1 = url.substr(0, epos);
	if (level1 == "/picture") {
		string filename = url.substr(1, url.length() - 1);
		if (req_msg.getMethod() == GET) {
			ifstream fin(filename.c_str());
			if (fin.is_open()) {
				fin.close();	// reopen in send_resp()
				send_resp(fd, 200, "OK", "image/webp",filename.c_str());
			}
			else
				send_resp(fd, 404, "Not Found", "text/html", "res/default.html");
			fin.close();
		} else if ( req_msg.getMethod() == POST) {
			ofstream fout(filename.c_str());
			// if (!fout.is_open()) {
				// fout << req_msg.getData();
				send_resp(fd, 200, "OK");
			// } else {
			//	send_resp(fd, 404, "Not Found");
			// }
			fout.close();
		}

	} else {
		send_resp(fd, 404, "Not Found", "text/html", "res/default.html");
	}
}

void server::response_to_req(int fd, char *msg) {
	httpRequest req_msg(msg);
	// req_msg.print();

	char buf[101];
	strcpy(buf, "HTTP/1.1 200 OK\r\n");
	strcat(buf, "Server: jdbhttpd/0.1.0\r\n");
	strcat(buf, "Content-Type: text/html\r\n");
	strcat(buf, "Connection: close\r\n");
	strcat(buf, "\r\n");
	write(fd, buf, strlen(buf));
	FILE *resource = fopen("res/index.html", "r");
	fgets(buf, sizeof(buf), resource);
	while (!feof(resource)) {
		write(fd, buf, strlen(buf));
		fgets(buf, sizeof(buf), resource);
	}
	fclose(resource);
	/*
	strcpy(msgToWrite, "HTTP/1.1 200 OK\n");
	strcat(msgToWrite, "Connection: keep-alive\n");
	// strcat(msgToWrite, "Content-Encoding: gzip\n");
	ifstream fin("res/index.html");
	fin.seekg(0, ios::end);
	size_t length = fin.tellg();
	sprintf(buf, "Content-Length: %lu\n", length);
	strcat(msgToWrite, buf);
	strcat(msgToWrite, " Content-Type: text/html\n\n");
	fin.seekg(0);
	fin.get(buf, length, '\0');
	strcat(msgToWrite, buf);
	// cout << msgToWrite << endl;
	int nwrite = write(fd, msgToWrite, strlen(msgToWrite));
	fin.close();
	*/
}

server::~server() {
	close(servsock);
	for (size_t i = 0; i < clntsock.size(); ++i)
		close(clntsock[i]);
	cerr << "Server shutdown, all connect closed\n";
}

