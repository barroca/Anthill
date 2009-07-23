#ifndef READER_H
#define READER_H

#include <iostream>
#include "constants.h"
#include "eventAPI.h"

class reader: public AHFilter
{
public:
	constructFunctions( reader )

	int lama;
	reader() : AHFilter(1){

		cout << "Filter: reader -> creator" <<endl;
		streamOutputHandler outPutMerger = getOutputHandler("outMerger");
		sendDB( DB_NAME, outPutMerger);
		
	}
	~reader(){
		lama = 0;
		cout << "Filter: reader -> destructor"<<endl;
	}

	void sendDB(char *dbName, streamOutputHandler outPut);
		
};


#endif
