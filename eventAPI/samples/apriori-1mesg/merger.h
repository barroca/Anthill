#ifndef MERGER_H
#define MERGER_H

#include <iostream>
#include "eventAPI.h"
#include "invertedList.h"

class merger  : public AHFilter
{

public:
	streamOutputHandler outToAdder;
	invertedList inverList;
	pthread_mutex_t lau;

	constructFunctions( merger )

	merger():AHFilter(2){
		streamInputHandler inputReader = getInputHandler("inReader");


		//getEventList(inputReader)->handler.connect( sigc::mem_fun(this, &merger::functionCreateInvertedList) );
		streamInputHandler inputGen = getInputHandler("inGen");
		//getEventList(inputGen)->handler.connect( sigc::mem_fun(this, &merger::countItemSet) );
		//
		setHandler( inputReader, &merger::functionCreateInvertedList );
		setHandler( inputGen, &merger::countItemSet );

		outToAdder = getOutputHandler("outAdder");
		pthread_mutex_init(&lau, NULL);

	}
	~merger(){
		cout << " Merge Destroy " <<endl;
//		inverList.printAttribute(1);
		pthread_mutex_destroy(&lau);
	}
	int functionCreateInvertedList(AHEvent *event);
	int countItemSet(AHEvent *event);
};


#endif

