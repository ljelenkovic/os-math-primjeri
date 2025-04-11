#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

pthread_spinlock_t kljuc;

void *dretva (int p) {
	while (1) {
		printf("Dretva %d spremna\n", p);
		pthread_spin_lock(&kljuc);
		printf("Dretva %d unutar K.O.\n", p);
		sleep(p);
		printf("Dretva %d izlazi iz K.O.\n", p);
		pthread_spin_unlock(&kljuc);
		sleep(p);
	}
}

int main () {
	pthread_t thr_id;

	pthread_spin_init(&kljuc, PTHREAD_PROCESS_PRIVATE);
	
	pthread_create(&thr_id, NULL, (void *(*)(void *)) dretva, (void *) 1);
	pthread_create(&thr_id, NULL, (void *(*)(void *)) dretva, (void *) 2);
	pthread_create(&thr_id, NULL, (void *(*)(void *)) dretva, (void *) 3);
	pthread_create(&thr_id, NULL, (void *(*)(void *)) dretva, (void *) 4);
	pthread_create(&thr_id, NULL, (void *(*)(void *)) dretva, (void *) 5);
	pthread_create(&thr_id, NULL, (void *(*)(void *)) dretva, (void *) 6);
		
	sleep(50); //vrijeme simulacije

	return 0;
}
