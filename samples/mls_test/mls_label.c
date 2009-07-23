#include <stdlib.h>
#include <stdio.h>

/* extracts the label from the message */
void getLabel(void *msg, int size, char label[]){
	
	sprintf(label, "%d", ((int*)msg)[0]);

}

/* hash the label */
void mlshash(char label[], int numDestinations, int destArray[]){
	int i, num = atoi(label);

	//send the message to all instances which divide this number
	//so instance 0 should get all messages
	for (i=0;i<numDestinations;i++){
		if ((num % (i+1)) == 0){
			destArray[i] = 1;
		}
		else {
			destArray[i] = 0;
		}
	}	
}

