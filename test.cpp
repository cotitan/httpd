#include <iostream>
#include <fstream>
#include <pthread.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

void* func(void *out) {
	// FILE *pout = (FILE *)out;
	FILE *pout = fopen("picture/new.png", "w");
	fprintf(stderr, "opened!\n");
	if (pout != NULL) {
		fprintf(stderr, "file pointer not null\n");
		fprintf(pout, "1.png\n");
		fclose(pout);
	} else {
		cout << "failed open!\n";
	}
	fprintf(stderr, "end\n");
	fclose(pout);
//	fclose(pout);
	return NULL;
}

int main() {
	pthread_t pid;
	FILE *pout = NULL;
	// pout = fopen("picture/new.png", "w");
	if (pthread_create(&pid, NULL, func, (void *)pout) == 0)
		printf("thread 1 create successfully...\n");
	// pthread_join(pid, NULL);
	// fclose(pout);
	return 0;
}

