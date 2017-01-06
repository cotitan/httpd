#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <iostream>
#include <signal.h>
using namespace std;

class test {
private:
	int nThread;
	queue<int> jobs;
	pthread_mutex_t mutex;
	sem_t nJobs;
	pthread_t *threads;
public:
	test(int nThr = 10) {
		nThread = nThr;
		pthread_mutex_init(&mutex, NULL);
		sem_init(&nJobs, 0, 0);
		threads = new pthread_t[nThread];
		for (int i = 0; i < nThread; i++)
			pthread_create(&threads[i], NULL, &exec, this);
		cout << nThread << " threads created!\n";
	}
	void add_job(int fd) {
		pthread_mutex_lock(&mutex);
		jobs.push(fd);
		pthread_mutex_unlock(&mutex);
		sem_post(&nJobs);
	}
	static void sigproc(int sig) {
		pthread_exit(0);
	}
	static void* exec(void *args) {
		signal(SIGINT, &sigproc);
		test *obj = (test *)args;
		while (true) {
			sem_wait(&obj->nJobs);
			pthread_mutex_lock(&obj->mutex);
			if (obj->jobs.front() > 10)
				break;
			cout << "exec job: #" << obj->jobs.front();
			cout << " on thread #" << pthread_self() << endl;
			obj->jobs.pop();
			pthread_mutex_unlock(&obj->mutex);
		}
		return NULL;
	}
	~test() {
		for (int i = 0; i < nThread; i++)
			pthread_join(threads[i], NULL);
		delete[] threads;
	}
};
