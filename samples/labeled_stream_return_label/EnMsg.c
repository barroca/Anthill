#include <stdlib.h>
#include <stdio.h>
#include "../../FilterDev/FilterDev.h"

OutputPortHandler saidaP;

int initFilter(void *work, int worksize){
	//pega os handlers de saida
	saidaP 	= dsGetOutputPortByName("saida");
	
	return 1;
}

int processFilter(void *work, int worksize){

	int cont, flag;

	char teste[8];

	
	flag = 1;
	cont = (*(int *) work);
	while(cont){
		sprintf(teste, "%d", cont--);
		dsWriteBuffer(saidaP, (void*)teste, (sizeof(char) * 8));
	}


	return 1;
}

int finalizeFilter(void){
	
	return 1;
}


