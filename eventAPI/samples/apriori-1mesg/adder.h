#ifndef ADDER_H
#define ADDER_H

#include <iostream>

#include "eventAPI.h"
#include <unistd.h>
#include <sys/resource.h>
#include <iostream>
#include "FilterDev.h"
#include <bitset>
#include <set>
#include "itemsets_to_calc.h"
#include "constants.h"
#include "struct.h"

using std::cout;
using std::endl;
using std::bitset;
using std::set;




class adder : public AHFilter
{

public:
	map < string, itemSetData > mapCount;
	streamOutputHandler outToGen;
	streamInputHandler inputMerger;

	constructFunctions( adder )

	adder() :AHFilter(1){
		inputMerger = getInputHandler("inMerger");
		setHandler( inputMerger, &adder::accItemset );
	//	getEventList(inputMerger)->handler.connect( sigc::mem_fun(this, &adder::accItemset) );
		outToGen = getOutputHandler("outGen");
	}
	~adder(){
	}

	int accItemset(AHEvent *event);
};


#endif

