#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/* funkcije za obradu signala, navedene ispod main-a */
void obradi_dogadjaj(int sig);
void obradi_sigterm(int sig);
void obradi_sigint(int sig);

int nije_kraj = 1;

int main()
{
	struct sigaction act;

	/* 1. maskiranje signala SIGUSR1 */

	/* kojom se funkcijom signal obradjuje */
	act.sa_handler = obradi_dogadjaj;

	/* koje jos signale treba blokirati dok se signal obradjuje */
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask, SIGTERM);

	act.sa_flags = 0; /* naprednije mogucnosti preskocene */

	/* maskiranje signala - povezivanje sucelja OS-a */
	sigaction(SIGUSR1, &act, NULL);

	/* 2. maskiranje signala SIGTERM */
	act.sa_handler = obradi_sigterm;
	sigemptyset(&act.sa_mask);
	sigaction(SIGTERM, &act, NULL);

	/* 3. maskiranje signala SIGINT */
	act.sa_handler = obradi_sigint;
	sigaction(SIGINT, &act, NULL);

	printf("Program s PID=%ld krenuo s radom\n", (long) getpid());

	/* neki posao koji program radi; ovdje samo simulacija */
	int i = 1;
	while (nije_kraj) {
		printf("Program: iteracija %d\n", i++);
		sleep(1);
	}

	printf("Program s PID=%ld zavrsio s radom\n", (long) getpid());

	return 0;
}

void obradi_dogadjaj(int sig)
{
	int i;
	printf("Pocetak obrade signala %d\n", sig);
	for (i = 1; i <= 5; i++) {
		printf("Obrada signala %d: %d/5\n", sig, i);
		sleep(1);
	}
	printf("Kraj obrade signala %d\n", sig);
}

void obradi_sigterm(int sig)
{
	printf("Primio signal SIGTERM, pospremam prije izlaska iz programa\n");
	nije_kraj = 0;
}

void obradi_sigint(int sig)
{
	printf("Primio signal SIGINT, prekidam rad\n");
	exit(1);
}
