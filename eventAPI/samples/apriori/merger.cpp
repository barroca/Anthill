#include "merger.h"
#include <vector>

void merger::functionCreateInvertedList(AHEvent *event){
	pthread_mutex_lock(&lau);	

	if(event == NULL){
		printf("NULL event\n");		
		pthread_mutex_unlock(&lau);	

		return;
	}


	int *mesg = (int *) event->getData();
	int transactionId = mesg[0];	
	int occurredAttributes = mesg[1];
#ifdef APP_DEBUG	
	cout << "transId = "<< transactionId << "occured" << occurredAttributes <<endl;
#endif	
	for(int i = 0; i < occurredAttributes; i++){
#ifdef APP_DEBUG		
		cout << mesg[i+2] << " ";
#endif		
		inverList.insertItemSorted( mesg[i+2], transactionId);
//		inverList.insertItem( mesg[i+2], transactionId);

	}
#ifdef APP_DEBUG	
	cout <<endl;
#endif	
	delete event;
	pthread_mutex_unlock(&lau);	
}

void merger::countItemSet(AHEvent *event){
	int itemSetSize, outDataSize, mergeResult;

	assert(event !=NULL);
	// receive  a message with the items to merge
	int *eventData = (int *)event->getData();
	assert(eventData !=NULL);

	// number of items
	itemSetSize = eventData[0];
	assert(itemSetSize >0 && itemSetSize <= NUM_ATTR);
	// merge the lists
	mergeResult = inverList.mergeListSorted(itemSetSize, &(eventData[1]));

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
