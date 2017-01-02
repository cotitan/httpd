#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <queue>
using namespace std;

struct job {
	void *(*func)(void *args);
	void *args;
	job(void *(*f1)(void *), void *arg) {
		func = f1;
		args = arg;
	}
};

pthread_mutex_t mutex;
sem_t count;
queue<job> jobs;

void *func(void *args) {
	while (true) {
		sem_wait(&count);
		pthread_mutex_lock(&mutex);
		job j1 = jobs.front();
		jobs.pop();
		pthread_mutex_unlock(&mutex);
		(*j1.func)(j1.args);
		sleep(1);
		cout << "finish executing job #" << *(int *)j1.args << endl;
		delete j1.args;
	}
	return NULL;
}

void *sqr(void *args) {
	int a = *(int *)args;
	cout << "a * a = " << a * a << endl;
	return NULL;
}

int main() {
	pthread_t threads[4];
	pthread_mutex_init(&mutex, NULL);
	sem_init(&count, 0, 0);
	for (int i = 0; i < 4; i++) {
		pthread_create(&threads[i], NULL, func, NULL);
	}
	int i = 0;
	while (cin.get() != '#') {
		int *a = new int;
		*a = i++;
		pthread_mutex_lock(&mutex);
		jobs.push(job(&sqr, a));
		pthread_mutex_unlock(&mutex);
		sem_post(&count);
	}
	void *ret;
	for (int i = 0; i < 4; i++) {
		pthread_join(threads[i], &ret);
	}
}
