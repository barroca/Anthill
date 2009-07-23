#include <iostream>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sstream>
using namespace std;

int main(int argc, char** argv){

   if (argc<=3){
      cerr << argv[0] << " <#registros> <#dimensoes> <arquivo>" << endl;
      return -1;
   }

   ofstream out(argv[3]);
   if (out.is_open()){
      unsigned int n_reg = atoi(argv[1]);
      unsigned int n_dim = atoi(argv[2]);
      srand( (unsigned int)time( NULL ) );


      // Each line generation
      for (unsigned int i=0; i < n_reg ; i++){
         ostringstream oss;
         oss.precision(9); // assim parece melhor
         for (unsigned int j=0; j < n_dim-1; j++){
            oss << fixed << drand48() << " ";
         }
         oss << fixed << drand48() << " ";
         
         // writing the line into file
         out << oss.str() << endl;
      }
      out.close();
   } else {
      cerr << "Error opening the output file:  " << argv[3] << endl;
      return -1;
   }
   return 0;
}

