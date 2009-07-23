#include <stdlib.h>
#include <stdio.h>
#include "FilterDev.h"

InputPortHandler entradaP;

int valor[1];

int initFilter(void *work, int size){
	//pega os handlers de saida
	entradaP = dsGetInputPortByName("entrada");

	return 1;
}

int processFilter(void *work, int size){

	while (dsReadBuffer(entradaP, valor, sizeof(int)) != EOW) {
		printf("Imprimindo: %d\n", *valor);
	}

	return 1;
}

int finalizeFilter(void){


	return 1;
}


