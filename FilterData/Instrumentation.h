#ifndef _INSTRUMENTATION_H_
#define _INSTRUMENTATION_H_

#include <sys/time.h>
#include <stack/stack.h>
#include "../constants.h"

/** The program can be in either of these states
  * proc means processing
  * read means entered a receive function
  * write means entered a write function
  * init means is initializing(initFilter)
  * finalizing mean is in finalize(finalizeFilter)
  * void means time void used for its own
  * end is used to end timing results
  * process is the sum of proc, read and write
  */
typedef enum { 
	TIMER_INIT=0, TIMER_PROC, TIMER_READ, 
	TIMER_WRITE, TIMER_WRITE_BLOCKED, TIMER_LS, 
	TIMER_FINALIZE, TIMER_VOID, TIMER_END 
} TimerState;
/** WARNING: if you change the number of states above, change here also! */
#define TIMER_NUM_VOID_STATES 9

#ifdef _INSTRUMENTATION_C_
char *timerStateNames[TIMER_NUM_VOID_STATES]={
	"timer_init", "timer_proc", "timer_read", "timer_write", "timer_w_bkd", 
	"timer_ls",	"timer_finalize", "timer_void", "timer_end_not_used"};
#endif

#define INST_DIR "instrumentation"

/** this is a state */
typedef struct _InstState_ {
	struct timeval timing; 	//< the timing for this state
	char *stateName;		//< a name to the state, used to print it
	int isUserState; 		//< indicates this is a user state
} InstState;

/** this holds the instrumentation data */
typedef struct _InstrumentationData {
	char instDir[MAX_IDIR_LENGTH+1]; //< the instrumentation directory, we write results to this
	
	InstState voidStates[TIMER_NUM_VOID_STATES]; 	//< void states
	InstState *userStates;	//< user states
	int numUserStates; 		//< number of user states

	/** current timer */	
	struct timeval tv;		//< time val used to calculate the above timings
	struct timezone tz; 	//< not used

	InstState *currentState; //< the state we are
	Stack *stackedStates;	//< stack of states, so we can have recursive calls	
} InstData;

#ifdef BMI_FT
// After the failure, the attatched filters don't call dlopen. Then
// the states deffined by the users must be initiated using this backup
typedef struct {
	InstState *userStates;
	int numUserStates;
} BackupUserSt;

BackupUserSt *backupUserSt;
#endif
void backupUserStates(InstData *inst);
void restoreUserStates (InstData *inst);
void destroyUserStateBackup();

/** Starts the instrumentation data, zeroing everything
  * @return the instrumentation data created
  */
InstData *instCreate();


/** Destroys the Instrumentation data, NULLing the pointer.
  *
  *@param InstDataPointerAddress the address of the Instrumentation data pointer, which will be NULL after this call
  */
void instDestroy(InstData **InstDataPointerAddress);

/** Sets instrumentation directory, where we write instrumentation data. Its usually instrumentation/#-#-#... where # is the number
  * of instances each filter has in the pipeline
  *
  * @param dir the directory
  */
void instSetDir(InstData *inst, char *dir);


/** enter a state, which can be a void state or am user state. The current state timings are computed and
  * is stacked, so we can return to it later
  *@param state the state we are changing to
  *@param isUserState is this state a user state?
  */
void instEnterState(InstData *inst, InstState *state);

/** switches the state, which means the current one is computed but not stacked, and current is set to the new one */
void instSwitchState(InstData *inst, InstState *state);

/** leaves the current state, computing times for it. The current state is set to the Top of the stack, which is poped
  * @param inst the instrumentation data
  */
void instLeaveState(InstData *inst);



/** Saves all instrumentation data to a file, appending time there
  *@param filename the name of the file to save
  *@param label a label to write with the timing results
  */
void instSaveTimings(InstData *inst, char *filename, char *label);

/** Adds user states to the instrumentation.
  * User can give us his own states 
  *
  * @param userStatesArray an array of user states names, NULL if not used
  * @param numStates the number of states in the user array
*/
void instSetUserStates(InstData *inst, char **userStatesArray, int numStates);

#ifdef BMI_FT
void instSaveIntermediaryTimings(InstData *inst, char *filename, char *label);
#endif

#endif
