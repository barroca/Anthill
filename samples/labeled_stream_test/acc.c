#include <stdlib.h>
#include <stdio.h>
#include "FilterDev.h"

InputPortHandler entacc;
char mesg[1000];
FILE *fp;
int acumulador;

int initFilter(void *work, int worksize){
	char nomeArq[50];
	sprintf(nomeArq, "saida%d.txt", dsGetMyRank());
	fp = fopen(nomeArq, "w");

	//pega os handlers de saida
	entacc 	= dsGetInputPortByName("entrada");

	return 1;
}

int processFilter(void *work, int worksize){

	int valor;
	acumulador = 0;

	while(1){
	  
		if ((dsReadBuffer(entacc, mesg, 1000)) == EOW){
			fprintf(fp, "Recebi EOW!!\n");
			fflush(fp);
			return 1;
		}

		sscanf(mesg, "%d", &valor);
		fprintf(fp, "%s\n", mesg); fflush(fp);
		
		acumulador += valor;
		
	}
	return 1;
}

int finalizeFilter(void){

	fprintf(fp, "acumulador: %d\n", acumulador);
	fprintf(fp, "memory: %d\n", dsGetMachineMemory());
	fprintf(fp, "numLocalInstances: %d\n", dsGetLocalInstances());	
	fclose(fp);

	
	return 1;
}


