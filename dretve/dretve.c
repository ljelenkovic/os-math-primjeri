#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define BROJ_DRETVI 3
#define BROJ_ITERACIJA 5

void *dretva (void *rbr)
{
	int i, *d = rbr;
	for (i = 1; i <= BROJ_ITERACIJA; i++) {
		printf("Dretva %d; iteracija=%d\n", *d, i);
		sleep(1);
	}
	return NULL;
}
int main(int argc, char *argv[])
{
	pthread_t opisnik[BROJ_DRETVI];
	int i, j, id[BROJ_DRETVI];

	for (i = 0; i < BROJ_DRETVI; i++) {
		id[i] = i;
		if (pthread_create(&opisnik[i], NULL, dretva, &id[i])) {
			fprintf(stderr, "Ne mogu stvoriti novu dretvu!\n");
			exit(1);
		}
	}

	for (j = 0; j < BROJ_DRETVI; j++)
		pthread_join(opisnik[j], NULL);

	return 0;
}
