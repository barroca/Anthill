#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "FilterDev.h"

OutputPortHandler numP;

int initFilter(void *work, int size){
	//output handler
	numP 	= dsGetOutputPortByName("numberOutput");
	
	return 0;
}

int processFilter(void *work, int size){
	int i=0, *non, num; 
	
	non = (int *)work; 

	//how many numbers are we generating? get from work
	int numJobs = non[0];

	//generate the numbers randomly and send them
	for (i=0;i<numJobs;i++){
		num = i;
		dsWriteBuffer(numP, &num, sizeof(int));
	}

	return 0;
}

int finalizeFilter(void){
	printf("End readNumber\n");

	return 0;
}


