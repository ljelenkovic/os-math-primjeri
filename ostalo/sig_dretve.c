/*!
 * Obradjuje signale SIGUSR1, SIGUSR2
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

void obrada_signala(int sig, siginfo_t *info, void *context)
{
	char *ime[] = {"SIGUSR1", "SIGUSR2"};
	int i, j;

	if (sig == SIGUSR1)
		i = 0;
	else
		i = 1;

	printf("\n%s - pocetak obrade", ime[i]);
	if (info != NULL && info->si_code == SI_QUEUE)
		printf(" uz parametar = %d (%p)\n", info->si_value.sival_int,
			 info->si_value.sival_ptr);
	else
		printf("\n");


	for (j = 1; j <= 8; j++)
	{
		printf("%s [pid=%d] - obrada %d/8\n",
			 ime[i], (int) getpid(), j);
		sleep(1);
	}

	printf("%s - kraj obrade\n\n", ime[i]);
}

static pid_t parent;
static pthread_t main_thread;

void *signal_me(void *p)
{
	int i;
	union sigval v;

	v.sival_ptr = NULL;

	/* zabrani prihvat prekida u ovoj dretvi! */
	sigset_t sigmask;
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR1);
	sigaddset(&sigmask, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &sigmask, NULL);

	for (i = 1; i <= 5; i++)
	{
		sleep (i);

		if (i < 3)
		{
			/* (SIGUSR1,-);(SIGUSR2,-) */
			printf("Saljem obican signal SIGUSR%d dretvi\n", 2-(i&1));
			if (i&1)
				pthread_kill(main_thread, SIGUSR1);
			else
				pthread_kill(main_thread, SIGUSR2);
		}
		else {
			/* (SIGUSR1,3); (SIGUSR2,4); (SIGUSR1,5) */
			printf("Saljem RT signal SIGUSR%d procesu\n", 2-(i&1));
			v.sival_int = i;
			if (i&1)
				sigqueue(parent, SIGUSR1, v);
			else
				sigqueue(parent, SIGUSR2, v);
		}
	}

	return NULL;
}

int main()
{
	struct sigaction act;
	sigset_t sigmask;
	int sig;
	pthread_t signaler;

	parent = getpid();
	main_thread = pthread_self();
	pthread_create(&signaler, NULL, signal_me, NULL);

	/* SIGUSR2 je blokiran, ali se moze dohvatiti sa sigwait */
	sigemptyset(&sigmask);
	sigaddset(&sigmask, SIGUSR2);
	pthread_sigmask(SIG_BLOCK, &sigmask, NULL);

	/* definiraj ponasanje za signale na razini procesa */
	/* SIGUSR1 obradjuje funkcija "obrada_signala" */
	act.sa_sigaction = obrada_signala;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask , SIGUSR1); /*koji se jos signali blokiraju*/
	sigaddset(&act.sa_mask , SIGUSR2); /*u funkciji obrade ovog prekida*/
	act.sa_flags = SA_SIGINFO; /* uz signal moze ici i dodatni opis */
	sigaction(SIGUSR1, &act, NULL); /* 'registriranje' signala */

	printf("Glavna dretva - cekam na signal SIGUSR2\n");
	sigwait(&sigmask, &sig);
	printf("Glavna dretva - dohvatio signal %d\n", sig);
	sigwait(&sigmask, &sig);
	printf("Glavna dretva - dohvatio signal %d\n", sig);

	pthread_join(signaler, NULL);

	return 0;
}

/* primjer pokretanja:
$ gcc sig_dretve.c -o sig_dretve -lpthread
$ ./sig_dretve
Glavna dretva - cekam na signal SIGUSR2
Saljem obican signal SIGUSR1 dretvi

SIGUSR1 - pocetak obrade
SIGUSR1 [pid=23572] - obrada 1/8
SIGUSR1 [pid=23572] - obrada 2/8
Saljem obican signal SIGUSR2 dretvi
SIGUSR1 [pid=23572] - obrada 3/8
SIGUSR1 [pid=23572] - obrada 4/8
SIGUSR1 [pid=23572] - obrada 5/8
Saljem RT signal SIGUSR1 procesu
SIGUSR1 [pid=23572] - obrada 6/8
SIGUSR1 [pid=23572] - obrada 7/8
SIGUSR1 [pid=23572] - obrada 8/8
SIGUSR1 - kraj obrade


SIGUSR1 - pocetak obrade uz parametar = 3 (0x3)
SIGUSR1 [pid=23572] - obrada 1/8
Saljem RT signal SIGUSR2 procesu
SIGUSR1 [pid=23572] - obrada 2/8
SIGUSR1 [pid=23572] - obrada 3/8
SIGUSR1 [pid=23572] - obrada 4/8
SIGUSR1 [pid=23572] - obrada 5/8
SIGUSR1 [pid=23572] - obrada 6/8
Saljem RT signal SIGUSR1 procesu
SIGUSR1 [pid=23572] - obrada 7/8
SIGUSR1 [pid=23572] - obrada 8/8
SIGUSR1 - kraj obrade


SIGUSR1 - pocetak obrade uz parametar = 5 (0x5)
SIGUSR1 [pid=23572] - obrada 1/8
SIGUSR1 [pid=23572] - obrada 2/8
SIGUSR1 [pid=23572] - obrada 3/8
SIGUSR1 [pid=23572] - obrada 4/8
SIGUSR1 [pid=23572] - obrada 5/8
SIGUSR1 [pid=23572] - obrada 6/8
SIGUSR1 [pid=23572] - obrada 7/8
SIGUSR1 [pid=23572] - obrada 8/8
SIGUSR1 - kraj obrade

Glavna dretva - dohvatio signal 12
Glavna dretva - dohvatio signal 12
$
*/