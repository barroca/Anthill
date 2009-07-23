#ifndef __ITEMSETS_TO_CALC 
#define __ITEMSETS_TO_CALC 

#include <map>
#include <bitset>
#include <set>
#include <iostream>
#include "constants.h"

using std::map;
using std::bitset;
using std::set;
using std::cout;
using std::endl;


class comparable_bitset{
   
   public:
      comparable_bitset( bitset<NUM_ATTR> &ib ){
      	int i = 0;
      	for(; i < NUM_ATTR; i++){
		internal_bitset.set(i, ib[i]);
	}
      } 
      ~comparable_bitset(){}
      bool operator<(const comparable_bitset& a) const;
      bool operator==(const comparable_bitset& a) const;
      bitset<NUM_ATTR> internal_bitset;
   
};

class itemsets_to_calc {

   public:
      itemsets_to_calc() {}
      ~itemsets_to_calc() {}

      set< comparable_bitset > generate_new_itemsets( bitset<NUM_ATTR>&  );

      int add_frequent_itemset( bitset<NUM_ATTR>& );
      map< int, set< comparable_bitset >* > frequent_itemsets;
      int calculate_itemset_size( bitset<NUM_ATTR>& );

};

#endif

