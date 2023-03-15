#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define BR_DRETVI	5
#define BROJI_DO	25

pthread_mutex_t m;  //monitor
pthread_cond_t red; //red uvjeta

int broj; //1, 2, ...
int uzeo_broj = 0;
time_t zadnja_obrada = 0;

void *dretva (void *p);
void *kada_nitko_nece(void *p);

int main ()
{
	pthread_t t[BR_DRETVI + 1];
	int i, id[BR_DRETVI];

	pthread_mutex_init (&m, NULL);
	pthread_cond_init (&red, NULL);
	broj = 2;
	zadnja_obrada = time(NULL);

	for (i = 0; i < BR_DRETVI; i++) {
		id[i] = i + 2;
		pthread_create (&t[i], NULL, dretva, &id[i]);
	}
	pthread_create (&t[i], NULL, kada_nitko_nece, NULL);

	for (i = 0; i < BR_DRETVI + 1; i++)
		pthread_join (t[i], NULL);

	pthread_mutex_destroy (&m);
	pthread_cond_destroy (&red);

	return 0;
}

void *dretva (void *p)
{
	int id = *((int *) p);

	pthread_mutex_lock (&m);
	while (broj <= BROJI_DO) {
		while (broj <= BROJI_DO && (uzeo_broj != 0 || broj % id != 0))
			pthread_cond_wait (&red, &m);
		if (broj > BROJI_DO)
			break;
		uzeo_broj = 1;
		pthread_mutex_unlock (&m);

		printf ("Dretva %d radi s brojem %d\n", id, broj);
		sleep(1);
		printf ("Dretva %d staje s radom\n", id);

		pthread_mutex_lock (&m);
		uzeo_broj = 0;
		broj++;
		zadnja_obrada = time(NULL);
		pthread_mutex_unlock (&m);
		pthread_cond_broadcast (&red);
		sleep(BR_DRETVI - (id - 1));
		pthread_mutex_lock (&m);
	}
	pthread_mutex_unlock (&m);
	pthread_cond_broadcast (&red);

	return NULL;
}

void *kada_nitko_nece(void *p)
{
	pthread_mutex_lock (&m);
	while (broj <= BROJI_DO) {
		if (uzeo_broj != 0) {
			//netko je uzeo broj
			pthread_cond_wait (&red, &m);
		}
		else if (zadnja_obrada + BR_DRETVI > time(NULL)) {
			//daj im vremena da uzmu broj
			pthread_mutex_unlock (&m);
			sleep(zadnja_obrada + BR_DRETVI - time(NULL));
			pthread_mutex_lock (&m);
		}
		else {
			printf("Nitko nece broj %d, povecavam ga\n", broj);
			broj++;
			zadnja_obrada = time(NULL);
			pthread_cond_broadcast (&red);
		}
	}
	pthread_mutex_unlock (&m);
}
