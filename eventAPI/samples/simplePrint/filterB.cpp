#include "filterB.h"


int filterB::functionEvent(AHData *data){
	int *aux = (int *)data->getData();
	cout << "I am instance "<< getMyRank() << " The total is " << getTotalInstances() <<". My eventData is " << aux[0] <<endl;
	
	delete data;
	return 0;
}

provide(filterB)
