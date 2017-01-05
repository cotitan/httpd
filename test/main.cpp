#include "test.h"

int main() {
	int nThr = 10;
	int test::nThread = nThr;
	pthread_mutex_init(&(int test::mutex), NULL);
	sem_init(&(int test::nJobs), 0, 0);
	pthread_t* test::threads = new pthread_t[nThr];
	test test1;
	for (int i = 0; i < nThr; i++)
		test1.add_job(i);
	return 0;
}
