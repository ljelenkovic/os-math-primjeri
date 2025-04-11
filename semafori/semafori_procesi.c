#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BR_DRETVI	5
sem_t *KO;
int *kraj;

void proces (int id)
{
	printf("Proces %d pocinje\n", id);
	while (*kraj == 0) {
		sem_wait(KO);
		printf("Proces %d unutar K.O.\n", id);
		sleep(1 * id);
		printf("Proces %d izlazi iz K.O.\n", id);
		sem_post(KO);
		sleep(1);
	}
	printf("Proces %d zavrsava\n", id);
	exit(0);
}

int main ()
{
	int ID, i, id[BR_DRETVI];
	pid_t pid;

	ID = shmget (IPC_PRIVATE, sizeof(sem_t) + sizeof(int), 0600);
	KO = shmat(ID, NULL, 0);
	shmctl(ID, IPC_RMID, NULL); //moze odmah ovdje, nakon shmat

	kraj = (int *) (KO + 1);

	sem_init(KO, 1, 1);
	*kraj = 0;

	for (i = 0; i < BR_DRETVI; i++ ) {
		pid = fork();
		if ( pid == 0 ) {
			id[i] = i + 1;
			proces (id[i]);
		}
		else if (pid == -1) {
			perror("Greska pri stvaranju procesa");
			exit(1);
		}
	}

	sleep(10);

	*kraj = 1;
	for (i = 0; i < BR_DRETVI; i++ )
		wait(NULL);

	sem_destroy(KO);
	shmdt(KO);

	return 0;
}
