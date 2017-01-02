#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <queue>
using namespace std;

struct job {
	void *func(void *args);
	void *args;
};

pthread_mutex_t mutex;
sem_t count;
queue<job> jobs;

void *func(void *args) {
	sem_wait(&mutex);
	pthread_mutex_lock(&mutex);
	job = jobs.front();
	jobs.pop();
	pthread_mutex_unlock(&mutex);
	job.func(job.args);
	sleep(1);
	cout << "finish executing job #" << a << endl;
	return NULL;
}

void *sqr(void *args) {
	int a = *(int *)args;
	cout << "a * a = " << a * a << endl;
	return NULL;
}

int main() {
	int a;
	pthread_t threads[4];
	pthread_mutex_init(&mutex, NULL);
	sem_init(&count, 0, 0);
	for (int i = 0; i < 4; i++) {
		phtread_create(threads[i], NULL, func, NULL);
	}
	for (int i = 0; i < 10; i++) {
		a = i;
		pthread_mutex_lock(&mutex);
		jobs.push(job(sqr, &a));
		pthread_mutex_unlock(&mutex);
		sem_post(&count);
	}
}
