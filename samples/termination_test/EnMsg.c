#include <stdlib.h>
#include <stdio.h>
#include "../../FilterDev/FilterDev.h"

OutputPortHandler saidaP;
InputPortHandler entradaP;

int initFilter(void *work, int size){
	//pega os handlers de saida
	saidaP 	= dsGetOutputPortByName("saida");
	
	//pega os handlers de entrada
	entradaP = dsGetInputPortByName("entrada");
	
	return 1;
}

int processFilter(void *work, int worksize){

	int cont, flag;
	char mesg[1000];
	int eow;

	char teste[8];
	
	flag = 1;
	cont = (*(int *) work);

	do{
		while(cont){

			sprintf(teste, "%d", cont--);
			dsWriteBuffer(saidaP, (void*)teste, (sizeof(char) * 8));
		}

		//Vai ficar esperando no
		eow = dsReadBuffer(entradaP, mesg, 1000);
	}while(eow != EOW);

	return 1;
}

int finalizeFilter(void){
	
	return 1;
}


