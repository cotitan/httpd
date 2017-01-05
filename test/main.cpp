#include "test.h"

const int nThr = 10;
int test::nThread = nThr;
pthread_t* test::threads = new pthread_t[nThr];
pthread_mutex_t test::mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t test::nJobs = 0;
queue<int> test::jobs = queue<int>();

int main() {
	int nThr = 10;
	test test1;
	for (int i = 0; i < nThr; i++)
		test1.add_job(i);
	return 0;
}
