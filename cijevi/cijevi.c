#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define NAZIV_CIJEVI  "/tmp/pipe_example"
#define N             50

void citac();

int main()
{
	int fd, i;
	char *poruka[] = { "abcd", "ABC", "12345", "X", NULL };

	if (mkfifo(NAZIV_CIJEVI, S_IWUSR | S_IRUSR))
		perror("pisac:mkfifo"); /* možda već postoji */

	if (fork() == 0)
		citac();

	sleep(1); /* i čitač će zbog ovoga čekati! */
	fd = open(NAZIV_CIJEVI, O_WRONLY); /* čekaj da i čitač otvara */
	if (fd == -1) {
		perror("pisac:open");
		return 1;
	}
	printf("pisac otvorio cijev\n");

	for (i = 0; poruka[i] != NULL; i++)
		write(fd, poruka[i], strlen(poruka[i]));
	close(fd);

	wait(NULL);

	unlink(NAZIV_CIJEVI);

	return 0;
}

void citac()
{
	int fd, i, sz;
	char buffer[N];

	fd = open(NAZIV_CIJEVI, O_RDONLY); /* čekaj da i pisač otvara */
	if (fd == -1) {
		perror("citac:open");
		exit(1);
	}
	printf("citac otvorio cijev\n");

	i = 0;
	memset(buffer, 0, N * sizeof(char));
	while ((sz = read(fd, &buffer[i], N * sizeof(char) - i)) > 0)
		i += sz;
	printf("Primljeno: %s\n", buffer);

	exit(0);
}
