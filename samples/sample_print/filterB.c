#include <stdio.h>
#include <stdlib.h>
#include "filterB.h"
#include <unistd.h>

InputPortHandler input;

int initFilter(void *work, int size) {
	printf("Inicializing filter B\n");

	input = dsGetInputPortByName("input");
	
	return 0;
}

int processFilter(void *work, int size) {	
	char *mesg = (char *)malloc(sizeof(char) * 104857600);
	int tmp;

	printf("Processing filter B\n");

	tmp = dsReadBuffer(input, mesg, 104857600);
//	printf("Recv mesg: %d retornou %d\n", mesg, tmp);

	// Recv EOW
	
/*	while(( tmp = dsReadBuffer(input, mesg, sizeof(int)) ) != EOW ){
		printf("Retornou %d\n", tmp);
	}*/
	
	//tmp = dsReadBuffer(input, mesg, sizeof(int));
	//printf("Retornou %d\n", tmp);

	/*while (1) {
		int i=0;
	}*/
	free(mesg);
	return 0;
}

int finalizeFilter() {
//sleep(60);
	printf("Stopping filter B\n");

	return 0;
}

