#include <stdlib.h>
#include <stdio.h>
#include "FilterDev.h"

InputPortHandler entacc;
OutputPortHandler saidacc;
char mesg[1000];
FILE *fp;
int acumulador;

int initFilter(void *work, int size){
	char nomeArq[50];
	sprintf(nomeArq, "saida%d.txt", dsGetMyRank());
	fp = fopen(nomeArq, "w");

	//pega os handlers de entrada
	entacc 	= dsGetInputPortByName("entrada");

	//pega os handlers de saida
	saidacc = dsGetOutputPortByName("saida");

	return 1;
}

int processFilter(void *work, int size){

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

	dsWriteBuffer(saidacc, (void *)"mesgFinal", sizeof(char)*10);
	return 1;
}

int finalizeFilter(void){

	fprintf(fp, "acumulador: %d\n", acumulador);
	fprintf(fp, "memory: %d\n", dsGetMachineMemory());
	fprintf(fp, "numLocalInstances: %d\n", dsGetLocalInstances());	
	fclose(fp);

	
	return 1;
}


