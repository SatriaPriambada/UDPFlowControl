#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include "boolean.h"
#include <stdio.h>
#include <stdlib.h>

#define IdxMax 20
#define IdxMin 1
#define UpperLimit 5
#define LowerLimit 2

int indeks = 1;
int NbElmt = 0;
int gsr = 2;
char temp;

typedef struct {
	char TI[IdxMax+1];
} Tab;

//void MakeEmpty(Tab *T);
void Add(Tab *T, char X);
boolean IsUpper(Tab T);
boolean IsLower(Tab T);
void printbuffer(Tab T);
#endif // BUFFER_H_INCLUDED
