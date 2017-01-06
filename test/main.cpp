#include "test.h"

int main() {
	int nThr = 10;
	test test1;
	for (int i = 0; i < nThr; i++)
		test1.add_job(i);
	return 0;
}
