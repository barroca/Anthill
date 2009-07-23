#include <stdio.h>
#include <stdlib.h>
#include "void.h"
#include <unistd.h>

InputPortHandler input;
OutputPortHandler output;

int initFilter(void *work, int size) {
	printf("Inicializing filter B\n");

	// Pega porta de entrada do filtroA
	input = dsGetInputPortByName("input");

	// Pega porta de saida para o filtroA
	output = dsGetOutputPortByName("output1");
	
	return 0;
}

int processFilter(void *work, int size) {	
	int *mesg = (int *) malloc(sizeof(int));
	int tmp, i;

	// loop esperando as dez tarefas criada no filtroA
	for(i = 0; i <10; i++){
		printf("Processing filter B\n");

		// recebe mesg enviada por A
		tmp = dsReadBuffer(input, mesg, sizeof(int));

//		printf("Recv mesg: %d retornou %d taskId = %d\n", *mesg, tmp, dsGetCurrentTask());
		// Envia mensagem recebida de volta para filtroA
		dsWriteBuffer(output, (void *)mesg, sizeof(int));

	}

	// Recebe EOW do filtroA
	tmp = dsReadBuffer(input, mesg, sizeof(int));

	return 0;
}

int finalizeFilter() {

	printf("Stopping filter B\n");

	return 0;
}

