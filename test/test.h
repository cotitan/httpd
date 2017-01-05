#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <iostream>
using namespace std;

class test {
private:
	static int nThread;
	static queue<int> jobs;
	static pthread_mutex_t mutex;
	static sem_t nJobs;
	static pthread_t *threads;
public:
	test() {
		threads = new pthread_t[nThread];
		for (int i = 0; i < nThread; i++)
			pthread_create(&threads[i], NULL, &exec, NULL);
		cout << nThread << " threads created!\n";
	}
	void add_job(int fd) {
		pthread_mutex_lock(&mutex);
		jobs.push(fd);
		pthread_mutex_unlock(&mutex);
		sem_post(&nJobs);
	}
	static void* exec(void *args) {
		while (true) {
			sem_wait(&nJobs);
			pthread_mutex_lock(&mutex);
			if (jobs.front() > 10)
				break;
			cout << "exec job: #" << jobs.front();
			cout << " on thread #" << pthread_self() << endl;
			pthread_mutex_unlock(&mutex);
		}
		return NULL;
	}
	~test() {
		for (int i = 0; i < nThread; i++)
			pthread_join(threads[i], NULL);
		delete[] threads;
	}
};
