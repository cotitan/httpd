#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#ifndef PAGE404
	#define PAGE404 ("res/default.html")
#endif

#include <fstream>
#include "httpMessage.h"
extern void send_resp(int fd, int code, const char *state,
	const char *type = "text/html", const char *datapath = NULL);
using namespace std;
using namespace http;

class controller {
public:
	virtual void handle(int fd, const httpRequest &req) {
		string url = req.getUrl();
		if (url == "/" || url == "/index.html") {
			send_resp(fd, 200, "OK", "text/html", "res/index.html");
		} else if (url == "/favicon.ico") {
			send_resp(fd, 200, "OK", "image/webp", "picture/favicon.ico");
		} else {
			send_resp(fd, 404, "Not Found", "text/html", PAGE404);
		}
	}
};

class pic_controller: public controller {
public:
	virtual void handle(int connfd, const httpRequest &req) {
		string url = req.getUrl();
		string path = url.substr(1, url.length() - 1);
		
		if (req.getMethod() == POST) {
			ofstream fout(path.c_str(), ios::binary);
			if (fout.is_open()) {
				fout.write(req.getData(), req.getContentLength());
				fout.close();
			}
			send_resp(connfd, 200, "OK");
		} else if (req.getMethod() == GET) {
			ifstream fin(path.c_str());
			if (fin.is_open()) {
				fin.close();	// reopen in send_resp()
				send_resp(connfd, 200, "OK", "image/webp", path.c_str());
			}
			else
				send_resp(connfd, 404, "Not Found", "text/html", PAGE404);
		}
	}
};

#endif
