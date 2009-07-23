#ifndef INVERTED_LIST
#define INVERTED_LIST

#include <iostream>
#include "constants.h"
#include <pthread.h>
#include <vector>
#include <list>
#include <assert.h>
using namespace std;

class invertedList{
	private:
		vector<int> invList[NUM_ATTR];

	public:
		pthread_mutex_t insertTrans;

		invertedList(){
			pthread_mutex_init(&insertTrans, NULL);
		}
		~invertedList(){
			pthread_mutex_destroy(&insertTrans);
		}

		int returnItem(int attribute, int position);
		int insertItem(int attribute, int line);
		int mergeLists(int numAttibutes, int *attributes);
		int mergeListSorted(int numAttibutes, int *attributes);

		int insertItemSorted(int attribute, int line);

		void printAttribute(int attribute){
			cout << "Printing attribute "<< attribute <<endl;
			assert(attribute >=0 && attribute < NUM_ATTR);
			for(unsigned int i = 0; i < invList[attribute].size(); i++){
				cout << invList[attribute][i]<< " " ;	
			}
			cout <<endl;
		}

};
#endif
