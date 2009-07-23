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
		assert(attribute >-1 && attribute < NUM_ATTR);
		assert(line >= 0);
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

int invertedList::mergeLists2(int numAttributes, int *attributes){
	int count = -1, i, j, k, smallerList = 0, smallerListAttribute = 0;
	int *indexes = (int *)malloc(sizeof(int)*numAttributes);

	if((!indexes)){
		if(indexes) free(indexes);
		printf("Error: Could not allocate memory\n");
		return 0;
	}
	// this loop have been created just to verify if our attributes
	// are in the right range
	for(i = 0; i < numAttributes; i++){
		indexes[i] = 0;
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
		int item = invList[attributes[smallerListAttribute]][i];
		itemCount = 0;
		// procura item em todas as listas invertidas
		for(j = 0; j < numAttributes; j++){
//			printf("smallerList --- = %d\n", smallerList);
			// nao procuro na menor lista, pois esse cara eh a referencia
			if(j == smallerListAttribute ) continue;
			//procura atributo na lista j	
//			for(k = indexes[j]; (unsigned int)k < invList[attributes[j]].size(); k++){
			for(; indexes[j] < invList[attributes[j]].size();){
				if(item > invList[attributes[j]][indexes[j]]){
					indexes[j]++;
					continue;
					//itemCount++;
				}
				if(item == invList[attributes[j]][indexes[j]]){
					itemCount++;
					indexes[j]++;
				//	break;
				}//else{
					
					
				//}
				break;
				
			}
		}
		if(itemCount ==  numAttributes -1)count++;
	}
	free(indexes);
	return count;
}

int invertedList::mergeLists(int numAttributes, int *attributes){
	int count = -1, i, j, k, smallerList = 0, smallerListAttribute = 0;

	// this loop have been created just to verify if our attributes
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
//			printf("smallerList --- = %d\n", smallerList);
			// nao procuro na menor lista, pois esse cara eh a referencia
			if(j == smallerListAttribute && smallerList > 1) continue;
			//procura atributo na lista j	
			for(k = 0; (unsigned int)k < invList[attributes[j]].size(); k++){
				if(item == invList[attributes[j]][k])	{
					itemCount++;
				}	
				
			}
		}
		if(itemCount ==  numAttributes -1)count++;
	}
//	if(smallerList == 1){
		
		
//	}
	return count;
}


int invertedList::mergeListSorted(int numAttributes, int *attributes){
	int count = -1, i, j, smallerList = 0, smallerListAttribute = 0;
	int *indexes = (int *)malloc(sizeof(int)*numAttributes);
	int *indexeSize = (int *)malloc(sizeof(int)*numAttributes);

//	printf("NumAttributes = %d\n", numAttributes);

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
#ifdef APP_DEBUG				
			//	printf("smallerList = %d attribute = %d \n", smallerList, attributes[i]);
#endif		

			}else{
				if(invList[attributes[i]].size() < (unsigned int)smallerList){
					smallerList = invList[attributes[i]].size();	
					smallerListAttribute = i;
				}	
			}	
		}
		indexeSize[i] = invList[attributes[i]].size();
//		printf("indexeSize[%d] = %d attribute = %d\n", i, invList[attributes[i]].size(), attributes[i]);
	}
	pthread_mutex_unlock(&insertTrans);

	int itemCount = 0;

	for(i = 0; i < smallerList; i++){
		int item = invList[attributes[smallerListAttribute]].at(i);
		itemCount = 0;
//		printf("Item = %d\n", item);
		// procura item em todas as listas invertidas
		for(j = 0; j < numAttributes; j++){
//			printf("Smaller ListSize = %d smallerAttributeIndex = %d numAttri = %d\n", smallerList, smallerListAttribute ,numAttributes);
			// nao procuro na menor lista, pois esse cara eh a referencia
			if(j == smallerListAttribute) continue;
			
			
			
			if(indexes[j] >= indexeSize[j]){
				if(itemCount ==  numAttributes -1)count++;
//				printf("bye\n");
				goto bye;
			}	

			//procura atributo na lista j	
			while(invList[attributes[j]].at(indexes[j]) < item){
				indexes[j]++;
				if(indexes[j] >= indexeSize[j])goto bye;
				
			}
			if(invList[attributes[j]].at(indexes[j]) == item){
				itemCount++;
				if(indexes[j] >= indexeSize[j]){
					if(itemCount ==  numAttributes -1)count++;
//					printf("bye\n");
					goto bye;
				}	
				indexes[j]++;
				if(indexes[j] >= indexeSize[j]){
//					printf("bye\n");
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
