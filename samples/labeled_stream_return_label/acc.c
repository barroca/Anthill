#include <stdlib.h>
#include <stdio.h>
#include "FilterDev.h"

InputPortHandler entacc;
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
	void *buf;
	char *label;
	char *mesg;
	int returnValue;

	while(1){
	  /// returns 0 if there is no data to be received
		if ((returnValue = dsReadNonBlockingBufferMalloc(entacc,&buf, &label)) == EOW){

			fprintf(fp, "Recebi EOW!!\n");
			fflush(fp);
			return 1;
		}

		if (returnValue == 0){
			continue;	
		}
		mesg = (char *) buf;
		sscanf(mesg, "%d", &valor);
		fprintf(fp, "mesg = %s label = %s\n", mesg,label); fflush(fp);
		
		acumulador += valor;
		free(buf);
		free(label);
		
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


