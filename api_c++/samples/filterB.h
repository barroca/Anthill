#ifndef FILTROA_H
#define FILTROA_H

#include <iostream.h>

#include "api_cpp.h"

class filterB  : public DCFilter
{

public:
	filterB(){}
	~filterB(){}

	int init();
	int process();
	int fini();
};


#endif
