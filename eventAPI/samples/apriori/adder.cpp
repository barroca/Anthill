#include "adder.h"
#include "util.h"

void adder::accItemset(AHEvent *event){
	int *dataInput = (int *)event->getData();
	itemset_t itemSet = msgToItemSetT(dataInput);
	itemSet.frequency = dataInput[dataInput[0]+1];
#ifdef APP_DEBUG	
	cout << "Adder" <<endl;
	for(int i = 0; i < dataInput[0]; i++){
		cout << dataInput[i+1] <<" ";	
	}
	cout << " count = "<< dataInput[dataInput[0]+1]<<endl;
#endif

	// search for a itemset 
	map < string, itemSetData >::iterator mapCountIt = mapCount.find(itemSet.itemset.to_string());
	map < string, itemSetData >::iterator mapEndIt = mapCount.end();
	
	int numRecv = 0;
	int frequency = 0;

	// is the itemset in the map?
	if(mapCountIt != mapEndIt){
		mapCountIt->second.numReceived++;
		mapCountIt->second.frequency+=itemSet.frequency;
		frequency = mapCountIt->second.frequency;
		numRecv = mapCountIt->second.numReceived;
		/// update done just to print
		itemSet.frequency = mapCountIt->second.frequency;
	}else{
		// insert a new itemset in the map
		itemSetData aux;
		aux.numReceived = 1;
		aux.frequency = itemSet.frequency ;
		mapCount[itemSet.itemset.to_string()] =aux;
		numRecv = 1;
		frequency =  itemSet.frequency;
	}

	if(numRecv == getNumWriters(inputMerger)){
		int isFrequent = 0;
		if(frequency >= SUPORTE){
			isFrequent = 1;
		}
		cout << "send itemset " << itemSet.itemset <<" Frequency "<< itemSet.frequency<<endl;

		mapCountIt = mapCount.find(itemSet.itemset.to_string());
		mapEndIt = mapCount.end();

		assert(mapCountIt != mapEndIt);
		mapCount.erase(mapCountIt);

		int outMsgSize = event->getDataSize();
		int *outMsg = (int*)malloc(outMsgSize);
		memcpy(outMsg, dataInput, (1+dataInput[0])*sizeof(int));
		outMsg[1+dataInput[0]] = isFrequent;
	
		AHEvent *itemFreqEvent = new AHEvent(outMsg, outMsgSize, outToGen);
	
		dispachEvent(itemFreqEvent);
		delete event;
	}
}

provide(adder)
