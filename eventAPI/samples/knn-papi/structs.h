#ifndef STRUCTS_
#define STRUCTS_

#include <cmath>
#include "eventAPI.h"

struct cpair{
   unsigned int class_id;
   double distance;
};

bool compare_pair( cpair& a, cpair& b ){
   return a.distance < b.distance;
};

bool from_the_same_class( cpair& a, cpair& b){
   return a.class_id == b.class_id;
}

double calculate_distance( double* a, double* b, unsigned int d){
   double sum =0;
   for (unsigned int i=0; i<d;i++){
      sum+=pow((a[i]-b[i]),2);
   }
   return sqrt( sum ); 
};

double* get_dimensions( double* buff, unsigned int d){
   const unsigned int dimensions_size = d*sizeof(double);
   double* dimensions = (double*)malloc(dimensions_size);
   memcpy(dimensions, buff, dimensions_size );
   return dimensions;
};
#endif
