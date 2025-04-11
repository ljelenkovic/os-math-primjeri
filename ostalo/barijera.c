#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_barrier_t barijera;

int na_barijeri = 0;

void *dretva (int p) {
	while (1) {
		printf("Dretva %d krece\n", p);
		sleep(p);

		na_barijeri++;
		printf("Dretva %d na barijeri (ukupno na barijeri %d dretvi)\n",
			p, na_barijeri);

		pthread_barrier_wait(&barijera);
		na_barijeri--;

		if(!na_barijeri)
			printf("---Prosli barijeru---\n");

		sleep(p);
	}
}

int main () {
	pthread_t thr_id;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_barrier_init(&barijera, NULL, 5);
	
	pthread_create(&thr_id, &attr, (void *(*)(void *)) dretva, (void *) 1);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) dretva, (void *) 2);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) dretva, (void *) 3);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) dretva, (void *) 4);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) dretva, (void *) 5);
		
	sleep(50); //vrijeme simulacije

	return 0;
}
