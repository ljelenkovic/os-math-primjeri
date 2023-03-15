#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t sem[2];

char *ispis[] = {"ping", "pong"};

void *dretva (void *p)
{
	int tip = *((int *) p);

	printf ("\tKrece dretva %s\n", ispis[tip]);

	sem_wait (&sem[tip]);
	printf ("%s\n", ispis[tip]);
	sem_post (&sem[1 - tip]);

	return NULL;
}
int main ()
{
	pthread_t t[10];
	int i, redoslijed[10] = {0,1,1,1,0,0,0,0,1,1}; //mora ih biti isto

	sem_init (&sem[0], 0, 1);
	sem_init (&sem[1], 0, 0);

	for (i = 0; i < 10; i++) {
		pthread_create (&t[i], NULL, dretva, &redoslijed[i]);
		sleep(1);
	}
	for (i = 0; i < 10; i++)
		pthread_join (t[i], NULL);

	sem_destroy (&sem[0]);
	sem_destroy (&sem[1]);

	return 0;
}
