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
	
	sprintf(label, "%s", (char*)msg);

}
