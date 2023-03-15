#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#define BR_DRETVI	5
sem_t KO;
int kraj;

void *dretva (void *p)
{
	int id = *((int *) p);
	printf("Dretva %d pocinje\n", id);
	while (kraj == 0) {
		sem_wait(&KO);
		printf("Dretva %d unutar K.O.\n", id);
		sleep(1 * id);
		printf("Dretva %d izlazi iz K.O.\n", id);
		sem_post(&KO);
		sleep(1);
	}
	printf("Dretva %d zavrsava\n", id);
	return NULL;
}

int main ()
{
	pthread_t thr_id[BR_DRETVI];
	int i, id[BR_DRETVI];

	sem_init(&KO, 0, 1);
	kraj = 0;

	for (i = 0; i < BR_DRETVI; i++ ) {
		id[i] = i + 1;
		if ( pthread_create(&thr_id[i], NULL, dretva, &id[i]) ) {
			perror("Greska pri stvaranju dretve");
			exit(1);
		}
	}

	sleep(10);

	kraj = 1;
	for (i = 0; i < BR_DRETVI; i++ )
		pthread_join (thr_id[i], NULL);

	sem_destroy (&KO);

	return 0;
}
