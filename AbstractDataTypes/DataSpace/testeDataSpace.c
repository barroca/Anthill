
#include <stdlib.h>
#include <string.h>
#include "DataSpace.h"

int main() {
	DataSpace *dataSpace = createDataSpace();
	DataSpace *dataSpace1 = createDataSpace();
	int valSz = 100, valSz1;
	char *val1 = malloc(valSz);
	char *val2 = malloc(valSz);
	char *val3 = malloc(valSz);
	char *val4;
	char *val5;
	char *val6;
	int i;
	FILE *pFile;

	for(i = 0; i < valSz; i++) {
		val1[i] = (char) (random() % 256);
	}
	putData(dataSpace, "teste1", val1, valSz);

	for(i = 0; i < valSz; i++) {
		val2[i] = (char) (random() % 256);
	}
	putData(dataSpace, "teste2", val2, valSz);

	for(i = 0; i < valSz; i++) {
		val3[i] = (char) (random() % 256);
	}
	putData(dataSpace, "teste3", val3, valSz);

	val4 = getData(dataSpace, "teste1", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val1, val4, valSz)) {
		printf("val1 com problema\n");
	}

	val6 = getData(dataSpace, "teste3", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val3, val6, valSz)) {
		printf("val1 com problema\n");
	}
	
	val5 = getData(dataSpace, "teste2", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val2, val5, valSz)) {
		printf("val1 com problema\n");
	}

	pFile = fopen("saidaDataSpace.txt", "w");

	writeDataSpace(pFile, dataSpace);
	
	fclose(pFile);

	pFile = fopen("saidaDataSpace.txt", "r");

	readDataSpace(pFile, dataSpace1);
	
	fclose(pFile);

	val4 = getData(dataSpace, "teste1", &valSz);
	val5 = getData(dataSpace1, "teste1", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val4, val5, valSz)) {
		printf("val1 com problema\n");
	}

	val4 = getData(dataSpace, "teste2", &valSz);
	val5 = getData(dataSpace1, "teste2", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val4, val5, valSz)) {
		printf("val1 com problema\n");
	}

	val4 = getData(dataSpace, "teste3", &valSz);
	val5 = getData(dataSpace1, "teste3", &valSz1);
	if(valSz1 != valSz) {
		printf("problema no valSz1");
	}
	if(memcmp(val4, val5, valSz)) {
		printf("val1 com problema\n");
	}
	
	destroyDataSpace(dataSpace);
	destroyDataSpace(dataSpace1);

	return 0;
}
