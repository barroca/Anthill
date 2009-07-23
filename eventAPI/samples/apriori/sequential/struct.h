#ifndef STRUCT
#define STRUCT
#include <bitset>


#include <unistd.h>
#include <sys/resource.h>
#include <iostream>
#include <bitset>
#include <set>
#include "constants.h"
#include "struct.h"

using std::cout;
using std::endl;
using std::bitset;
using std::set;

struct frequent_itemset_t{
   bitset<NUM_ATTR> itemset;
   long frequency;
   bool isFrequent;
}; 

#define MSG_M_A_SZ	sizeof(bitset<NUM_ATTR> itemset);


#endif

