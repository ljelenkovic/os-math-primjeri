#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BR_PROCESA 3
#define BR_PROLAZA 10

int a = 0;

void Proces (int i)
{
	int j;

	for (j = 1; j <= BR_PROLAZA; j++) {
		printf ("D: Proces:%2d, prolaz:%2d (pid=%d, ppid=%d)\n",
			i, j, (int) getpid(), (int) getppid());
		a = a + i;
		sleep (1);
	}
	printf ("D: Proces:%2d, a = %d\n", i, a);
}

int main (void)
{
	int i, pid;

	printf ("R: pid=%d, ppid=%d\n", (int) getpid(), (int) getppid());

	for (i = 0; i < BR_PROCESA; i++) {
		switch (pid = fork ()) {
		case -1:
			printf ("R: Ne mogu stvoriti novi proces!\n");
			exit (1);

		case 0:
			Proces (i + 1);
			exit (0);

		default:
			printf ("R: Stvorio proces %d\n", pid);
			break;
		}
	}

	for (i = 0; i < BR_PROCESA; i++)
		wait (NULL);

	printf ("Proces roditelj, a = %d\n", a);

	return 0;
}
