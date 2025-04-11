#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define BRDR 3 /* broj dretvi */
pthread_mutex_t kljuc = PTHREAD_MUTEX_INITIALIZER;

void *dretva(void *p)
{
	int id = *((int*) p);

	while (1)
	{
		sleep(id);
		printf("Dretva %d spremna\n", id);
		pthread_mutex_lock(&kljuc);
		printf("Dretva %d unutar K.O.\n", id);
		sleep(id);
		printf("Dretva %d izlazi iz K.O.\n", id);
		pthread_mutex_unlock(&kljuc);
	}
}

int main()
{
	pthread_t id[BRDR];
	int i, broj[BRDR];

	for (i = 0; i < BRDR; i++) {
		broj[i] = i + 1;
		pthread_create(&id[i], NULL, dretva, (void *) &broj[i]);
	}
		
	sleep(50); /* vrijeme simulacije = 50 s */

	return 0; /* sve će dretve automatski biti prekinute */
}
