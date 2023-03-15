#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

/*
 * Dio koda do funkcije obradi_sigint() ne treba previse analizirati
 * (ne pita se to podrucje).
 * Dovoljno je koristiti ga (ali nije ni to nuzno):
 * - umjesto sleep(n) pozvati spavaj(n)
 * - umjesto printf(...) pozvati PRINTF(...)
 */

/*****************************************************************/
static struct timespec t0; /* vrijeme pocetka programa */

/* postavlja trenutno vrijeme u t0 */
void postavi_pocetno_vrijeme()
{
	clock_gettime(CLOCK_REALTIME, &t0);
}

/* dohvaca vrijeme proteklo od pokretanja programa */
void vrijeme(void)
{
	struct timespec t;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec -= t0.tv_sec;
	t.tv_nsec -= t0.tv_nsec;
	if (t.tv_nsec < 0) {
		t.tv_nsec += 1000000000;
		t.tv_sec--;
	}

	printf("%03ld.%03ld:\t", t.tv_sec, t.tv_nsec/1000000);
}

/* ispis kao i printf uz dodatak trenutnog vremena na pocetku */
#define PRINTF(format, ...)       \
do {                              \
  vrijeme();                      \
  printf(format, ##__VA_ARGS__);  \
}                                 \
while(0)

/*
 * spava zadani broj sekundi
 * ako se prekine signalom, kasnije nastavlja spavati neprospavano
 */
void spavaj(time_t sekundi)
{
	struct timespec koliko;
	koliko.tv_sec = sekundi;
	koliko.tv_nsec = 0;

	while (nanosleep(&koliko, &koliko) == -1 && errno == EINTR)
		PRINTF("Bio prekinut, nastavljam\n");
}
/*****************************************************************/

void obradi_sigint(int sig)
{
	PRINTF("SIGINT: zapocinjem obradu\n");
	spavaj(4);
	PRINTF("SIGINT: obrada gotova\n");
}

void inicijalizacija()
{
	struct sigaction act;

	act.sa_handler = obradi_sigint;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = obradi_sigint;
	sigaction(SIGINT, &act, NULL);

	postavi_pocetno_vrijeme();
}

int main()
{
	inicijalizacija();

	PRINTF("G: Pocetak glavnog programa\n");
	spavaj(10);
	PRINTF("G: Kraj glavnog programa\n");

	return 0;
}

/* primjer rada:
$ gcc lab1-primjer_spavanja.c -o lab1ps
$ ./lab1ps
000.000:        G: Pocetak glavnog programa
^C002.046:      SIGINT: zapocinjem obradu
006.047:        SIGINT: obrada gotova
006.047:        Bio prekinut, nastavljam
014.003:        G: Kraj glavnog programa
$
*/