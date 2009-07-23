#include "invertedList.h"

int main(){
	
	int mergeResult;
	invertedList *list = new invertedList();
//	vector<int> attributes;
//	attributes.push_back(0);
//	attributes.push_back(1);
	int attributes[2] = {0, 1};

	list->printAttribute(0);
	list->printAttribute(1);
	mergeResult = list->mergeListSorted((int)2, (int *)attributes);
	cout << "MergeResult = "<< mergeResult <<endl;

	assert(mergeResult==0);

	list->insertItemSorted(0, 0);
	list->insertItemSorted(0, 5);

	list->insertItemSorted(0, 2);
	list->insertItemSorted(0, 10);

	list->insertItemSorted(1, 1);
	list->insertItemSorted(1, 2);
	list->insertItemSorted(1, 5);
	list->insertItemSorted(1, 9);
	
	list->printAttribute(0);
	list->printAttribute(1);

	mergeResult = list->mergeListSorted(2,(int *)attributes);

	cout << "MergeResult = "<< mergeResult <<endl;
	assert(mergeResult == 2);

}
