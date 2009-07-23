#ifndef FILTRO_READER_H
#define FILTRO_READER_H

#include <iostream>
#include <cstdlib> 
#include <stdexcept>
#include <string>
#include <fstream>
#include <sstream>
#include <sys/time.h>
#include <sys/unistd.h>
#include "eventAPI.h"

using namespace std;

class reader  : public AHFilter
{
   public:
      reader() : AHFilter(1), num_db_registers(0){

         string train_file_path = getArgument("train");
         string test_file_path = getArgument("test");
         string k_str = getArgument("k");
         string d_str = getArgument("d");

         if ( train_file_path.size() == 0 || test_file_path.size() == 0 || k_str.size() == 0 || d_str.size() == 0 ){
            cout << "[ERROR] Invalid arguments. Usage ./initScript -train <train file path> -test <test file path> -k <k> -d <#dimensions>" <<endl;
            return;
         }

         gettimeofday( &btime,NULL );

         cout << "Reader initiated. Arguments Train: " << train_file_path << " Test: " << test_file_path << " k: " << k_str << " d: " << d << endl;
         try{

            trainning_to_class = getOutputHandler( "trainning_to_class" );
            classifier_to_class = getOutputHandler( "classifier_to_class" );

            k =  atoi(k_str.c_str());
            d = atoi(d_str.c_str());
            
            gettimeofday( &train_btime,NULL );
            process_train_file(train_file_path);
            gettimeofday( &train_etime,NULL );
	    cout << "Train sent" << endl;
            cout.flush();
	    sleep(500);
//            sleep(500);
	    cout << "WAIT = 500"<< endl;
            gettimeofday( &test_btime,NULL );
            process_test_file(test_file_path);
            gettimeofday( &test_etime,NULL );

         } catch (exception &rte){
            cout <<  "[ERROR] " << rte.what() << endl;
            return;
         }
         
         cout << "Reader processing finished sucessfully" << endl;
      }

      ~reader(){
         gettimeofday( &etime,NULL );
         cout << "DB_REGISTERS=" << num_db_registers<< endl;
         cout.precision(5); // number of decimal numbers
         cout << fixed << "READER_EXEC_TIME=" << ((etime.tv_sec-btime.tv_sec)*(double)1000000 + (double)(etime.tv_usec-btime.tv_usec))/(double)1000000 << endl;
         cout << fixed << "READER_TRAIN_TIME=" << ((train_etime.tv_sec-train_btime.tv_sec)*(double)1000000 + (double)(train_etime.tv_usec-train_btime.tv_usec))/(double)1000000 << endl;
         cout << fixed << "READER_TEST_TIME=" << ((test_etime.tv_sec-test_btime.tv_sec)*(double)1000000 + (double)(test_etime.tv_usec-test_btime.tv_usec))/(double)1000000 << endl;
      }

   private:

      streamOutputHandler trainning_to_class, classifier_to_class;
      unsigned int k,d;
      
      void get_dimensions(stringstream& ss, double* buff);
      void process_train_line( string& line, void* buff ) throw (runtime_error);
      void process_train_file(string& train_file_path ) throw (runtime_error);
      void process_test_file(string& test_file_path);

      // Metrics
      unsigned int num_db_registers;
      struct timeval btime, etime, test_btime, test_etime, train_btime, train_etime;

};

#endif
