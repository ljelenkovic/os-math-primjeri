# Primjeri s dretvama

## dretve.c

Primjer koji demonstrira sučelja za stvaranje dretvi, prijenos parametara u dretvu, čekanje na kraj dretvi.

## a-eq-a+1.c

Više dretvi paralelno povećava varijablu a za jedan u petlji. Obzirom da rade paralelno moguće je (za veće broje iteracija petlji) da konačna vrijednost ne bude broj_dretvi x broj_iteracija.

Zašto?

Operacija a = a + 1 se sastoji od tri podoperacije:
1. dohvat a iz memorije
2. povećaj za jedan
3. pohrani novu vrijednost u memoriju

Ako dretve paralelno rade, dvije ili više mogu dohvatiti istu vrijednost pa ju obje uvećati za 1 (ukupno povećanje je samo za jedan, a ne dva). Moguće su i gore situacije, kad jedna dretva zastane (raspoređivač ju makne s procesora) prije nego li je pohranila svoj rezultat. Tada su sva povećanja u međuvremenu odbačena kad se ova dretva vrati i zapiše puno manju vrijednost.

## a-eq-a+1-Lamport.c

Slično prethodnom primjeru, ali sada je povećanje stavljeno u kritični odsječak zaštićen Lamportovim algoritmom.

Kada bi on bio ispravno izveden rezultat bi trebao biti broj_dretvi x broj_iteracija.

Međutim, zbog modernih procesora koji mogu obaviti instrukcije izvan reda, to ne mora biti tako.

Također, zbog sinkronizacije ovaj je program značajno sporiji od prethodnog s istim brojem dretvi i iteracija.

## a-eq-a+1-Lamport+atomic_int.c

Varijable koje se koriste u Lamportovu algoritmu nisu obični cijeli brojevi već je korišten tip atomic_int. To nalaže prevoditelju (gcc-u) da s njima postupa "vrlo oprezno", pa se time i izbjegava problem spomenut u prethodnom primjeru.
Međutim, zbog toga je ovaj program još sporiji.

## a-eq-a+1-Lamport+atomic_store.c

Drukčiji pristup rješavanju gornjeg problema s atomarnim operacijama nad varijablama koje koristi Lamportov algoritam.

## a-eq-a+1-atomic_fetch_add.c

Povećanje zajedničke varijable izvedeno atomarnom operacijom.
Ovdje je to moguće jer je operacija jednostavna. Za složenije operacije nad zajedničkim podacima bi ipak trebalo koristiti druge metode (iz ostalih primjera).
