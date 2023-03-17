#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

/*
 * Povećanje obavljeno atomarnom operacijom.
 * Ovdje je to moguće jer je operacija jednostavna. Za složenije
 * operacije nad zajedničkim podacima bi ipak trebalo koristiti
 * druge metode (iz ostalih primjera).
 */


#define MAX_DR 20
long a, N, M;
long ulaz[MAX_DR];
long broj[MAX_DR];

void *dretva (void *arg)
{
	long i;

	for (i = 0; i < M; i++)
		__atomic_fetch_add(&a, 1, __ATOMIC_SEQ_CST);

	return NULL;
}

int main (int argc, char **argv)
{
	long i;
	pthread_t dretve[MAX_DR];
	int id[MAX_DR];

	if (argc != 3) {
		fprintf (stderr, "Upotreba: %s br_dretvi br_iteracija\n", argv[0]);
		return 0;
	}

	sscanf (argv[1], "%ld", &N);
	sscanf (argv[2], "%ld", &M);

	if (N < 1 || N > MAX_DR || M < 1) {
		printf ("Krivi parametri: br_dretvi [1-20], br_iteracija [1-oo>\n");
		return -1;
	}

	for (i = 0; i < N; i++)
		ulaz[i] = broj[i] = 0;

	a = 0;
	for (i = 0; i < N; i++)
		if (pthread_create(&dretve[i], NULL, dretva, NULL)) {
			perror ("Ne mogu stvoriti dretvu");
			return -1;
		}

	for (i = 0; i < N; i++)
		pthread_join (dretve[i], NULL);

	printf ("Konacna vrijednost za a=%ld (optimalno %ld)\n",
		a, N * M);

	return 0;
}

// kompajlirati sa: gcc a-eq-a+1.c -pthread
// pokretati sa npr.: ./a.out 10 1000000
