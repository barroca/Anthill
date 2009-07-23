#ifndef FILTROA_H
#define FILTROA_H

#include <iostream>

#include "eventAPI.h"

class filterA  : public AHFilter
{
public:
	int lama;
	filterA() : AHFilter(1){
		for( lama = 1; lama < 10; lama++){
			void *buf = malloc(sizeof(int));
			memcpy(buf, &lama, sizeof(int));
			cout << "filterA lama = " << lama <<endl;

			AHData *data = new AHData(buf, sizeof(int), 0);
			sendMsg(data);
		}
		
		cout << getArgument("teste") <<endl;
	}
	~filterA(){
		lama = 0;
		cout << "filterA lama = " << lama <<endl;
	}
};


#endif
