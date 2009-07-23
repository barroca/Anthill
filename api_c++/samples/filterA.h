#ifndef FILTROA_H
#define FILTROA_H

#include <iostream.h>

#include "api_cpp.h"

class filterA  : public DCFilter
{

public:
	filterA(){}
	~filterA(){}

	int init();
	int process();
	int fini();
};


#endif
