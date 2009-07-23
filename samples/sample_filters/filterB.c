#include <stdio.h>
#include "filterB.h"


int initFilter(void *work, int size) {
	printf("Inicializing filter B\n");
	return 0;
}

int processFilter(void *work, int size) {	
	printf("Processing filter B\n");
	return 0;
}

int finalizeFilter() {
	printf("Stopping filter B\n");
	return 0;
}

