#ifndef __CONST_H
#define __CONST_H

#define HASH_SIZE      1000
// used by hash
#define INITIAL_CAPACITY	100


#define MAXFILTERS			20 ///< maximum number of filters allowed
#define MAXINPSTREAMS		MAXFILTERS
#define MAXOUTSTREAMS		MAXFILTERS
#define MAXSTREAMS			((MAXINPSTREAMS+MAXOUTSTREAMS)/2)*MAXFILTERS
#define MAXRESOURCES		5 ///< maximum number of resources a host can have
#define MAXHOSTS			40 ///< maximum number of hosts the system can have
#define MAXINSTANCES		40 ///< maximum number of instances a filter can have


#define MINSIZE				100 //< buffer minimum size for pack and unpacks

#define PROD_CONS_SIZE 1000 ///< 
#define ID_LIST_INIT_SIZE 10

/* String lengths ******************************/
// all lengths DO NOT include the terminating \0
// so the variable should be declared as varname[MAX...+1]
#define MAX_CWD_LENGTH		500 ///< max cwd string length
#define MAX_XNAME_LENGTH	100 ///< max executable name
#define MAX_COM_LENGTH		(MAX_CWD_LENGTH+MAX_XNAME_LENGTH+1) ///< max command length
#define MAX_FNAME_LENGTH		50 ///< max filtername length
#define MAX_LBL_LENGTH		1000 ///< max label length
#define MAX_LNAME_LENGTH		(MAX_FNAME_LENGTH+6) ///< max libname length, +6 for the lib and .so
#define MAX_HNAME_LENGTH		100 ///< max hostname length
#define MAX_PTNAME_LENGTH	50 ///< max port name length
#define MAX_PLNAME_LENGTH	50 ///< max policy name length
#define MAX_SNAME_LENGTH		50 ///< max stream name length
#define MAX_RNAME_LENGTH	50 ///< max resource name length
#define MAX_IDIR_LENGTH		500 ///< max instrumentation directory length
/***********************************************/

/// MATLAB CONSTANTS
#ifdef MATLAB

#define MAX_ARGTYPE_NAME_LENGTH	50
#define MAX_FUNCNAME_LENGTH	120
#define MAX_INPUTYPE_NAME_LENGTH	10
#define MAX_MATLAB_OUTPUTS	10
#define MAX_MATLAB_INPUTS	10
#define MAX_MATLAB_INPUTOUTPUS	10

#endif


// Error Codes:
#define E_TASK_EXISTS -2 	   ///< task already exists
#define E_TASK_NOT_RUNNING	-3 ///< task already finished or still created
#define E_NO_SUCH_TASK -4		///< Task dosn't exist
#define E_COULD_NOT_RECOVER_TASK -5 ///< error opening/reading task serialization file



#endif
