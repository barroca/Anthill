#include "invertedList.h"
#include <pthread.h>


int invertedList::returnItem(int attribute, int position){
	int returnValue = -1;
	if(attribute < NUM_ATTR){
		if(position > -1 && (unsigned int)position <  invList[attribute].size())
			returnValue = invList[attribute][position];
	}
	return returnValue;
}
int invertedList::insertItem(int attribute, int line){
	int returnValue = -1;
	pthread_mutex_lock(&insertTrans);
	if(attribute < NUM_ATTR){
		invList[attribute].push_back(line);
		returnValue = 1;	
	}
	pthread_mutex_unlock(&insertTrans);

	return returnValue;
}

int invertedList::insertItemSorted(int attribute, int line){
	int returnValue = -1, i;
	try{
		pthread_mutex_lock(&insertTrans);
//		assert(attribute >-1 && attribute < NUM_ATTR);
		if(attribute < 0 || attribute >= NUM_ATTR|| line < 0){
			printf("assert");
			return 0;
		}	
//		assert(line >= 0);
		if(attribute < NUM_ATTR){
			int size = invList[attribute].size();
			invList[attribute].push_back(line);

			for(i = size-1; i > 0; i--){
				if(line < invList[attribute].at(i)){
					//				invList[attribute][i+1] = invList[attribute][i];	
					invList[attribute].at(i+1) = invList[attribute].at(i);	

				}else{

					break;
				}
			}

			invList[attribute].at(i+1) = line;
			//		invList[attribute].push_back(line);
			returnValue = 1;	
		}
		pthread_mutex_unlock(&insertTrans);
	}catch(exception e ){
		printf("Error:\n");
		fflush(stdout);
	}
	return returnValue;
}

int invertedList::mergeLists(int numAttributes, int *attributes){
	int count = -1, i, j, k, smallerList = 0, smallerListAttribute = 0;

	// this loop have been created just to verify if our attibutes
	// are in the right range
	for(i = 0; i < numAttributes; i++){
		if(attributes[i] >= NUM_ATTR){
			return -1;	
		}else{
			if(count == -1){
				smallerList = invList[attributes[i]].size();	
				smallerListAttribute = 0;
				count = 0;
			}else{
				if(invList[attributes[i]].size() < (unsigned int)smallerList){
					smallerList = invList[attributes[i]].size();	
					smallerListAttribute = i;
				}	
			}	
		}

	}

	int itemCount = 0;

	for(i = 0; i < smallerList; i++){
		int item = invList[smallerListAttribute][i];
		itemCount = 0;
		// procura item em todas as listas invertidas
		for(j = 0; j < numAttributes; j++){
			// nao procuro na menor lista, pois esse cara eh a referencia
			if(j == smallerListAttribute) continue;
			//procura atributo na lista j	
			for(k = 0; (unsigned int)k < invList[attributes[j]].size(); k++){
				if(item == invList[attributes[j]][k])	{
					itemCount++;
				}	
				
			}
		}
		if(itemCount ==  numAttributes -1)count++;
	}
	return count;
}


int invertedList::mergeListSorted(int numAttributes, int *attributes){
	int count = -1, i, j, smallerList = 0, smallerListAttribute = 0;
	int *indexes = (int *)malloc(sizeof(int)*numAttributes);
	int *indexeSize = (int *)malloc(sizeof(int)*numAttributes);

	if((!indexes)||(!indexeSize)){
		if(indexes) free(indexes);
		if(indexeSize)free(indexeSize);
		printf("Error: Could not allocate memory\n");
		return 0;
	}

	memset(indexes, 0, sizeof(int)*numAttributes);
	memset(indexeSize, 0, sizeof(int)*numAttributes);

	pthread_mutex_lock(&insertTrans);

	// this loop have been created just to verify if our attibutes
	// are in the right range
	for(i = 0; i < numAttributes; i++){
		if(attributes[i] >= NUM_ATTR){
			return -1;	
		}else{
			if(count == -1){
				smallerList = invList[attributes[i]].size();	
				smallerListAttribute = 0;
				count = 0;
			}else{
				if(invList[attributes[i]].size() < (unsigned int)smallerList){
					smallerList = invList[attributes[i]].size();	
					smallerListAttribute = i;
				}	
			}	
		}
		indexeSize[i] = invList[attributes[i]].size();
	}
	pthread_mutex_unlock(&insertTrans);

	int itemCount = 0;

	for(i = 0; i < smallerList; i++){
		int item = invList[attributes[smallerListAttribute]].at(i);
		itemCount = 0;
		// procura item em todas as listas invertidas
		for(j = 0; j < numAttributes; j++){
			// nao procuro na menor lista, pois esse cara eh a referencia
			if(j == smallerListAttribute) continue;
			//procura atributo na lista j	
			while(invList[attributes[j]].at(indexes[j]) < item){
				indexes[j]++;
				if(indexes[j] >= indexeSize[j])goto bye;
				
			}
			if(invList[attributes[j]].at(indexes[j]) == item){
				indexes[j]++;
				itemCount++;
				if(indexes[j] >= indexeSize[j]){
					if(itemCount ==  numAttributes -1)count++;
					goto bye;
				}	

			}else{
				break;	

			}	
				
		}
		if(itemCount ==  numAttributes -1)count++;
	}
bye:	
	free(indexeSize);
	free(indexes);
	return count;
}
