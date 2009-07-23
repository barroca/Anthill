#include "itemsets_to_calc.h"


bool comparable_bitset::operator<(const comparable_bitset& a) const
{
   for (size_t i=0; i<NUM_ATTR; ++i){
      if (internal_bitset[i] < a.internal_bitset[i]) { return true; }
      else if (internal_bitset[i] > a.internal_bitset[i]) { return false; }
   }
   return false;
};

bool comparable_bitset::operator==(const comparable_bitset& a) const
{
   return internal_bitset == a.internal_bitset;
};

int itemsets_to_calc::add_frequent_itemset( bitset<NUM_ATTR> &frequent_itemset ) {

   int size = calculate_itemset_size( frequent_itemset );   
   set< comparable_bitset >* itemset_set;
   
   map< int, set< comparable_bitset >* >::iterator frequent_itemsets_it = frequent_itemsets.find( size );
   if (frequent_itemsets_it == frequent_itemsets.end()) { // SE A LISTA NAO EXISTIR, CRIAMOS UMA
      itemset_set = new set< comparable_bitset >();
      frequent_itemsets[size] = itemset_set;
   } else {
      itemset_set = frequent_itemsets_it->second;
   }

   // ADICIONA ITEM NA LISTA
   itemset_set->insert( frequent_itemset);
   return size;
}


set< comparable_bitset > itemsets_to_calc::generate_new_itemsets( bitset<NUM_ATTR> & frequent_itemset  ){
   int size = add_frequent_itemset( frequent_itemset );
   set< comparable_bitset > returned;
   
   set< comparable_bitset >* frequent_itemset_set = frequent_itemsets.find( size )->second;
   set< comparable_bitset >::iterator frequent_itemset_set_it = frequent_itemset_set->begin();
   
   bitset<NUM_ATTR> to_add, aux;
   for ( ; frequent_itemset_set_it != frequent_itemset_set->end(); frequent_itemset_set_it++ ){ // VARRENDO LISTA
      aux =  (*frequent_itemset_set_it).internal_bitset ;
      if ( frequent_itemset != aux) // SE FOR DIFERENTE DELE MESMO
      {
         to_add = frequent_itemset;
         to_add |= aux;
         returned.insert( comparable_bitset(to_add) );
      }
   
   }

   return returned;
}

int itemsets_to_calc::calculate_itemset_size( bitset<NUM_ATTR>& itemset )
{
   int returned = 0;
   for (size_t i=0; i<itemset.size(); ++i){
       returned += (int) itemset.test(i);
   }
   return returned;
}


#include <string>
using std::string;
int main()
{
   bitset<NUM_ATTR> itemset_1(string("01000"));
   bitset<NUM_ATTR> itemset_2(string("00100"));
   bitset<NUM_ATTR> itemset_3(string("01110"));
   bitset<NUM_ATTR> itemset_4(string("01111"));
   bitset<NUM_ATTR> itemset_5(string("11110"));
   itemsets_to_calc calculator;
   calculator.generate_new_itemsets( itemset_1 );
   calculator.generate_new_itemsets( itemset_2 );
   calculator.generate_new_itemsets( itemset_4 );
   calculator.generate_new_itemsets( itemset_4 );
   calculator.generate_new_itemsets( itemset_5 );
}

