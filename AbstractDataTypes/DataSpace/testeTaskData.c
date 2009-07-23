
#include <stdlib.h>
#include <string.h>
#include "TaskData.h"

int main() {
	TaskData *taskData1 = createTaskData();
	TaskData *taskData2 = createTaskData();
	int valSz1 = 100;
	int valSz2;
	char *val1 = malloc(valSz1);
	char *val2 = malloc(valSz1);
	int i;
	FILE *pFile;

	for(i = 0; i < valSz1; i++) {
		val1[i] = (char) (random() % 256);
	}

	putTaskDataSz(taskData1, valSz1);
	putTaskDataVal(taskData1, val1, valSz1);

	valSz2 = getTaskDataSz(taskData1);
	if(valSz1 != valSz2)
		printf("ERROR: tamanho do dado retornado:%d difere de:%d ", valSz1, valSz2);

	memcpy(val2, getTaskDataVal(taskData1), taskData1->valSz);
	if(memcmp(val1, val2, valSz1))
		printf("ERROR: string armazenada:%s difere de:%s", val1, val2);

	pFile = fopen("saidataskData.txt", "w");
	
	writeTaskData(pFile, taskData1);
	writeTaskData(pFile, taskData1);
	writeTaskData(pFile, taskData1);

	fclose(pFile);

	pFile = fopen("saidataskData.txt", "r");

	taskData2 = readTaskData(pFile);
	if(taskData1->valSz != taskData2->valSz)
		printf("ERROR: tamanho do dado lido:%d difere de:%d ", taskData2->valSz, taskData1->valSz);
	if(memcmp(taskData1->val, taskData2->val, taskData1->valSz))
		printf("ERROR: string armazenada:%s difere de:%s", taskData2->val, taskData1->val);

	taskData2 = readTaskData(pFile);
	if(taskData1->valSz != taskData2->valSz)
		printf("ERROR: tamanho do dado lido:%d difere de:%d ", taskData2->valSz, taskData1->valSz);
	if(memcmp(taskData1->val, taskData2->val, taskData1->valSz))
		printf("ERROR: string armazenada:%s difere de:%s", taskData2->val, taskData1->val);

	taskData2 = readTaskData(pFile);
	if(taskData1->valSz != taskData2->valSz)
		printf("ERROR: tamanho do dado lido:%d difere de:%d ", taskData2->valSz, taskData1->valSz);
	if(memcmp(taskData1->val, taskData2->val, taskData1->valSz))
		printf("ERROR: string armazenada:%s difere de:%s", taskData2->val, taskData1->val);

	fclose(pFile);

	destroyTaskData(taskData1);

	return 0;
}
