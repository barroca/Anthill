#include <stdlib.h>
#include <stdio.h>
#include <void.h>

InputPortHandler dividendoP;
OutputPortHandler restoP;
OutputPortHandler quocienteP;

int initFilter(void *work, int size){
	//pega os handlers de saida
	dividendoP 	= dsGetInputPortByName("dividendoP");
	restoP 		= dsGetOutputPortByName("resto");
	quocienteP 	= dsGetOutputPortByName("quociente");

	return 1;

}

int processFilter(void *work, int size){
	int dividendo[1];
	int divisor = 4;
	int resto;
	int quociente;
 
	printf("Mim ser divP4\n");
	//le o dividendo
	while ((dsReadBuffer(dividendoP, dividendo, sizeof(int))) != EOW) {
		quociente = *dividendo / divisor;
		resto = *dividendo % divisor;

		//escreve pra filtro resto
		dsWriteBuffer(restoP, &resto, sizeof(int));
		//escreve para filtro quociente
		dsWriteBuffer(quocienteP, &quociente, sizeof(int));
	}

	return 1;
}

int finalizeFilter(void){
	return 1;
}


