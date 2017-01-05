#include "test.h"

int main() {
	test test1;
	for (int i = 0; i < 10; i++)
		test1.add_job(i);
	return 0;
}
