
#ifndef EVENT_API_H
#define EVENT_API_H

#include "eventExceptions.h"

#include <list>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define KB_1          1024
#define KB_2          2048
#define KB_4          4096
#define KB_8          8192
#define KB_16        16384
#define KB_32        32768
#define KB_64        65536
#define KB_128      131072
#define KB_256      262144
#define KB_512      524288
#define MB_1       1048576
#define MB_2       2097152
#define MB_4       4194304
#define MB_8       8388608
#define MB_16     16777216 
#define MB_32     33554432
#define MB_64     67108864
#define MB_128   134217728
#define MB_256   268435456
#define MB_512   536870912
#define GB_1    1073741824



using namespace std;

typedef int streamInputHandler, streamOutputHandler, streamHandlerType;

// Warnning: Take care when u are using this class, because we do not make 
// copies or free the data in all its functions. We do it to have better 
// performance. Case you need it you can implement these funcions
class AHData
{
	private:
		void *data;
		unsigned int dataSize;
		string label;
		streamHandlerType streamHandler;

	public:
		AHData(){
			dataSize = 0;	
			data = NULL;
			streamHandler = -1;
		}
		AHData(void *data, int dataSize, streamHandlerType streamHandler, string *label = NULL){
			// Warnning: I am not copying the data to have better perfomance
			// so take care when you are using this funcion
			this->data = data;	
			this->dataSize = dataSize;
			if(streamHandler != -1){
				this->streamHandler = streamHandler;
				if(label != NULL){
					this->label = *label;
				}
			}else{
				if(label != NULL){
					printf("Warnning: You have seted the label, but not the streamHandler.\n");	
					this->label = *label;
				}

			}
		}
		~AHData(){
			freeData();
		}

		void *getData(){
			return data;
		}
		void setData(void *data){
			this->data = data;
		}

		int getDataSize(){
			return dataSize;	
		}

		void setDataSize(int dataSize){
			this->dataSize = dataSize;
		}
		string getLabel(){
			return label;	
		}
		void setLabel(string label){
			this->label = label;
		}
		streamHandlerType getStreamHandler(){
			return streamHandler;	
		}
		void setStreamHandler(streamHandlerType streamHandler){
			this->streamHandler = streamHandler;
		}

		unsigned int size(){
			return (unsigned int)(dataSize + label.size() + sizeof(streamHandlerType));
		}
		void *serialize(int *size){
			*size = dataSize;			
			return data;
		}

		// this function is called by the framework 
		// after send the event to free the used memory
		void freeData(){
			if (data != NULL)free(data);
		}
};


class eventType{
	private:
		string label;
		string stream;
	
	public:

		eventType(){};
		eventType(string *streamInput, string *labelInput=NULL){
			if(streamInput == NULL){
				cout << "Warning: Creating a a eventType with stream=NULL. It will be totaly empty\n";	
			}else{
				stream = *streamInput;		
				if(labelInput != NULL){
					label = *labelInput;	
				}
			}			
			
		}
		~eventType(){};

		void setLabel(string label){
			this->label = label;	
		}
		string getLabel(){
			return label;
		}
		void setStream(string stream){
			this->stream = stream;	
		}
		string getStream(){
			return stream;
		}
};

class AHEventTypeList
{
	private:
		list<eventType> eventTypeList;
		pthread_mutex_t listLock;
	
	public:
		AHEventTypeList(){
			pthread_mutex_init(&listLock, NULL);	
		};

		~AHEventTypeList(){
			pthread_mutex_destroy(&listLock);	
		};

		void insert(eventType event);

		eventType *getFirst();
		
		unsigned int size();

		bool iseEmpty();
		// TODO: temos q fazer um iterador para andarmos na fila quando tivermos
		// restricao de dependencia
};

class AHEventList
{
	private:
		list<AHData*> eventList;
		pthread_mutex_t eventListLock;
		string stream;
		bool eow;
	
	public:
		// It is the function that knows how to handle this type
		// of events

		AHEventList(){
			pthread_mutex_init(&eventListLock, NULL);	
			setEOW(false);
		};

		~AHEventList(){
			pthread_mutex_destroy(&eventListLock);	
		};

		bool getEOE(){
			return (eow && eventList.empty()) ;	
		}

		void setEOW(bool eow){
			this->eow = eow;	
		}
		bool getEOW(){
			return eow;	
		}
		void setStream(string stream);

		string getStream();

		unsigned int size();
		
		void insert(AHData *event);

		AHData *getFirst();
		// TODO: temos q fazer um iterador para andarmos na fila quando tivermos
		// restricao de dependencia
};

class AHDispachList: public AHEventList{
	private:
		pthread_mutex_t getFirstMutex;   

	public:
		AHDispachList(){
			pthread_mutex_init(&getFirstMutex, NULL);
		};
		~AHDispachList(){
			pthread_mutex_destroy(&getFirstMutex);
		};

		// send the next event available in the event list
		// return 0 case it is successful
		// return 1 case there is no event to be sent
		// return -1 in case of error
		int sendNextEvent(AHData *event);
		int sendAllEvents();
};

class Arguments
{
	private:
		map<string, string> args;

	public:

		void setArguments(int argc, char **argv);

		string getArgument(string argumentName);
	
};

class AHFilter 
{
	private:
		// Variables used to control the space of memory used by events
		unsigned int eventsUsedMemory;
		unsigned int eventsMaxMemory;
		void setUsedMemory(unsigned int size);
		unsigned int getUsedMemory();
		bool isInConstructor;

		unsigned int lastProcessedStream;
		// synchronizantion objects
		pthread_mutex_t usedMem;
		pthread_mutex_t threadsCreated;
		pthread_mutex_t avaliableEvent;
		pthread_t *workerThreads;
		sem_t aliveThreads;

		void setMaxNumThreads(int numThreads);
		int getMaxNumThreads(); 

		// used by the framework to create the worker threads
		int maxNumThreads;
		int actualNumThreads;


		Arguments filterArguments;

		// The events list of events created, dispached and event type to handle 
		vector<bool> eoE;
		sem_t eventsToBeProcessed;
		int numReceivedEOW;


		// translation of the Anthill API. We create it just to not include
		// the FilterDev.h and do not export the Anthill API to the user
		char **AHGetArguments(int *size);
		vector<string> AHGetInputPortNames();
		AHDispachList dispachedEvents;

		void addNumReceivedEOW();
		int getNumReceivedEOW();

		// function called before add events to a list, because
		// we have to verify the available memory
		bool tryAllocateEventMemory(unsigned int size);

		// called to free the memory used with the dispached or removed event
		void freeEventMemory(unsigned int size);

		// function tha return a Event to the workers threads threat
		AHData* returnAvailableEvent();
		/// TOSCAO
		vector<AHEventList> eventLists;

		AHData **processingEvents;


	public:
		int AHGetNumInputPorts();

		int init;

		AHFilter(int maxNumThreads = 1)
			: dispachedEvents()
		{
			isInConstructor = true;
			cout << "AHFilter Construtor"<<endl;
			// Used to know if we have or now alloced space to the  EventHandlerFunctions
			init = 0;
			// Max of memory used by the events lists
			pthread_mutex_init(&usedMem, NULL);
			pthread_mutex_init(&avaliableEvent, NULL);

			// the the available memory to the events to 16MB
			setEventMaxMemory(MB_16);

			// In the beginning we are using zero bytes
			setUsedMemory(0);

			// create list of events for each stream
			int numInputPorts = AHGetNumInputPorts();

			// get the inputPorts and initialize each existing stream
			vector<string> inputPorts = AHGetInputPortNames();
			for (int i = 0; i < numInputPorts; i++){
				AHEventList *aux = new AHEventList();
				aux->setStream(inputPorts[i]);
				eventLists.push_back(*aux);
			}
			cout << "EventListVectorSize = "<< eventLists.size() <<endl;
			numReceivedEOW = 0;
			setMaxNumThreads(maxNumThreads);

			processingEvents = (AHData**)malloc(sizeof(AHData*)*maxNumThreads);
			// Allocate the threads data structure
			workerThreads = (pthread_t *) malloc(sizeof(pthread_t) * getMaxNumThreads());
			int ret = sem_init(&aliveThreads, 0, getMaxThreads());

			// We start if no one event in our lists
			ret = sem_init(&eventsToBeProcessed, 0, 0);

			// Max of memory used by the events lists
			pthread_mutex_init(&threadsCreated, NULL);

			// Setting the filter arguments
			int argsSize;
			char **arguments = AHGetArguments(&argsSize);
			filterArguments.setArguments(argsSize, arguments);

			// the first stream that we're going try to process in the begining is the 0
			lastProcessedStream = -1;
		};

		virtual ~AHFilter(){
			cout << "AHFilter Destrutor"<<endl;
			pthread_mutex_destroy(&usedMem);
			pthread_mutex_destroy(&avaliableEvent);
			pthread_mutex_destroy(&threadsCreated);
			free(processingEvents);
			sem_close(&aliveThreads);
		};

		void setMaxThreads(int maxNumThreads);
		int getMaxThreads();

		// This function inserts the event in the outPut Queue and
		// when it is sent to the next filter we free the event
		bool sendMsg(AHData *data);

		void setEventMaxMemory(unsigned int eventsMemory);
		string getArgument(string argumentName);

		AHEventList *getEventList(streamInputHandler input);

		/**Return the stream handler associated with a given stream
		 * input port name.
		 *
		 * @throws anthill::HandlerNotFoundError if a handler was not
		 * 	found.
		 */
		streamInputHandler getInputHandler(string stream);

		/**Return the stream handler associated with a given stream
		 * output port name.
		 *
		 * @throws anthill::HandlerNotFoundError if a handler was not
		 * 	found.
		 */
		streamOutputHandler getOutputHandler(string stream);

		int getNumWriters(streamInputHandler input);
		int getNumReaders(streamOutputHandler output);

		int getMyRank();
		int getTotalInstances();

		// functions implemented by Anthill
		// Responsible by receive messages and create events
		void controlEvents();

		// process events get the event from the events list and
		// call the handler responsible to process it
		void processEvents(int tid);

		// Used to "force" a EOW in a channel. When, for example, you have a 
		// loop application and one filter have to start the termination
		int closeEventList(streamInputHandler listIndex);	

		// it is "abstractly" implemented inside the filters
		virtual void run(AHData *data){};

};



///////////// The code below do not need to be modified to create filters 
///////////// it is used to create and destroy filter objects in C
// OBS: Do you love your life??
// Yes: Do not modify the code below
// No : Go ahead, read and modify it. Good Lucky!!!
//////////////////////////////////////////////////////////////////////////
extern AHFilter *filter;

// functions to help loading the C++ filter
void registerFilterObj(AHFilter* filter);
void unRegisterFilterObj();
typedef void (filterFactory_t)(); 
extern "C" void filterFactory();

#define provide( c1 )  void filterFactory() { if(filter==NULL){ registerFilterObj(new c1());}else{unRegisterFilterObj();} }
//#define constructFunctions( c1 )  int (c1::**funcPtr)(); void setHandler(int pos, int (c1::*func)() ){ if(init == 0){ funcPtr = (int (c1::**)())malloc(sizeof(int (c1::*)())*2); init = 1;} funcPtr[pos] = func;}; void run(int handler){ printf("Run\n"); (this->*(funcPtr[handler]))(); }
#define constructFunctions( c1 )  int (c1::**funcPtr)(AHData *); void setHandler(int pos, int (c1::*func)(AHData *) ){ if(init == 0){ funcPtr = (int (c1::**)(AHData *))malloc(sizeof(int (c1::*)(AHData *))*AHGetNumInputPorts()); init = 1;} funcPtr[pos] = func;}; void run(AHData *event){ (this->*(funcPtr[event->getStreamHandler()]))(event); }

////////////////////////NOT IMPLEMENTED YET ////////////////////////
//bool dependencyFunctionRegister(dependencyFunction_t, string stream);
//int nextEvent(string stream = NULL, string label = NULL);

#endif
