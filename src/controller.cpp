#include "controller.h"

extern void send_resp(int fd, int code, const char *state,
	const char *type = "", const char *datapath = nullptr);

int controller::handle(int fd, const httpRequest &req) {
	string url = req.getUrl();
	if (url == "/" || url == "/index.html") {
		return send_resp(fd, 200, "OK", "text/html", "res/index.html");
	} else if (url == "/favicon.ico") {
		return send_resp(fd, 200, "OK", "image/webp", "picture/favicon.ico");
	} else {
		return send_resp(fd, 404, "Not Found", "text/html", PAGE404);
	}
}

int pic_controller::handle(int connfd, const httpRequest &req) {
	string url = req.getUrl();
	string path = url.substr(1, url.length() - 1);

	if (req.getMethod() == POST) {
		ofstream fout(path.c_str(), ios::binary);
		if (fout.is_open()) {
			fout.write(req.getData(), req.getContentLength());
			fout.close();
		}
		 return send_resp(connfd, 200, "OK");
	} else if (req.getMethod() == GET) {
		ifstream fin(path.c_str());
		if (fin.is_open()) {
			fin.close();	// reopen in send_resp()
			return send_resp(connfd, 200, "OK", "image/webp", path.c_str());
		}
		else
			return send_resp(connfd, 404, "Not Found", "text/html", PAGE404);
	}
}
