#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>

pthread_rwlock_t cp_kljuc = PTHREAD_RWLOCK_INITIALIZER;
int br_citaca = 0;
int br_pisaca = 0;

void *citac (int p)
{
	while (1) {
		printf("Citac %d spreman\n", p);
		pthread_rwlock_rdlock(&cp_kljuc);
		br_citaca++;
		printf("Citac %d cita  (br. citaca = %d)\n", p, br_citaca);
		sleep(2);
		br_citaca--;
		printf("Citac %d gotov (br. citaca = %d)\n", p, br_citaca);
		pthread_rwlock_unlock(&cp_kljuc);
		sleep(p);
	}
}

void *pisac (int p)
{
	while (1) {
		printf("Pisac %d spreman\n", p);
		pthread_rwlock_wrlock(&cp_kljuc);
		br_pisaca++;
		printf("Pisac %d pise  (br. pisaca = %d)\n", p, br_pisaca);
		sleep(1);
		br_pisaca--;
		printf("Pisac %d gotov (br. pisaca = %d)\n", p, br_pisaca);
		pthread_rwlock_unlock(&cp_kljuc);
		sleep(p * 5);
	}
}

int main ()
{
	pthread_t thr_id;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	pthread_create(&thr_id, &attr, (void *(*)(void *)) citac, (void *) 1);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) citac, (void *) 2);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) citac, (void *) 3);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) citac, (void *) 4);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) citac, (void *) 5);

	pthread_create(&thr_id, &attr, (void *(*)(void *)) pisac, (void *) 1);
	pthread_create(&thr_id, &attr, (void *(*)(void *)) pisac, (void *) 2);
		
	sleep(50); //simulation time

	return 0;
}
