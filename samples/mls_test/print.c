#include <stdlib.h>
#include <stdio.h>
#include "FilterDev.h"

InputPortHandler ip;

int initFilter(void *work, int size){
	//input handler
	ip 	= dsGetInputPortByName("incoming");

	return 0;
}

int processFilter(void *work, int size){
	int whoami = dsGetMyRank(), number;
	FILE *fp;
	char filename[50];

	sprintf(filename, "output_%d.txt", whoami);

	fp = fopen(filename, "w");
	if (fp == NULL){
		//invalid file, lets exit
		dsExit("Invalid filename");
	}

	while (dsReadBuffer(ip, &number, sizeof(int)) != EOW) {
		fprintf(fp, "%d\n", number);
	}
	fclose(fp);

	return 0;
}

int finalizeFilter(void){

	return 0;
}


