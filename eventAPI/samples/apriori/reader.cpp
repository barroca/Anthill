#include "reader.h"

void reader::sendDB(char *dbName, streamOutputHandler outPut){
	char line[NUM_ATTR+10];
	int i, occurredAttributes, lineSize;
	int *mesg = NULL;
	FILE *inFile = fopen(dbName, "r");

	if(inFile == NULL){
		printf("Could not open file %s\n", DB_NAME);	
		exit(1);
	}

	for(i = 0; i < NUM_LINES; i++){
		occurredAttributes = 0;
		fgets(line, NUM_ATTR+10, inFile);
		line[NUM_ATTR] = '\0';
#ifdef APP_DEBUG
		cout << line<<endl;
#endif
		lineSize = strlen(line);

		for(int j = 0; j < lineSize; j++){
			if(line[j] == '1'){
				occurredAttributes++;
			}
		}

		// 2 => 
		//      1 -> to the transaction number
		//      2 -> to the number of items 
		mesg = (int *) malloc(sizeof(int) * (occurredAttributes + 2));
	
		mesg[0] = i;
		mesg[1] = occurredAttributes;
		occurredAttributes = 0;
#ifdef APP_DEBUG		
		cout << "READER"<<endl;
#endif
		for(int j = 0; j < lineSize; j++){
			if(line[j] == '1'){
				mesg[occurredAttributes+2] = j;
#ifdef APP_DEBUG				
				cout << mesg[occurredAttributes+2];
#endif				
				occurredAttributes++;
			}
		}
#ifdef APP_DEBUG		
		cout <<endl;
#endif		

		AHEvent *event = new AHEvent(mesg, sizeof(int) * (occurredAttributes + 2), outPut);
		dispachEvent(event);
	}
}


provide(reader)
