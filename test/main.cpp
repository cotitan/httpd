#include "test.h"

int main() {
	int nThr = 10;
	test test1(nThr);
	for (int i = 0; i < nThr * 2; i++)
		test1.add_job(i);
	return 0;
}
