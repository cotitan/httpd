#include "server.h"

int main(int argc, char** argv) {
	server httpd;
	httpd.start();
	return 0;
}
