#ifndef MERGER_H
#define MERGER_H

#include <iostream>
#include <cstring>
#include <sstream>
#include <map>
#include <list>
#include <fstream>
#include <sys/time.h>
#include "eventAPI.h"
#include "structs.h"
#include "constants.h"

using namespace std;

class merger : public AHFilter
{
   public:
	constructFunctions( merger )
      merger():AHFilter(NUM_MERGER_THREADS), merge_time(0), n_proc(0){

         string k_str = getArgument("k");
         string d_str = getArgument("d");
         
         if ( k_str.size() == 0 || d_str.size() == 0 ){
            cout << "[ERROR] Invalid arguments. Usage ./initScript -train <train file path> -test <test file path> -k <k> -d <#dimensions>" <<endl;
            return;
         }
         gettimeofday( &btime,NULL );
         cout << "Merger initiated. Arguments k: " << k_str << "\td: " << d_str << endl;
         try{

            streamInputHandler input = getInputHandler("classifier_from_classifier");
	    setHandler( input, &merger::merge );

          //  getEventList(input)->handler.connect( sigc::mem_fun(this, &merger::merge) );

            k =  atoi(k_str.c_str());
            d = atoi(d_str.c_str());
            
         } catch (exception &rte){
            cout <<  "[ERROR] " << rte.what() << endl;
            return;
         }
         cout << "Merger processing finished sucessfully" << endl;
      }

      ~merger(){
         gettimeofday( &etime,NULL );
         cout.precision(5); // number of decimal numbers
         cout << fixed << "MERGER_EXEC_TIME=" << ((etime.tv_sec-btime.tv_sec)*(double)1000000 + (double)(etime.tv_usec-btime.tv_usec))/(double)1000000 << endl;
         cout << fixed << "MERGER_MERGE_TIME=" << merge_time << endl;

         //print output
         ofstream out("output");
         if (out.is_open()){
            map< string, list<cpair> >::iterator it = points_map.begin();
            for(;it!=points_map.end();it++){
               out << "Point: " << it->first << " --> ";
               list<cpair>::iterator it1 = it->second.begin();
               for(;it1!=it->second.end();it1++){
                  out << "[class:" << (*it1).class_id << "|distance:" << (*it1).distance << "]";
               }
               out << endl;
            }
            out.close();
         }
      }


      int merge(AHEvent *event);
   
   private:
      unsigned int k,d;
      map< string, list<cpair> > points_map;
      struct timeval btime, etime, merge_btime, merge_etime, aux;
      double merge_time;
      int n_proc;
};


#endif
