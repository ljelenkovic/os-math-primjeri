#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

int ShmId; /* identifikacijski broj segmenta */

#define N 5
int *a, *b, *c; //a[N], b[1], c[1]

void proces (int i)
{
	a[i] = i;
	*b += i;
	*c -= *b;
}

int main (void)
{
	int i;

	ShmId = shmget (IPC_PRIVATE, sizeof(int) * (N + 2), 0600);
	/* jedan blok memorije: 0 1 2 3 4 5 6
	                        <  a[5] > b c */

	if (ShmId == -1)
		exit (1);  /* greška - nema zajedničke memorije */

	a = (int *) shmat (ShmId, NULL, 0);
	b = a + N;
	c = b + 1;

	*b = *c = 0;

	for (i = 0; i < N; i++)
		if (fork () == 0) {
			proces (i);
			exit (0);
		}

	for (i = 0; i < N+2; i++)
		wait (NULL);

	printf ("a=");
	for (i = 0; i < N; i++)
		printf ("%d ", a[i]);
	printf ("b=%d c=%d\n", *b, *c);

	shmdt ((char *) a);
	shmctl (ShmId, IPC_RMID, NULL);

	return 0;
}
