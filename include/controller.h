#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#ifndef PAGE404
	#define PAGE404 ("res/default.html")
#endif

#ifndef PAGE_UPLOAD
	#define PAGE_UPLOAD ("res/upload.html")
#endif

#include <fstream>
#include "httpMessage.h"
using namespace std;
using namespace http;

class controller {
public:
	virtual int handle(int fd, const httpRequest &req);
	virtual ~controller() {}
};

class pic_controller: public controller {
public:
	virtual int handle(int fd, const httpRequest &req);

	virtual ~pic_controller() {}
};

#endif
