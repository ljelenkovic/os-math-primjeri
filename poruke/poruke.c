#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <sys/wait.h>
#include <fcntl.h>

#define NAZIV_REDA           "/msgq_example_name"
#define MAX_PORUKA_U_REDU    5
#define MAX_VELICINA_PORUKE  20

int proizvodjac();
int potrosac();

int main(void)
{
	proizvodjac();

	sleep(1);
	if (!fork()) {
		potrosac();
		exit(0);
	}
	wait(NULL);

	mq_unlink(NAZIV_REDA);

	return 0;
}

int proizvodjac()
{
	mqd_t opisnik_reda;
	struct mq_attr attr;
	char poruka[] = "primjer sadrzaja";
	size_t duljina = strlen (poruka) + 1;
	unsigned prioritet = 10;

	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_PORUKA_U_REDU;
	attr.mq_msgsize = MAX_VELICINA_PORUKE;
	opisnik_reda = mq_open(NAZIV_REDA, O_WRONLY | O_CREAT, 00600, &attr);
	if (opisnik_reda == (mqd_t) -1) {
		perror("proizvodjac:mq_open");
		return -1;
	}
	if (mq_send(opisnik_reda, poruka, duljina, prioritet)) {
		perror("mq_send");
		return -1;
	}
	printf("Poslano: %s [prio=%d]\n", poruka, prioritet);

	return 0;
}

int potrosac()
{
	mqd_t opisnik_reda;
	char poruka[MAX_VELICINA_PORUKE];
	size_t duljina;
	unsigned prioritet;

	opisnik_reda = mq_open(NAZIV_REDA, O_RDONLY);
	if (opisnik_reda == (mqd_t) -1) {
		perror("potrosac:mq_open");
		return -1;
	}
	duljina = mq_receive(opisnik_reda, poruka, MAX_VELICINA_PORUKE,
							&prioritet);
	if (duljina < 0) {
		perror("mq_receive");
		return -1;
	}
	printf("Primljeno: %s [prio=%d]\n", poruka, prioritet);

	return 0;
}
