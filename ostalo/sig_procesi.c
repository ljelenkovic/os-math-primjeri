/*!
 * Obradjuje signale SIGUSR1, SIGUSR2
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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

void signal_me ()
{
	int i;
	union sigval v;

	v.sival_ptr = NULL;

	for (i = 1; i <= 5; i++)
	{
		sleep (i);
		if (i < 3)
		{
			/* (SIGUSR1,-);(SIGUSR2,-) */
			printf("Saljem obican signal SIGUSR%d\n", 2-(i&1));
			if (i&1)
				kill(parent, SIGUSR1);
			else
				kill(parent, SIGUSR2);
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
}

int main()
{
	struct sigaction act;
	int i;
	pid_t child;

	parent = getpid();
	child = fork();
	if (!child)
	{
		signal_me ();
		exit (0);
	}

	/* definiraj ponasanje za signale na razini procesa */
	/* SIGUSR1 i SIGUSR2 obradjuje funkcija "obrada_signala" */
	act.sa_sigaction = obrada_signala;
	sigemptyset(&act.sa_mask);
	sigaddset(&act.sa_mask , SIGUSR1); /*koji se jos signali blokiraju*/
	sigaddset(&act.sa_mask , SIGUSR2); /*u funkciji obrade ovog prekida*/
	act.sa_flags = SA_SIGINFO; /* uz signal moze ici i dodatni opis */
	sigaction(SIGUSR1, &act, NULL); /* 'registriranje' signala */
	sigaction(SIGUSR2, &act, NULL); /* 'registriranje' signala */

	for (i = 1; i <= 10; i++)
	{
		printf("Glavna dretva (pid=%d)- obrada %d/10\n", parent, i);
		sleep(1);
	}

	waitpid(child, NULL, 0);

	return 0;
}

/*
$ gcc sig_procesi.c -o sig_procesi
$ ./sig_procesi
Glavna dretva (pid=23586)- obrada 1/10
Glavna dretva (pid=23586)- obrada 2/10
Saljem obican signal SIGUSR1

SIGUSR1 - pocetak obrade
SIGUSR1 [pid=23586] - obrada 1/8
SIGUSR1 [pid=23586] - obrada 2/8
Saljem obican signal SIGUSR2
SIGUSR1 [pid=23586] - obrada 3/8
SIGUSR1 [pid=23586] - obrada 4/8
SIGUSR1 [pid=23586] - obrada 5/8
Saljem RT signal SIGUSR1 procesu
SIGUSR1 [pid=23586] - obrada 6/8
SIGUSR1 [pid=23586] - obrada 7/8
SIGUSR1 [pid=23586] - obrada 8/8
SIGUSR1 - kraj obrade


SIGUSR1 - pocetak obrade uz parametar = 3 (0x3)
SIGUSR1 [pid=23586] - obrada 1/8
Saljem RT signal SIGUSR2 procesu
SIGUSR1 [pid=23586] - obrada 2/8
SIGUSR1 [pid=23586] - obrada 3/8
SIGUSR1 [pid=23586] - obrada 4/8
SIGUSR1 [pid=23586] - obrada 5/8
SIGUSR1 [pid=23586] - obrada 6/8
Saljem RT signal SIGUSR1 procesu
SIGUSR1 [pid=23586] - obrada 7/8
SIGUSR1 [pid=23586] - obrada 8/8
SIGUSR1 - kraj obrade


SIGUSR1 - pocetak obrade uz parametar = 5 (0x5)
SIGUSR1 [pid=23586] - obrada 1/8
SIGUSR1 [pid=23586] - obrada 2/8
SIGUSR1 [pid=23586] - obrada 3/8
SIGUSR1 [pid=23586] - obrada 4/8
SIGUSR1 [pid=23586] - obrada 5/8
SIGUSR1 [pid=23586] - obrada 6/8
SIGUSR1 [pid=23586] - obrada 7/8
SIGUSR1 [pid=23586] - obrada 8/8
SIGUSR1 - kraj obrade


SIGUSR2 - pocetak obrade
SIGUSR2 [pid=23586] - obrada 1/8
SIGUSR2 [pid=23586] - obrada 2/8
SIGUSR2 [pid=23586] - obrada 3/8
SIGUSR2 [pid=23586] - obrada 4/8
SIGUSR2 [pid=23586] - obrada 5/8
SIGUSR2 [pid=23586] - obrada 6/8
SIGUSR2 [pid=23586] - obrada 7/8
SIGUSR2 [pid=23586] - obrada 8/8
SIGUSR2 - kraj obrade

Glavna dretva (pid=23586)- obrada 3/10
Glavna dretva (pid=23586)- obrada 4/10
Glavna dretva (pid=23586)- obrada 5/10
Glavna dretva (pid=23586)- obrada 6/10
Glavna dretva (pid=23586)- obrada 7/10
Glavna dretva (pid=23586)- obrada 8/10
Glavna dretva (pid=23586)- obrada 9/10
Glavna dretva (pid=23586)- obrada 10/10
$
 */