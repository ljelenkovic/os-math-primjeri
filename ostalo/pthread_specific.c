#include <pthread.h>
#include <malloc.h>
#include <stdio.h>
#include <unistd.h>

//dummy structures
typedef long Status;
typedef long Podaci;

//globalne varijable, dijeljene meðu dretvama
pthread_key_t kStatus, kPodaci;

void *dretva(void *x);
void funkcija();
void oslobodi(void *x);

int main()
{
	pthread_t t1, t2; //opisnici za nove dretve

	//inicijalizacija kljuceva, potrebno za prvatne podatke dretve
	pthread_key_create(&kStatus, oslobodi);
	pthread_key_create(&kPodaci, oslobodi);
	//inicijalno, NULL vrijednost je povezana uz kljuceve za sve dretve

	pthread_create(&t1, NULL, dretva, (void *) 1);
	pthread_create(&t2, NULL, dretva, (void *) 2);

	pthread_join(t1, NULL);
	pthread_join(t2, NULL);

	return 0;
}
	
//radna dretva - inicijalizacija
void *dretva(void *x)
{
	Status *stat;
	Podaci *podaci;

	stat = malloc(sizeof(Status));   //stat - lokalna varijabla
	podaci = malloc(sizeof(Podaci)); //podaci - lokalna varijabla

	*stat = (long) x;
	*podaci = *stat * 10;

	//povezi 'stat' s kljucem 'kStatus' u trenutnoj dretvi
	pthread_setspecific(kStatus, stat);
	//povezi 'podaci' s kljucem 'kPodaci' u trenutnoj dretvi
	pthread_setspecific(kPodaci, podaci);

	sleep(1);
	funkcija();

	return NULL;
}

//radna dretva – u nekoj funkciji
void funkcija()
{
	Status *s;
	Podaci *b;

	//dohvati podatke povezane uz kljuceve
	s = pthread_getspecific(kStatus);
	b = pthread_getspecific(kPodaci);

	//koristi ‘s’ and ‘b’
	printf("stat=%ld, podaci=%ld\n", *s, *b);
}

void oslobodi(void *x)
{
	//printf("Oslobadjanje %p\n", x);
	if (x)
		free(x);
}
