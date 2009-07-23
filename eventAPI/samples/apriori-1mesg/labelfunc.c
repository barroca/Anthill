#include <stdlib.h>
#include <stdio.h>


// faz essa aqui se quiser, DS tem um hash proprio
int hash(char *label, int image){
  	int dest;
	
	dest = atoi(label) % image;
	
	return dest;
}



/* essa tem que ter */
void getLabel(void *msg, int size, char label[]){
	int i, itemsSum = 0;
	int *iMsg = (int *)msg;
	int itemSetSize = iMsg[0];

	for(i = 0; i < itemSetSize; i++){
		itemsSum += iMsg[i+1];	
	}
	itemsSum = itemsSum %100;
	sprintf(label, "%d", itemsSum);

}
