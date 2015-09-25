# include "buffer.h"
/*
#include "boolean.h"
#include <stdio.h>
#include <stdlib.h>

#define IdxMax 100
#define IdxMin 1

int indeks;

typedef struct {
	char TI[IdxMax+1];
} Tab;

*/

/*void MakeEmpty(Tab *T)
{

}*/

void Add(Tab *T, char X)
{
    (*T).TI[indeks] = X;
    indeks++;
    NbElmt++;
}

boolean IsUpper(Tab T)
{
    if (NbElmt >= UpperLimit)
    {
        return true;
    }
    else
    {
        return false;
    }

}

boolean IsLower(Tab T)
{
    if (NbElmt <= LowerLimit)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void printbuffer(Tab T)
{
    /*printf("%c", T.TI[1]);
    while (gsr <= indeks)
    {
        temp = T.TI[gsr];
        T.TI[1] = temp;
        gsr++;
    }
    indeks--;
    NbElmt--;
    gsr = 2;*/
}
