#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Zbog modernih procesora koji izvode instrukcije preko reda
 * ("out-of-order") koristenjem obicnih varijabli bez posebne paznje
 * algoritam vise nije Lamportov, tj. mogu se dogoditi greske.
 * Jedan on nacina kako to izbjeci je posebnim koristenjem tih
 * varijabli kroz makroe *atomic* koji, osim što zadane operacije
 * obavljaju atomarno, osiguravaju ispravan redoslijed izvođenja:
 * - ono sto se nalazi prije *atomic* mora biti gotovo prije
 *   zapocinjanja *atomic* operacije
 * - ono sto se nalazi poslije *atomic* ne smije započeti prije
 *   završetka *atomic* operacije
 */

#define MAX_DR 20
long a, N, M;
long broj[MAX_DR];
long ulaz[MAX_DR];

void Udji_u_KO (int i);
void Izadji_iz_KO (int i);

void *dretva (void *_id)
{
	long i;
	int id = *( (int*) _id );

	for (i = 0; i < M; i++) {
		Udji_u_KO (id);
		a = a + 1;
		Izadji_iz_KO (id);
	}

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

	for (i = 0; i < N; i++) {
		id[i] = i;
		if ( pthread_create(&dretve[i], NULL, dretva, &id[i]) ) {
			perror ("Ne mogu stvoriti dretvu");
			return -1;
		}
	}

	for (i = 0; i < N; i++)
		pthread_join (dretve[i], NULL);

	printf ("Konacna vrijednost za a=%ld\n", a);

	return 0;
}

void Udji_u_KO (int i)
{
	long j, ulazJ, brojJ;

	__atomic_store_n(&ulaz[i], 1, __ATOMIC_SEQ_CST); //ulaz[i] = 1
	broj[i] = 0;
	for (j = 0; j < N; ++j)
		if (broj[j] >= broj[i])
			__atomic_store_n(&broj[i], broj[j] + 1, __ATOMIC_SEQ_CST); //broj[i] = broj[j] + 1
	__atomic_store_n(&ulaz[i], 0, __ATOMIC_SEQ_CST); //ulaz[i] = 0

	for (j = 0; j < N; ++j){
		do __atomic_load(&ulaz[j], &ulazJ, __ATOMIC_SEQ_CST); //ulazJ = ulaz[j]
		while (ulazJ == 1);

		do __atomic_load(&broj[j], &brojJ, __ATOMIC_SEQ_CST); //brojJ = broj[j]
		while (brojJ != 0 && (brojJ < broj[i] || (brojJ == broj[i] && j < i)));
	}
}

void Izadji_iz_KO (int i)
{
	__atomic_store_n(&broj[i], 0, __ATOMIC_SEQ_CST); //broj[i] = 0
}

/*
kompajlirati sa:
$ gcc a-eq-a+1-Lamport+atomic_store.c -pthread
$

pokretati sa npr.:
$ ./a.out 10 1000000
Konacna vrijednost za a=10000000
$
Vidi se da je 10000000 koliko i treba biti ako je sinkronizacija ispravna

Izmjeriti trajanje:
$ time ./a.out 10 1000000
Konacna vrijednost za a=10000000

real    0m3.998s
user    0m39.753s
sys     0m0.000s
$
*/

