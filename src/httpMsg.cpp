#include "httpMessage.h"
#include <iostream>
#include <cstring>
#include <cctype>
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

	int spos = line.find('/', epos);
	epos = line.find(' ', spos);
	this->Host = line.substr(spos, epos - spos);

	spos = epos + 1;
	epos = line.length();
	this->http_version = line.substr(spos, epos - spos);
}
