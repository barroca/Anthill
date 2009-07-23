#include "genCandidates.h"


void genCandidates::createCandidates(AHEvent *event){
	int *data = (int *)event->getData();
	itemset_t recvItemSet = msgToItemSetT(data);
	recvItemSet.isFrequent = data[data[0]+1];
	itemsets_being_processed.erase( comparable_bitset( recvItemSet.itemset ) );
	cout << "GenCandidates: itemSize" <<endl;
	for(int i = 0; i < data[0]; i++){
		cout << data[i+1] <<" ";	
	}
	cout << "frequent = "<< data[data[0]+1] <<endl;

	itemset_t new_itemset;
	set< comparable_bitset > new_itemsets;
	if ( recvItemSet.isFrequent ) {
		new_itemsets = calculator.generate_new_itemsets( recvItemSet.itemset );  
		set< comparable_bitset >::iterator new_itemsets_it = new_itemsets.begin();        

		for(; new_itemsets_it!=new_itemsets.end(); new_itemsets_it++){
			itemsets_being_processed.insert( *new_itemsets_it );
			new_itemset.itemset = (*(new_itemsets_it)).internal_bitset;
			int itemSetSize = calculator.calculate_itemset_size(new_itemset.itemset);
			int *outMsg = (int *)malloc(sizeof(int)* (itemSetSize + 1));
			assert(outMsg != NULL);


			outMsg[0] = itemSetSize;
			int j = 0;
			for(int i = 0; i < NUM_ATTR; i++){
				if(new_itemset.itemset[i] == 1){
					outMsg[j+1] = i;			
					j++;
				}
			}
			AHEvent *candidateEvent = new AHEvent(outMsg, (itemSetSize+1) *sizeof(int), outToMerger);
			assert(candidateEvent != NULL);
			dispachEvent(candidateEvent);
			
			new_itemset.itemset.reset();
		}
		cout << "Itemset " << recvItemSet.itemset.to_string() << " is frequent. " << endl;
	}

	if (new_itemsets.empty() && itemsets_being_processed.empty()){
			closeEventList(inputAdder);
	}
	delete event;

}

void genCandidates::sendCandidate(int *items){
	int itemSize = items[0];
	bitset<NUM_ATTR> itemBitSet;
	itemBitSet.reset();
	
	for(int j =0; j < itemSize; j++){
		itemBitSet.set(items[j+1]);		
	}
	itemsets_being_processed.insert( comparable_bitset(itemBitSet) );

	AHEvent *event = new AHEvent(items, (itemSize +1) *sizeof(int) , outToMerger);
	assert(event!= NULL);

	dispachEvent(event);	
}
void genCandidates::createsend_first_itemsets(){
//	int itemSet[2];
//	itemSet[0] = 1;
	int *itemSet;	
	for (size_t i=0;i<NUM_ATTR;i++){
		itemSet = (int *)malloc(sizeof(int) * 2);
		itemSet[0] = 1;
		itemSet[1] = i;
		sendCandidate(itemSet);
		printf("Sending candidate - size = %d and item = %d\n", itemSet[0], itemSet[1]);
	}		
	
}
provide(genCandidates)
