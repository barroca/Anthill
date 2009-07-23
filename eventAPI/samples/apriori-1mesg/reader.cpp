#include "reader.h"

/*void reader::sendDB(char *dbName, streamOutputHandler outPut){
//	char line[NUM_ATTR+10];
	char line[NUM_LINES][NUM_ATTR+10];
	int *mesg[NUM_LINES];
	int i, occurredAttributes, lineSize;
//	int *mesg = NULL;
	FILE *inFile = fopen(dbName, "r");
	int totalSize = 0;
	if(inFile == NULL){
		printf("Could not open file %s\n", DB_NAME);	
		exit(1);
	}

	for(i = 0; i < NUM_LINES; i++){
		occurredAttributes = 0;
		fgets(line[i], NUM_ATTR+10, inFile);
		line[i][NUM_ATTR] = '\0';
#ifdef APP_DEBUG
		cout << line[i]<<endl;
#endif
		lineSize = strlen(line[i]);

		for(int j = 0; j < lineSize; j++){
			if(line[i][j] == '1'){
				occurredAttributes++;
			}
		}
		totalSize += occurredAttributes + 2;
		// 2 => 
		//      1 -> to the transaction number
		//      2 -> to the number of items 
		mesg[i] = (int *) malloc(sizeof(int) * (occurredAttributes + 2));
	
		mesg[i][0] = i;
		mesg[i][1] = occurredAttributes;
		occurredAttributes = 0;
#ifdef APP_DEBUG		
		cout << "READER"<<endl;
#endif
		for(int j = 0; j < lineSize; j++){
			if(line[i][j] == '1'){
				mesg[i][occurredAttributes+2] = j;
#ifdef APP_DEBUG				
				cout << mesg[i][occurredAttributes+2];
#endif				
				occurredAttributes++;
			}
		}
#ifdef APP_DEBUG		
		cout <<endl;
#endif		

	}
	printf("total = %d\n", totalSize);
	AHEvent *event = new AHEvent(mesg, sizeof(int) * (occurredAttributes + 2), outPut);
	dispachEvent(event);

}*/

void reader::sendDB(char *dbName, streamOutputHandler outPut){
	int numReaders = getNumReaders( outPut);
	printf("NumReaders = %d\n", numReaders);
	fflush(stdout);
	char **line = (char **)malloc(sizeof(char *)*NUM_LINES);
	for(int i =0; i < NUM_LINES; i++){
		line[i] = (char*)malloc(sizeof(char )*NUM_ATTR+10 );
	}
	int **mesg = (int **)malloc(sizeof(int*)*NUM_LINES);

	int i, occurredAttributes, lineSize;
//	printf("ei\n");
//	fflush(stdout);
	FILE *inFile = fopen(dbName, "r");
	int totalSize = 0;
	if(inFile == NULL){
		printf("Could not open file %s\n", DB_NAME);	
		exit(1);
	}

	for(i = 0; i < NUM_LINES; i++){
		occurredAttributes = 0;
		fgets(line[i], NUM_ATTR+10, inFile);
		line[i][NUM_ATTR] = '\0';
#ifdef APP_DEBUG
		cout << line[i]<<endl;
#endif
		lineSize = strlen(line[i]);

		for(int j = 0; j < lineSize; j++){
			if(line[i][j] == '1'){
				occurredAttributes++;
			}
		}
		totalSize += occurredAttributes + 2;
		// 2 => 
		//      1 -> to the transaction number
		//      2 -> to the number of items 
		mesg[i] = (int *) malloc(sizeof(int) * (occurredAttributes + 2));
	
		mesg[i][0] = i;
		mesg[i][1] = occurredAttributes;
		occurredAttributes = 0;
#ifdef APP_DEBUG		
		cout << "READER"<<endl;
#endif
		for(int j = 0; j < lineSize; j++){
			if(line[i][j] == '1'){
				mesg[i][occurredAttributes+2] = j;
#ifdef APP_DEBUG				
				cout << mesg[i][occurredAttributes+2];
#endif				
				occurredAttributes++;
			}
		}
#ifdef APP_DEBUG		
		cout <<endl;
#endif		

	}
	int ii = 0;

	for(int j = 0; j < numReaders; j++){
//		int msgSize = 0;
		int *finalMesg = (int *)malloc(sizeof(int)*(totalSize+1));
		finalMesg[0] = NUM_LINES/numReaders;
		printf("Num lines = %d\n", finalMesg[0]);
		fflush(stdout);
		int msgSize = 1;
		for(i = 0; i < NUM_LINES/numReaders; i++, ii++){
			occurredAttributes = 1;
			finalMesg[msgSize] = ii;
			finalMesg[msgSize+1] = mesg[ii][1];
			msgSize+=2;
			for(int j = 0; j < lineSize; j++){
				if(line[ii][j] == '1'){
					finalMesg[msgSize] = j;
//					finalIndex++;
					msgSize++;
				}
			}
			free(mesg[ii]);
			free(line[ii]);
//			msgSize += occurredAttributes+2;
	
		}
		AHEvent *event = new AHEvent(finalMesg, sizeof(int) * (msgSize), outPut);
		dispachEvent(event);

		printf("total = %d\n", totalSize);
	}
	free(line);
	free(mesg);
	fclose(inFile);
}

provide(reader)
