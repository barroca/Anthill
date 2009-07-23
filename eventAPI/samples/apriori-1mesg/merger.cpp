#include "merger.h"
#include <vector>

int merger::functionCreateInvertedList(AHEvent *event){

	if(event == NULL){
		printf("NULL event\n");		
		pthread_mutex_unlock(&lau);	

		return 0;
	}

	printf("Merger: create inverted list\n");

	int *mesg = (int *) event->getData();
	int numLines = mesg[0];
	int mesgIndex = 1;
	for(int j = 0; j < numLines; j++){
		int transactionId = mesg[mesgIndex];	
		int occurredAttributes = mesg[mesgIndex+1];
		mesgIndex+=2;
#ifdef APP_DEBUG	
		cout << "transId = "<< transactionId << "occured " << occurredAttributes <<endl;
#endif	
		for(int i = 0; i < occurredAttributes; i++){
#ifdef APP_DEBUG		
			cout << mesg[mesgIndex] << " ";
#endif		
			inverList.insertItem( mesg[mesgIndex], transactionId);
			//		inverList.insertItem( mesg[i+2], transactionId);
			mesgIndex++;
		}
#ifdef APP_DEBUG	
		cout <<endl;
#endif	
	}
	delete event;
}

int merger::countItemSet(AHEvent *event){
	int itemSetSize, outDataSize, mergeResult;


	assert(event !=NULL);
	// receive  a message with the items to merge
	int *eventData = (int *)event->getData();
	assert(eventData !=NULL);

	// number of items
	itemSetSize = eventData[0];
	if(itemSetSize < 0 || itemSetSize > NUM_ATTR){

		printf("ERRO\n");	
		fflush(stdout);
		exit(1);
		return 0;
	}


	// merge the lists
	mergeResult = inverList.mergeListSorted(itemSetSize, &(eventData[1]));
//	mergeResult = inverList.mergeLists2(itemSetSize, &(eventData[1]));

#ifdef APP_DEBUG	
	cout << "Merger" <<endl;
	for(int i = 0; i < eventData[0]; i++){
		cout << eventData[i+1] <<" ";	
	}
	cout << " count = "<< mergeResult<<endl;
#endif
	// create the output.. is the sames as received + the mergeResult
	outDataSize = itemSetSize + 2;
	int *countItemData = (int *)malloc(sizeof(int) * (outDataSize));
	assert(countItemData != NULL);

	countItemData[0] = itemSetSize;
	memcpy(&(countItemData[1]), &(eventData[1]), (itemSetSize * sizeof(int)));
	countItemData[outDataSize -1] = mergeResult;
//	pthread_mutex_lock(&lau);	
	// Create another event to be dispached to the Adder filter
	AHEvent *countItemEvent = new AHEvent(countItemData, outDataSize*sizeof(int), outToAdder);
	assert(countItemEvent != NULL);
//	pthread_mutex_lock(&lau);	

	dispachEvent(countItemEvent);
	delete event;

}

provide(merger)
