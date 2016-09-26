#include "httpMessage.h"
#include <iostream>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include <sstream>
using namespace http;
using namespace std;

string trip(const string& line) {
	int i = 0, j = line.length() - 1;
	while (isspace(line[i]))
		++i;
	while (isspace(line[j]) || line[j] == '\r')
		--j;
	++j;
	return line.substr(i, j - i);
}

httpRequest::httpRequest(char *http_str) {
	data = NULL;
	this->from_str(http_str);
}

Method resolve_method(const char *method) {
	if (strcasecmp(method, "GET") == 0) {
		return GET;
	} else if (strcasecmp(method, "POST") == 0) {
		return POST;
	} else if (strcasecmp(method, "HEAD") == 0) {
		return HEAD;
	} else if (strcasecmp(method, "DELETE") == 0) {
		return DELETE;
	} else if (strcasecmp(method, "PUT") == 0) {
		return PUT;
	} else {
		return ERR;
	}
}

bool httpRequest::from_str(char *http_str) {
	stringstream ss(http_str);
	string line;
	int cnt_char = 0;
	
	getline(ss, line);
	cnt_char += line.length() + 1; // including '\n'
	line = trip(line);
	reslove_req_line(line);

	size_t pos_colon, len;
	string attr, value;
	// for each line, there is a '\r' in ending
	while (getline(ss, line) && (len = line.length()) > 1) {
		// '\n' is neglected by getline
		// so we need to add it back
		cnt_char += len + 1;
		line = trip(line);
		pos_colon = line.find(':');
		if (pos_colon != string::npos) {
			attr = line.substr(0, pos_colon);
			value = line.substr(pos_colon + 2, len - pos_colon - 2);
			if (set_attr(attr, value) == false)
				cout << "unknown header: " << attr << endl;
		}
	}
	data = new char[content_length + 1];
	strcpy(data, http_str + cnt_char);
	return true;
}

char method_str[][10] = {"GET", "POST", "HEAD", "DELETE", "PUT", "ERR" };
void httpRequest::print() {
	cout << "Method: " << method_str[method] << endl;
	cout << "URL: " << url << endl;
	cout << "HTTP: " << http_version << endl;
	cout << "Host: " << Host << endl;
	cout << "Connection: " << Connection << endl;
	cout << "Cache-control: " << cache_control.max_age << endl;
	cout << "Upgrade-Insecure-Requests: " << UIR << endl;
	cout << "User-Agent: " << user_agent << endl;
	cout << "Accept: " << accept << endl;
	cout << "Accept-Encoding: " << acpt_encd << endl;
	cout << "Accept-Language: " << acpt_lang << endl;
	cout << endl;
}

bool httpRequest::set_attr(const string &attr, const string& value) {
	const char *str = attr.c_str();
	if (strcasecmp(str, "Host") == 0) {
		Host = value;
	} else if (strcasecmp(str, "Connection") == 0) {
		Connection = value;
	} else if (strcasecmp(str, "Cache-Control") == 0) {
		cache_control.max_age = atoi(value.c_str());
	} else if (strcasecmp(str, "Upgrade-Insecure-Requests") == 0) {
		UIR = atoi(value.c_str());
	} else if (strcasecmp(str, "User-Agent") == 0) {
		user_agent = value;
	} else if (strcasecmp(str, "Accept") == 0) {
		accept = value;
	} else if (strcasecmp(str, "Referer") == 0) {
		referer = value;
	} else if (strcasecmp(str, "Accept-Encoding") == 0) {
		acpt_encd = value;
	} else if (strcasecmp(str, "Accept-Language") == 0) {
		acpt_lang = value;
	} else {
		return false;
	}
	return true;
}

void httpRequest::reslove_req_line(string &line) {
	int epos = line.find(' ');
	const char *str_method = line.substr(0, epos).c_str();
	this->method = resolve_method(str_method);

	int spos = epos + 1;
	epos = line.find(' ', spos);
	this->url = line.substr(spos, epos - spos);

	spos = epos + 1;
	epos = line.length();
	this->http_version = line.substr(spos, epos - spos);
}


/*
	string http_version;
	int code;
	string phrase;
	string content_type;
	size_t content_length;
	char *datapath;
*/
httpResponse::httpResponse() {
	http_version = "HTTP/1.1";
	set(200, "OK", "text/html", 0);
}

httpResponse::httpResponse(int code_, const string& phrase_,
		const string &type, size_t length) {
	http_version = "HTTP/1.1";
	set(code_, phrase_, type, length);
}

void httpResponse::set(int code_, const string& phrase_,
		const string &type, size_t length) {
	code = code_;
	phrase = phrase_;
	content_type = type;
	content_length = length;
}

void httpResponse::set_code(int code_) {
	code = code_;
}

void httpResponse::set_phrase(const string &phrase_) {
	phrase = phrase_;
}

void httpResponse::set_content_type(const string &type) {
	content_type = type;
}

void httpResponse::set_content_length(size_t length) {
	content_length = length;
}

void httpResponse::send_head(int fd) {
	char buf[1024], temp[100];
	sprintf(buf, "HTTP/1.1 %d %s\r\n", code, phrase.c_str());
	strcat(buf, "Server: jdbhttpd/0.1.0\r\n");
	strcat(buf, "Connection: keep-alive\r\n");
	sprintf(temp, "Content-Type: %s\r\n", content_type.c_str());
	strcat(buf, temp);
	sprintf(temp, "Content-Length: %lu\r\n", content_length);
	strcat(buf, temp);
	strcat(buf, "\r\n");
	write(fd, buf, strlen(buf));
}
