#ifndef UTIL_H
#define UTIL_H

#include "struct.h"

itemset_t msgToItemSetT(int *data){
	itemset_t itemTask;
	itemTask.itemset.reset();		

	int itemSetSize = data[0];
	for(int j =0; j < itemSetSize; j++){
		itemTask.itemset.set(data[j+1]);		
	}
	itemTask.frequency = data[itemSetSize+1];

	return itemTask;	
}

#endif
