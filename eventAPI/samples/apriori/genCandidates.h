#ifndef GEN_H
#define GEN_H

#include <iostream>
#include "util.h"
#include "struct.h"
#include "eventAPI.h"
#include "itemsets_to_calc.h"
#include <sys/time.h>

class genCandidates : public AHFilter
{

public:
	itemsets_to_calc calculator;
	streamOutputHandler outToMerger;
	streamInputHandler inputAdder;
	set< comparable_bitset > itemsets_being_processed;
	struct timeval btime, etime;

	genCandidates() :AHFilter(1){
		inputAdder = getInputHandler("inAdder");
		getEventList(inputAdder)->handler.connect( sigc::mem_fun(this, &genCandidates::createCandidates) );
		outToMerger = getOutputHandler("outMerger");
		sleep(250);
		gettimeofday( &btime,NULL );
		createsend_first_itemsets();
	}
	~genCandidates(){

		gettimeofday( &etime,NULL );
        	 cout.precision(5); // number of decimal numbers
	         cout << "#GENCANDIDATES" << endl;
	         cout << fixed << "GEN_CANDIDATES = " << ((etime.tv_sec-btime.tv_sec)*(double)1000000 + (double)(etime.tv_usec-btime.tv_usec))/(double)1000000 << endl;
	}

	void sendCandidate(int *items);

	void createsend_first_itemsets();
	void createCandidates(AHEvent *event);


};


#endif

