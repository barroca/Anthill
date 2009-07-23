#ifndef FILTROB_H
#define FILTROB_H

#include <iostream>

#include "eventAPI.h"

class filterB  : public AHFilter
{

public:
	int lama;
	constructFunctions( filterB );

	filterB() :AHFilter(2){
		lama = 1;
		streamInputHandler input = getInputHandler("in");
		setHandler( input, &filterB::functionEvent );

	}
	~filterB(){
		lama = 0;
	}


	int functionEvent(AHData *data);

};


#endif

//		this->eventLists[0].handler.connect( sigc::mem_fun(this, &filterB::functionEvent) ); 

