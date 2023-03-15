#include <pthread.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t m;
pthread_cond_t red[2];

char *tsmjer[] = { "jug", "sjever" };

int autiM;	//broj automobila na mostu
int smjerM;	//smjer automobila na mostu: 0 ili 1

int zadnji_id, br_dr;

void *novi_auto (void *p)
{
	int smjerA = rand() & 1;
	int id;

	pthread_mutex_lock(&m);
	br_dr++;
	id = ++zadnji_id;
	while ( autiM == 3 || (autiM > 0 && smjerM != smjerA) )
		pthread_cond_wait(&red[smjerA], &m);
	autiM++;
	smjerM = smjerA;

	printf("Auto %2d na mostu, ide na %s (na mostu %d, idu na %s)\n",
		id, tsmjer[smjerA], autiM, tsmjer[smjerA]);
	pthread_mutex_unlock(&m);

	sleep(5);

	pthread_mutex_lock(&m);
	autiM--;
	if (autiM > 0)
		pthread_cond_signal(&red[smjerA]);
	else
		pthread_cond_broadcast(&red[1 - smjerA]);

	printf("Auto %2d sisao s mosta, ide na %s (na mostu %d, idu na %s)\n",
		id, tsmjer[smjerA], autiM, tsmjer[smjerA]);
	br_dr--;
	pthread_mutex_unlock(&m);

	return NULL;
}

int main ()
{
	pthread_t thr_id;
	pthread_attr_t attr;

	pthread_mutex_init (&m, NULL);
	pthread_cond_init (&red[0], NULL);
	pthread_cond_init (&red[1], NULL);

	//ne ceka se kraj dretve, pa se ona oznacava kao "odvojena"
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	autiM = 0;
	smjerM = 0;
	zadnji_id = 0;
	br_dr = 0;
	srand ((unsigned int) time(NULL));

	while (zadnji_id < 20) {
		pthread_create(&thr_id, &attr, novi_auto, NULL);
		sleep(2);
	}
	while (br_dr > 0)
		sleep(1);

	pthread_mutex_destroy (&m);
	pthread_cond_destroy (&red[0]);
	pthread_cond_destroy (&red[1]);

	return 0;
}
