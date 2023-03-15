#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t m;     //monitor
pthread_cond_t red[2]; //redovi uvjeta

char *ispis[] = {"ping", "pong"};
int na_redu = 0, radi = 0;

void *dretva (void *p)
{
	int tip = *((int *) p);

	pthread_mutex_lock (&m);
	printf ("\tKrece dretva %s\n", ispis[tip]);
	while (na_redu != tip || radi)
		pthread_cond_wait (&red[tip], &m);
	radi = 1;
	pthread_mutex_unlock (&m);

	printf ("%s\n", ispis[tip]);

	pthread_mutex_lock (&m);
	radi = 0;
	na_redu = 1 - tip;
	pthread_cond_signal (&red[1-tip]);
	pthread_mutex_unlock (&m);

	return NULL;
}
int main ()
{
	pthread_t t[10];
	int i, redoslijed[10] = {0,1,1,1,0,0,0,0,1,1}; //mora ih biti isto

	pthread_mutex_init (&m, NULL);
	pthread_cond_init (&red[0], NULL);
	pthread_cond_init (&red[1], NULL);

	for (i = 0; i < 10; i++) {
		pthread_create (&t[i], NULL, dretva, &redoslijed[i]);
		sleep(1);
	}
	for (i = 0; i < 10; i++)
		pthread_join (t[i], NULL);

	pthread_mutex_destroy (&m);
	pthread_cond_destroy (&red[0]);
	pthread_cond_destroy (&red[1]);

	return 0;
}
