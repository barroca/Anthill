/* This object contains all the data a Filter needs to run
 * Added by Matheus: September, 13, 2004
 */

#ifndef __FILTERDATA_H
#define __FILTERDATA_H

#include "FilterDev.h"
#include <Task/Task.h>
#include "Ports.h"
#include "constants.h"
#ifdef VOID_INST
#include "Instrumentation.h"
#endif
#ifdef VOID_TRACER
#include <Tracer/tracer.h>
#include "tracingdefs.h"
#endif

/// the information about the library of the filter
typedef struct _LibData_ {
	char name[MAX_LNAME_LENGTH+1]; ///< the library name, usually lib"filtername".so
	void *libHandler; ///<  "handler"  for  the dynamic  library
	initialize_t *init; ///< init pointer 
	process_t	 *process; ///< process pointer
	finalize_t *finalize; ///< finalize pointer
} LibData;


///the data that filter needs to run
typedef struct __FilterData__{
	int id; ///< the filter id
 	char name[MAX_FNAME_LENGTH +1]; ///<the filter name
	char hostName[MAX_HNAME_LENGTH + 1]; ///< name of filter host
	int memory; ///< the memory this machine has available for this execution, from XML
	int numLocalInstances; ///< the number of brother + me in this machine
	int numInstances; ///< number of brothers + me
	int parentTid; ///< the tid of my parent
	int argc;
	char **argv;
#ifdef ATTACH	
	int attached; ///< 1: if this is a filter attached 0: if no 
#endif	
#ifdef BMI_FT
	int faultStatus; ///< NO_FAULT, FAULT_OTHER_FILTER_INST, or FAULT_THIS_FILTER_INST
	int lastFilter; ///< 1 if this is the last filter, 0, otherwise
#endif
	int tids[MAXINSTANCES]; ///< array of tids, mine and brothers 
	int myRank; ///< which brother am I?
	int numInputPorts, numOutputPorts; ///<my number of ports
	int numInportsAdded, numOutportsAdded; ///<number of ports loaded till now
	InputPort *inputPorts[MAXINPSTREAMS]; ///<my input ports
	OutputPort *outputPorts[MAXOUTSTREAMS]; ///<my output ports
	LibData libdata; ///<the library data
	//TaskData taskData;
#ifdef VOID_INST
	InstData *instData; ///< instrumentation data
#endif
#ifdef VOID_TRACER
	TrcData *trcData; ///< tracing data
#endif

} FilterData;


#ifdef FILTERDATA_C
FilterData *fd; ///< Global variable that points to data of filter
#else
extern FilterData *fd; ///< Global variable that points to data of filter
#endif


///constructor
FilterData *createFilterData(); 
///destroyer
void destroyFilterData(FilterData *fd); 
/** when the filter runs this, hell receive all data he needs to run the pipeline
 * @return 1 if all goes well, -1 if an error is found
 */
int recvFilterData();
///runs the filter
void runFilter(int argc, char **argv);

//these functions does not use arguments after call
/// Sets the name of filter
int setFDArgc(FilterData *fd, int argc);
/// Sets the name of filter
int setFDArgv(FilterData *fd, char **argv);
/// Sets the name of filter
int setFDName(FilterData *fd, char *name);
/// Sets the name of the machine where the filter is runnig
int setFDHostName(FilterData *fd, char *hostName); 
/// Sets the number of inputs of the filter 
int setFDNumInputs(FilterData *fd, int numInp);
/// Sets the number of output
int setFDNumOutputs(FilterData *fd, int numOut);
/// Sets the libname of the filter: Used to load the filter
int setFDLibName(FilterData *fd, char *libName);
///sets the machine memory declared inside the XML file
int setFDMachineMem(FilterData *fd, int mem);
///sets the number of instances this filter has in current machine
int setFDLocalInstances(FilterData *fd, int numLocalInst);
/// Sets the number of instances of this type of filter
int setFDNumInstances(FilterData *fd, int numInstances); 
/// Sets the rank of this instance in his set of filters
void setFDRank(FilterData *fd, int rank);
/// Sets the id of this filter - unique for all filters 
void setFDIdFilter(FilterData *fd, int id);
/// sets the tids of this filter, mine and brothers
void setFDTids(FilterData *fd, int *tids);
#ifdef VOID_INST
/// sets the instrumentation directory
void setFDInstDir(FilterData *fd, char *dir);
#endif
#ifdef BMI_FT
/// sets the fault's status
void setFDFaultStatus(FilterData *fd, int faultStatus);
/// sets if the filter is the last one in the pipeline
void setFDLastFilter(FilterData *fd, int lastFilter);
#endif


/// Gets the ammount of memory the machine this filter runs on has
int getFDMachineMem(FilterData *fd);
///get the number of brothers I have in this same machine
int getFDLocalInstances(FilterData *fd);
/// Gets the name of filter
char *getFDName(FilterData *fd);
/// Gets the number of instances of this type of filter
int getFDNumInstances(FilterData *fd); 

int getFDArgc(FilterData *fd);
char **getFDArgv(FilterData *fd);


//these functions use argument after call, ie, dont free them
/// adds another inputPort to filteData: we only point to InputPort received -- dont free it
int addFDInputPort(FilterData *fd, InputPort *p);
/// adds another OutputPort to filteData: we only point to OutputPort received -- dont free it
int addFDOutputPort(FilterData *fd, OutputPort *p);

///this function loads the filter libraries(init process finalize)
int loadFDLibFunctions(FilterData *fd); 

#ifdef BMI_FT
int getFDFaultStatus(FilterData *fd);
int getFDLastFilter(FilterData *fd);
#endif


// Prototype of initCppAPI().
// Coutinho: I decided to don't include a C++ API header because
// 	this will make Anthill depend of the C++ API.
typedef int (initCppAPI_t)(char* cppFilterLib);

#endif
