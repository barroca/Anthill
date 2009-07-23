#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "void.h"

#define VAL_SIZE 10

OutputPortHandler output;
InputPortHandler input;

int *numToPrint;

int initFilter(void *work, int size) {
	// Pega o Work 
	numToPrint = (int *) work;

	fprintf(stderr, "Inicializing filter A\n");

	// Pega porta de saida para o FiltroB
	output = dsGetOutputPortByName("output");

	// Pega porta de entra que vem do FiltroB
	input = dsGetInputPortByName("input1");
	
	return 0;
}

int processFilter(void *work, int size) {
	// numToPrint: variavel que contem o work
  	int taskId = numToPrint[0] +1;
	int oldTaskId;

	// alocando espaco para as mensagens que receberei
	int *mesg = (int *) malloc(sizeof(int));
	
	// Usado para identificar os dados salvos
	char id[] = "identificador\0";
	char val[VAL_SIZE] = "\0";
	
	// Variavel auxiliar usada para receber dados salvos
	char *valGet = NULL;
	
	int currentTask;
	int tmp, i;

	fprintf(stderr, "Processing filter A\n");

	//cria a primeira tarefa e envida numToPrint para o proximo filtro
	// no pipeline
	dsCreateTask(taskId, NULL, 0, (char *)numToPrint, sizeof(int));
	oldTaskId = taskId;
	taskId++;

	
	for(i = 0; i < 9; i++){
		sprintf(val, "val%d\n", oldTaskId);
		
		// guarda dados da ultima tarefa criada
		dsPutData(id, val, VAL_SIZE);

		// cria tarefa que depende da tarefa criada anteriormente. A mensagem
		// com a criacao dessa tarefa soh eh gerada apos o termino da anterior
		dsCreateTask(taskId, &oldTaskId, 1, (char *)numToPrint, sizeof(int));
		oldTaskId = taskId;
		taskId++;
	}

	// recebe a resposta da primeira tarefa criada
	tmp = dsReadBuffer(input, mesg, sizeof(int));
	// imprime a tarefa corrente
	currentTask = dsGetCurrentTask();
	fprintf(stderr, "CurrentTask =  %d \n", currentTask);
	// finaliza tarefa corrente
	dsEndTask(currentTask);

	for(i = 0; i < 9; i++){
		// recebe reposta de uma dada tarefa 
		tmp = dsReadBuffer(input, mesg, sizeof(int));
		currentTask = dsGetCurrentTask();

		// pega dado com identificador id da tarefa anterior
		int valSz = 0;
		valGet = dsGetData((currentTask -1), id, &valSz);
		dsRemoveData(id);

		fprintf(stderr, "CurrentTask =  %d ValGet = %s\n", currentTask, (char *)valGet);
		// finaliza tarefa corrente
		dsEndTask(currentTask);
	}
	
	return 0;
}

int finalizeFilter() {
	int *mesg = (int *) malloc(sizeof(int));

	// Espera o EOW proveniente do filtroA. 
	if(dsReadBuffer(input, mesg, sizeof(int)) == EOW){
		fprintf(stderr, "stoping filter A. EOW received\n");
	}

	return 0;
}

