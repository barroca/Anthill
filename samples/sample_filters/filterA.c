#include <stdio.h>
#include "filterA.h"


int initFilter(void *work, int size) {
	printf("Inicializing filter A\n");
	return 0;
}

int processFilter(void *work, int size) {	
	printf("Processing filter A\n");
	return 0;
}

int finalizeFilter() {
	printf("stoping filter A\n");
	return 0;
}

