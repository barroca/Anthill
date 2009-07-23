#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;

int main(int argc, char** argv){

   if (argc<=4){
      cerr << argv[0] << " <#registros> <#dimensoes> <#classes> <arquivo>" << endl;
      return -1;
   }

   ofstream out(argv[4]);
   if (out.is_open()){
      unsigned int n_reg = atoi(argv[1]);
      unsigned int n_dim = atoi(argv[2]);
      unsigned int n_classes = atoi(argv[3]);
      double class_choose;
      srand( (unsigned int)time( NULL ) );

      // class top generation
      double* class_top = (double*) malloc(n_classes *sizeof(double));
      for (unsigned int i=n_classes; i>=1; i--){
         class_top[n_classes-i] = (double)1/(double)i; // realize the inverse order
      }

      // Each line generation
      for (unsigned int i=0; i < n_reg ; i++){
         ostringstream oss;
         oss.precision(9); // assim parece melhor
         
         // Class choose
         class_choose = drand48();
         for (unsigned int i=0; i<n_classes;i++){
            if (class_choose<=class_top[i]){
               oss << i << " ";
               break;
            }
         }

         for (unsigned int j=0; j < n_dim-1; j++){
            oss << fixed << drand48() << " ";
         }
         oss << fixed << drand48(); // the last one without space
         
         // writing the line into file
         out << oss.str() << endl;
      }
      out.close();
   } else {
      cerr << "Error opening the output file:  " << argv[4] << endl;
      return -1;
   }
   return 0;
}
