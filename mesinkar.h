#ifndef __MESIN_KAR__
#define __MESIN_KAR__

#include "boolean.h"
#include <stdio.h>
#include <string.h>

#define MARK '.'

/* State Mesin */
extern char CC;
extern boolean EOP;
typedef struct
{
    char T[11];
} TabKar;
void START(char* filename);
/*	Mesin siap dioperasikan. Pita disiapkan untuk dibaca.
	Karakter pertama yang ada pada pita posisinya adalah pada jendela.
	I.S. : sembarang
	F.S. : CC adalah karakter pertama pada pita
	       Jika CC !=MARKmaka EOP akan padam (false)
	       Jika CC = MARKmaka EOP akan menyala (true)*/
void ADV();
/* 	Pita dimajukan satu karakter.
	I.S. : Karakter pada jendela = CC,
	CC !=MARK F.S. : CC adalah karakter berikutnya dari CC yang lama,
	CC mungkin = MARK Jika  CC = MARKmaka EOP akan menyala (true)*/

#endif
