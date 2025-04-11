/*! Primjer koristenja zajednickog spremnika POSIX suceljem */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

struct djeljeno {
	int b1, b2, b3;
};

#define VELZS	sizeof (struct djeljeno)
#define ZSIME	"/lab4ZS"

/*!
 * Inicijalizacija zajednickog spremnika i sl.
 * Ne poziva se izravno, vec iz ostalih funkcija (kao prva stvar)
 */
static void *inicijaliziraj_zs ()
{
	char *imezs;
	int id;
	void *zs;

	imezs = getenv ( "ZS_IME" ); /* dohvati varijablu okoline */
	if ( imezs == NULL )
		imezs = ZSIME; /* ako varijabla ne postoji uzmi ovo ime */

	/* otvori (ili i stvori) opisnik zajednickog spremnika (shm_open),
	 * postavi velicinu (ftruncate); pogledati u '/dev/shm' (na Linuxu) */
	id = shm_open ( imezs, O_CREAT | O_RDWR, 00600 );
	/* parametri: ime datoteke; 'stvori ako ne postoji', 'za citanje i
	 * pisanje'; ako se datoteka stvara postavi dozvole: rw- --- --- */

	if ( id == -1 || ftruncate ( id, VELZS ) == -1)
	{
		perror ("shm_open ili ftruncate");
		exit(1);
	}

	/* 'mapiraj' zadanu datoteku (id) u adresni prostor procesa */
	zs = mmap ( NULL, VELZS, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0 );
	/* parametri: NULL => 'postavi bilo gdje'; velicina mapiranog dijela,
	 * 'za citanje i pisanje'; 'djeljeno medju procesima'; id => datoteka;
	 * pomak od pocetka datoteke (0 - od pocetka) */

	if ( zs == (void *) -1)
	{
		perror ("mmap");
		exit(1);
	}

	/* mapiranje je uspjelo, opisnik 'datoteke' vise nije potreban */
	close ( id );

	return zs;
}

static void obrisi_zs ( void *zs )
{
	char *imezs;

	/* makni zs iz adresnog prostora procesa */
	munmap ( zs, VELZS );

	imezs = getenv ( "ZS_IME" );
	if ( imezs == NULL )
		imezs = ZSIME;

	/* obrisi zs, tj. ime pod kojim se vodi u sustavu */
	shm_unlink ( imezs );
}

int main ( void )
{
	struct djeljeno *d;

	d = inicijaliziraj_zs ();
	/* kad se segment prvi puta dodijeli on je popunjen nulama */
	if ( d->b1 == 0 )
		d->b1 = d->b2 = d->b3 = 1;

	printf ( "Roditelj: pid=%d, b1=%d, b2=%d, b3=%d\n", (int) getpid(),
		d->b1, d->b2, d->b3 );

	if ( !fork() )
	{
		sleep (1);
		if ( d->b2 == 1 )
			d->b2++;
		else
			d->b3++;
		printf ( "Dijete:   pid=%d, b1=%d, b2=%d, b3=%d\n", (int) getpid(),
			d->b1, d->b2, d->b3 );
		exit (0);
	}

	sleep(2);

	printf ( "Roditelj: pid=%d, b1=%d, b2=%d, b3=%d\n", (int) getpid(),
		d->b1, d->b2, d->b3 );

	obrisi_zs (d);

	/* za test, zakomentirati obrisi_zs i ponavljati pokretanje
	 * u /dev/shm/ potraziti zajednicku memoriju (lab4ZS)
	 * na kraju ipak otkomentirati da se zajednicki spremnik obrise
	 */

	return 0;
}

// prevodjenje: koristiti zastavicu -lrt ako je potrebno
// $ gcc shm_open.c -lrt
