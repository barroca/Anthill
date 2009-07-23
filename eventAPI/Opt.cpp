#include "eventAPI.h"
#include "FilterDev/FilterDev.h"
#include <dlfcn.h>

AHFilter *filter = NULL;
filterFactory_t* filterLibFactory = NULL; 

/////////////////////// AHEventTypeList ////////////////////

void AHEventTypeList::insert(eventType event){
	pthread_mutex_lock(&listLock);
	eventTypeList.push_back(event);
	pthread_mutex_unlock(&listLock);
}

eventType* AHEventTypeList::getFirst(){
	eventType *event = NULL;
	pthread_mutex_lock(&listLock);
	if(!eventTypeList.empty()){
		*event = eventTypeList.front();
		eventTypeList.pop_front();
	}else{
//		int sval;
//		sem_getvalue(, &sval);

		printf("eventList = empty| semaphore \n");	
		
	}
	pthread_mutex_unlock(&listLock);
	return event;
}

unsigned int AHEventTypeList::size(){
	return eventTypeList.size();	
}
bool AHEventTypeList::iseEmpty(){
	return eventTypeList.empty();
}

////////////////////////// AHEventList /////////////////////////

void AHEventList::setStream(string stream){
	this->stream = stream;
}

string AHEventList::getStream(){
	return stream;
}
unsigned int AHEventList::size(){
	pthread_mutex_lock(&eventListLock);
	unsigned int returnValue = eventList.size();
	pthread_mutex_unlock(&eventListLock);
	return returnValue;
}

void AHEventList::insert(AHEvent *event){
	pthread_mutex_lock(&eventListLock);
	eventList.push_back(event);
	pthread_mutex_unlock(&eventListLock);
}

//TODO NOTE THAT THIS METHOD IS NOT 
//SYNCHRONIZED ANYMORE. IF OTHER
//THREAD GET THE PROCESS SLICE IN THE
//INSTANT BEFORE THE RETURN, SHIT 
//COULD OCCURS
AHEvent *AHEventList::getFirst(){
	AHEvent *event = NULL;
	pthread_mutex_lock(&eventListLock);
	if (!eventList.empty()){
		event = eventList.front();
		eventList.pop_front();
	}	
	pthread_mutex_unlock(&eventListLock);
	return event;
}

//////////////////////// AHDispachList ///////////////////////
int AHDispachList::sendNextEvent(){
   pthread_mutex_lock(&getFirstMutex);
	AHEvent *event = getFirst();
   pthread_mutex_unlock(&getFirstMutex);

	// it means that we have an event to be sent
	if(event !=NULL){
		int serializedSize;
		void *eventSerialized = event->serialize(&serializedSize);
		streamHandlerType streamHandler = event->getStreamHandler();

		int writeReturn = dsWriteBuffer((OutputPortHandler) streamHandler, eventSerialized, serializedSize);	

		if(writeReturn < 0) return -1;
//		cout << "eventPointer = "<< (void *)event << endl;

//		event->freeData();
		delete event;
		return 0;	
	}
	return 1;	
}

int AHDispachList::sendAllEvents(){
	int returnValue;
	do{
		returnValue = sendNextEvent();
	}while(returnValue == 0);

	return returnValue;
}
////////////////////////// Arguments /////////////////////

void Arguments::setArguments(int argc, char **argv){
	assert(argc%2==1);

	for(int i = 1; i < argc; i+=2){
		if(argv[i][0] != '-'){
			cout << "The argument name = " << argv[i] << ", should start with '-'." << endl;	
			exit(1);
		}
		if(strlen(argv[i]) == 1){
			cout << "Argument has no name"<<endl;
			exit(1);
		}

		args.insert(map<string, string>::value_type(&argv[i][1], argv[i+1]));
		
	}
}


// Return an empty string case this argument does not exist
string Arguments::getArgument(string argumentName){
	return args[argumentName];	
}



///////////////////////// AHFilter  ////////////////////////


////// Some private functions ///////

int AHFilter::AHGetNumInputPorts(){
	return dsGetNumInputPorts();		
}	

char **AHFilter::AHGetArguments(int *size){
	*size = dsGetArgc();
	return dsGetArgv();
}
void AHFilter::setMaxNumThreads(int numThreads){
	maxNumThreads = numThreads;	
}
int AHFilter::getMaxNumThreads(){
	return maxNumThreads;	
}
void AHFilter::addNumReceivedEOW(){
	numReceivedEOW ++;	
}
int AHFilter::getNumReceivedEOW(){
	return numReceivedEOW;	
}

void AHFilter::setUsedMemory(unsigned int size){
	eventsUsedMemory = size;
}
unsigned int AHFilter::getUsedMemory(){
	return eventsUsedMemory;	
}

////////////// PUBLIC Functions
int AHFilter::getNumWriters(streamInputHandler input){
	return 	(InputPortHandler) dsGetNumWriters( input);
}
int AHFilter::getNumReaders(OutputPortHandler output){
	return dsGetNumReaders( output);
}


void AHFilter::setEventMaxMemory(unsigned int eventsMemory){
	eventsMaxMemory = eventsMemory;	
}
void AHFilter::setMaxThreads(int maxNumThreads){
	this->maxNumThreads = maxNumThreads;
}
int AHFilter::getMaxThreads(){
	return maxNumThreads;	
}


string AHFilter::getArgument(string argumentName){
	return filterArguments.getArgument(argumentName);
}	
AHEventList *AHFilter::getEventList(streamInputHandler input){
	return &eventLists[input];
}

streamInputHandler AHFilter::getInputHandler(string stream){
	return (streamInputHandler) dsGetInputPortByName((char *)stream.c_str());
	
}
streamOutputHandler AHFilter::getOutputHandler(string stream){
	return (OutputPortHandler) dsGetOutputPortByName((char *)stream.c_str());
}



// return the inputports in a string vector
vector<string> AHFilter::AHGetInputPortNames(){
	int i, numInputs = 0;
	numInputs = AHGetNumInputPorts();

	char **inputPorts = dsGetInputPortNames();
   
   // creating the intern array with the correct size
	vector<string> aux(numInputs); 

	for(i = 0; i < numInputs; i++){
		aux.push_back(inputPorts[i]);	
	}
	return aux; 
}



// function called before add events to a list, because
// we have to verify the available memory
bool AHFilter::tryAllocateEventMemory(unsigned int size){
	bool returnValue = true;

	pthread_mutex_lock(&usedMem);
	unsigned used = getUsedMemory();
	// it means that we have 
	if(( used - size) >= 0){
		setUsedMemory((used - size));	

	}else{
		returnValue = false;	
	}		
	pthread_mutex_unlock(&usedMem);
	return returnValue;
}

// called to free the memory used with the dispached or removed event
void AHFilter::freeEventMemory(unsigned int size){
	pthread_mutex_lock(&usedMem);
	unsigned usedMemory = getUsedMemory() - size;
	assert(usedMemory >= 0 );
	setUsedMemory(usedMemory);
	pthread_mutex_unlock(&usedMem);		
} 

// function tha return a Event to the workers threads threat
AHEvent* AHFilter::returnAvailableEvent(){
	int i;
//TODO::/// Colocar um mutex nessa funcao.. isso vai dar pau	
	AHEvent *event = NULL;
	for(i = 0; i < AHGetNumInputPorts(); i++){
		event = eventLists[i].getFirst();
		if(event != NULL)break;
	}
	if(event == NULL){
		printf("ei\n");	
	}
	return event;
}




//Public function
// function responsable for the all logic involved with the events handler task
void AHFilter::processEvents(int tid){
	// Used to control the number of threads that still alive
	sem_wait(&aliveThreads);
	// it is used during the beginning, because the main thread can not
	// finish before the workers start
	pthread_mutex_unlock(&threadsCreated);
        

	while(1){
		sem_wait(&eventsToBeProcessed);
//		cout <<"Available Event"<<endl;

      pthread_mutex_lock(&avaliableEvent);
		processingEvents[tid] = returnAvailableEvent();
		
      pthread_mutex_unlock(&avaliableEvent);
//		cout<< "tid ="<<tid<< endl;
  //  cout <<" Event = "<< (void*)event <<endl;
		int sval;
//		sem_getvalue(&eventsToBeProcessed, &sval);
//		printf("events = %d\n", sval);
		fflush(stdout);

		if(processingEvents[tid] == NULL){
			sem_getvalue(&eventsToBeProcessed, &sval);

			printf("Saindo: event == NULL. eventsToBe = %d\n", sval);	
			break;
		}
		this->run(processingEvents[tid]);
//cout <<"I got a Event"<<endl;
//		eventLists[processingEvents[tid]->getStreamHandler()].handler(processingEvents[tid]);
	}
	sem_post(&aliveThreads);
}


void *callThread(void *arg){
	
	int *argInt = (int*)arg;
	cout << "Thread "  << *argInt << " before process Events"<<endl;
	filter->processEvents(*argInt);
	pthread_exit(NULL);
}

int AHFilter::closeEventList(streamInputHandler listIndex){
	addNumReceivedEOW();
	eventLists[listIndex].setEOW(true);
	return 0;
}

void AHFilter::controlEvents(){
	int i, createReturn;
		
	pthread_mutex_lock(&threadsCreated);
	// Creating worker threads
	for (int i =0; i < getMaxNumThreads(); i++){
		void *arg = (void*) malloc(sizeof(int));
		memcpy(arg, &i, sizeof(int));
		createReturn = pthread_create(&(workerThreads[i]), NULL, callThread, arg);
		if (createReturn){
			printf("ERROR: Return code from pthread_create() is %d\n", createReturn);
			exit(-1);
		}
	}
	int j;
	// while we do not receive EOW from all channel we
	// have to still listening them and creating events
	while(getNumReceivedEOW() < AHGetNumInputPorts()){
		int dsReadReturn;
		int numReceived = 0;
		int isMsg = 0;
		int probeRet = 0;
		//	for(j = 0; j < 10; j++){
		// verify if we have messages in one of the channels
		for(i = 0; i < AHGetNumInputPorts(); i++) {
			if(eventLists[i].getEOW() == true)continue;
			void *buf = NULL;
			char *label = NULL;
			probeRet = dsProbe((InputPortHandler) i);
			if(probeRet == 0){
				continue;	
			}
			isMsg = 1;
				
			dsReadReturn = 	dsReadBufferMalloc((InputPortHandler) i, &buf, &label);	

//			dsReadReturn = dsReadNonBlockingBufferMalloc((InputPortHandler) i, &buf, &label);	

			switch ( dsReadReturn ){
				case 0:	
					continue;
					break;
				case EOW:
					addNumReceivedEOW();
					eventLists[i].setEOW(true);
					printf("Receive EOW = %d and numInputPorts = %d\n", getNumReceivedEOW(), AHGetNumInputPorts());
					fflush(stdout);

					break;

				default:
					numReceived++;
					isMsg = 1;
					AHEvent *event;
					assert(buf != NULL);
					if(label != NULL){
						string labelString = label;
						event =  new AHEvent (buf, dsReadReturn, (streamHandlerType) i, &labelString);
					}else{
						//						int *mesg = (int*) buf;
						//						cout << "Creating a received event. Mesg = " << mesg[0] <<endl;

						event =  new AHEvent (buf, dsReadReturn, (streamHandlerType) i);
					}	
					assert(event != NULL);

					eventLists[i].insert(event);
					sem_post(&eventsToBeProcessed);
					break;
			}			
		}
		dispachedEvents.sendAllEvents();
		if(isMsg){
			isMsg = 0;	
			n_noReceive = 0;
		}else{
			if(n_noReceive < 10){
				n_noReceive +=1
			}	
			usleep(100*n_noReceive);
		}

	}

	pthread_mutex_lock(&threadsCreated);
	int semValue;

	// At this point no other event will be created in this filter.
	// I will give the threads the permission to get another event,
	// but this event is NULL, meanning that there no more events 
	// to be processed. Bellow I just wait them die
	for(int i = 0; i < getMaxNumThreads(); i++){
		sem_post(&eventsToBeProcessed);
	}

	do{ 
		// Wait all threads finishing and send the created events
		dispachedEvents.sendAllEvents();
		sem_getvalue(&aliveThreads, &semValue);

	}while(semValue != getMaxNumThreads());

	// Dar um Join para ter certeza

	// Now I will send the last created events
	dispachedEvents.sendAllEvents();

}

bool AHFilter::dispachEvent(AHEvent *event){
	dispachedEvents.insert(event);
	return true;	
}


//TODO
//
/*
void *AHFilter::getParameters(){
	return NULL;	
}


int AHFilter::nextEvent(string stream, string label){
	return 0;	
}*/




//////////////////// END AHFilter Functions //////////////////////////









//END:TODO
extern "C" int initEventAPI(char* cppFilterLib); ///< loaded by anthill to init the Event API
//extern "C" int initFilterEvent(void *work, int workSize);
extern "C" int processFilterEvent(void *work, int workSize);
extern "C" int finalizeFilterEvent();

int initEventAPI(char* eventFilterLib) {
	char *error = NULL;

	// try load lib in local directory, so we need 2 strigns,
	// the first with ./, em the second whithout
	char* libnameLocal = (char*)malloc(strlen(eventFilterLib) + 3);
	sprintf(libnameLocal, "./%s", eventFilterLib);

	// get the library handler
	void* filterLibHandler = NULL;
	if (((filterLibHandler = dlopen(libnameLocal, RTLD_NOW)) == NULL) &&
		((filterLibHandler = dlopen(eventFilterLib, RTLD_NOW)) == NULL )) {
			fprintf(stderr, "eventAPI.cpp: could not load filter %s library, %s\n", eventFilterLib, dlerror());
			dlclose(eventFilterLib);
			return -1;
	}
	free(libnameLocal);

	// load filter from dynamic library : filterFactory()
	// warnnig: here we only try to load simbols in C
	filterLibFactory = (filterFactory_t *)dlsym(filterLibHandler, "filterFactory");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "Erro no dlsym: %s\n", error);
		dlclose(filterLibHandler);
		return -1;
	}

	// call filterFactory() to register the filter object
	filterLibFactory();

	return 1;
}

int processFilterEvent(void *work, int workSize){
	filter->controlEvents();	
	return 0;
}

int finalizeFilterEvent(){
	// call filterFactory() to register the filter object
	filterLibFactory();

	return 1;
}

void unRegisterFilterObj(){
	delete filter;	
}

void registerFilterObj(AHFilter* f) {
	filter = f;
}

int main(){
	AHFilter *a= new AHFilter();
	AHEvent *event = new AHEvent();
	a->dispachEvent(event);
}
