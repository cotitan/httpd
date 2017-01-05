#include "test.h"

int main() {
	test::nThread = nThr;
	pthread_mutex_init(&test::mutex, NULL);
	sem_init(&test::nJobs, 0, 0);
	test::threads = new pthread_t[nThr];
	test test1;
	for (int i = 0; i < 10; i++)
		test1.add_job(i);
	return 0;
}
