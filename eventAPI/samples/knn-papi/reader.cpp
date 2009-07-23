#include "reader.h"


void reader::get_dimensions(stringstream& ss, double* buff){
   string element;
   for (unsigned int i=0;i<d;i++){
      ss >> element;
      buff[i]= atof( element.c_str() );
   }
}


void reader::process_train_line( string& line,  void* buff ) throw (runtime_error) {
#ifdef DEBUG
   cout << "Process train line " << line << endl;
#endif

   stringstream ss(line);
   string element;
   ss >> element;

   int class_id = atoi(element.c_str());
   memcpy( buff, &class_id, sizeof(int) )  ;
   
   get_dimensions(ss,(double*) &((int*)buff)[1]);
}

void reader::process_train_file(string& train_file_path) throw (runtime_error){
   cout << "Process train file path: " << train_file_path << "\tdexample: " << d << endl;
   ifstream train_file (train_file_path.c_str());
   if (train_file.is_open())
   {
      string line;
      void* buff;
      const unsigned int buff_size = (sizeof(double)*d) + sizeof(unsigned int);
      getline (train_file,line);
      while (!train_file.eof() )  {
         buff = malloc( buff_size );
         process_train_line( line, buff );
         AHEvent *event = new AHEvent(buff, buff_size, trainning_to_class);
         dispachEvent(event);
         num_db_registers++;
//	cout << "Train reg sent "  << num_db_registers<<endl;
//	cout.flush();  
         getline (train_file,line);
      }
      train_file.close();
#ifdef DEBUG
   cout << "Train file closed." << endl;
#endif
   } else {
      throw runtime_error( "Unable to open train file");
   }
}

void reader::process_test_file(string& test_file_path){
   cout << "Process test file path: " << test_file_path << "\td: " << d << endl;
   ifstream test_file (test_file_path.c_str());
   if (test_file.is_open())  {
      string line;
      void* buff;
      const unsigned int buff_size = (sizeof(int) + sizeof(double)*d);
      getline (test_file,line);
      while (!test_file.eof() )  {
         buff = malloc( buff_size );
         stringstream ss(line);
         get_dimensions(ss, (double*)buff );
         AHEvent *event = new AHEvent(buff, buff_size, classifier_to_class);
         dispachEvent(event);
         getline (test_file,line);
      }
      test_file.close();
   } else {
      throw runtime_error( "Unable to open test file");
   }   
}

provide(reader) /* Macro that registers the filter don't touch here*/
