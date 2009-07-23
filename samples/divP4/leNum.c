#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "FilterDev.h"

OutputPortHandler numP;

int initFilter(void *work, int size){
	
	//pega os handlers de saida
	numP 	= dsGetOutputPortByName("saida");
	
	return 1;
}

int processFilter(void *work, int size){

	//le o dividendo
	int *num_jobs = (int *)work;

	dsWriteBuffer(numP, num_jobs, sizeof(int));

	return 1;
}

int finalizeFilter(void){
	printf("Fim leNum PNC geral\n");

	return 1;
}


