#include "test.h"

int main() {
	int nThr = 10;
	int test::nThread = nThr;
	sem_t test::nJobs;
	pthread_t test::mutex;
	pthread_mutex_init(&(test::mutex), NULL);
	sem_init(&(test::nJobs), 0, 0);
	pthread_t* test::threads = new pthread_t[nThr];
	test test1;
	for (int i = 0; i < nThr; i++)
		test1.add_job(i);
	return 0;
}
