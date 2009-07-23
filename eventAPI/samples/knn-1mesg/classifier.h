#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include <iostream>
#include <cstring>
#include <map>
#include <list>
#include <climits>
#include <pthread.h> 
#include <sys/time.h>
#include "eventAPI.h"
#include "structs.h"
#include "constants.h"
//#include "papi_counters.h"

using namespace std;

class classifier : public AHFilter
{
   public:
	constructFunctions( classifier );

	int a;
	int numCounters;
	int numInserted;

	classifier():AHFilter(NUM_CLASSIFIER_THREADS), number_off_train_reg(0){

	numInserted = 0;
	a = 0;
//	numCounters = PAPI_SIMPLE_INIT();

         string k_str = getArgument("k");
         string d_str = getArgument("d");
         
         if ( k_str.size() == 0 || d_str.size() == 0 ){
            cout << "[ERROR] Invalid arguments. Usage ./initScript -train <train file path> -test <test file path> -k <k> -d <#dimensions>" <<endl;
            return;
         }
         pthread_mutex_init(&mutex, NULL);
         pthread_mutex_init(&test_mutex, NULL);
	 threadsInFunction = 0;
         gettimeofday( &btime,NULL );

         cout << "Classifier initiated. Arguments k: " << k_str << "\td: " << d_str << endl;
         try{

            streamInputHandler input = getInputHandler("trainning_from_reader");
	    setHandler( input, &classifier::trainning );
 //           getEventList(input)->handler.connect( sigc::mem_fun(this, &classifier::trainning) );

            streamInputHandler class_input = getInputHandler("classifier_from_reader");
	    setHandler( class_input, &classifier::classify );

 //           getEventList(class_input)->handler.connect( sigc::mem_fun(this, &classifier::classify) );

            classifier_to_merger = getOutputHandler("classifier_to_merger");

            k =  atoi(k_str.c_str());
            d = atoi(d_str.c_str());
            buff_size = buff_size = (sizeof(double)*d) + sizeof(unsigned int);
            
         } catch (exception &rte){
            cout <<  "[ERROR] " << rte.what() << endl;
            return;
         }
         
         cout << "Classifier processing finished sucessfully" << endl;
      }

      ~classifier(){

         pthread_mutex_lock(&mutex);
         gettimeofday( &etime,NULL );
         pthread_mutex_unlock(&mutex);
         pthread_mutex_destroy(&mutex);
         pthread_mutex_destroy(&test_mutex);
         
         cout.precision(5); // number of decimal numbers
         cout << "CLASSIFIER_TRAIN_REG=" << number_off_train_reg << endl;
         cout << fixed << "CLASSIFIER_EXEC_TIME=" << ((etime.tv_sec-btime.tv_sec)*(double)1000000 + (double)(etime.tv_usec-btime.tv_usec))/(double)1000000 << endl;
//	 PAPI_SIMPLE_PRINT(numCounters, "CLASSIFIER_CACHE");
      }


      int trainning(AHEvent *event);
      int classify(AHEvent *event);
   
   private:
      streamOutputHandler classifier_to_merger;
      void print_train_map();
      unsigned int k,d, buff_size;
      map< int, list<double*> > train_map;

      // Metrics
      unsigned int number_off_train_reg;
      struct timeval btime, etime,timestamp;
      pthread_mutex_t mutex, test_mutex;
      int streamHandler;
      int threadsInFunction;
};


#endif
