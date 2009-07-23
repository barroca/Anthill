#define _INSTRUMENTATION_C_

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Instrumentation.h"

InstData *instCreate(){
	InstData *instData = (InstData*)malloc(sizeof(InstData));

	int i;
	//initialize void states
	for (i=0;i<TIMER_NUM_VOID_STATES;i++){
		timerclear(&instData->voidStates[i].timing);
		instData->voidStates[i].stateName = timerStateNames[i]; 
		instData->voidStates[i].isUserState = 0;
	}

	//user stuff
    instData->numUserStates = 0;
    instData->userStates = NULL;

	//start timer
	gettimeofday(&instData->tv, &instData->tz);

	//current state and state stack
	instData->currentState = &instData->voidStates[TIMER_VOID];
	instData->stackedStates = stCreate();

	return instData;
}

void instDestroy(InstData **instDataPointerAddress){
	//free user states
	int i;
	InstData *instData = instDataPointerAddress[0];
	if (instData->numUserStates > 0 ){
		for (i=0;i<instData->numUserStates;i++){
			free(instData->userStates[i].stateName);
		}
		free(instData->userStates);
	}
	
	free(instData);
	instDataPointerAddress[0] = NULL;
}

void instSetDir(InstData *inst, char *dir){
	int len = strlen(dir);
	if (len > MAX_IDIR_LENGTH){
		len =  MAX_IDIR_LENGTH;
		fprintf(stderr, "%s %d: warning, dir length too big, truncating\n", __FILE__, __LINE__);
	}
	strncpy(inst->instDir, dir, len);
	inst->instDir[len] = '\0';

	//now we create the directory
	char cmd[MAX_IDIR_LENGTH + 15];
	sprintf(cmd, "mkdir -p %s", dir);
	if (system(cmd) != 0){
		fprintf(stderr, "%s %d: error, cant create %s instrumention directory\n", __FILE__, __LINE__, inst->instDir);
	}	
}


static void instComputeTimings(InstData *inst){
	InstState *currentState = inst->currentState;

	struct timeval tv;
	struct timezone tz; //not used

	//get the time
	gettimeofday(&tv, &tz);

	//now we substract the time we just got from the current timing
	//we add 1000000 to usecs and subtract 1 from seconds, then we adjust later
	suseconds_t usecs = 1000000 + tv.tv_usec - inst->tv.tv_usec;
	time_t secs = tv.tv_sec - inst->tv.tv_sec - 1;

	//and compute the for the current state
	struct timeval *curTiming = (&currentState->timing);
	curTiming->tv_sec += secs;
	curTiming->tv_usec += usecs;

	//adjust timings, as usecs cant be bigger than 1000000
	while (curTiming->tv_usec > 1000000){
		curTiming->tv_sec++;
		curTiming->tv_usec -= 1000000;
	}
}

void instEnterState(InstData *inst, InstState *state){
	//compute the timings
	instComputeTimings(inst);
	
	//push current state
	stPush(inst->stackedStates, inst->currentState);

	//change state
	inst->currentState = state;
	
	//update the current timer
	gettimeofday(&inst->tv, &inst->tz);
}

void instLeaveState(InstData *inst){
	//compute the timings
	instComputeTimings(inst);
	
	//pop a state current state
	InstState *state;
	if (stPop(inst->stackedStates, (void**)&state) == -1){
		fprintf(stderr, "%s %d, %s: cant pop stack\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}

	//change state
	inst->currentState = state;
	
	//update the current timer
	gettimeofday(&inst->tv, &inst->tz);

}

void instSwitchState(InstData *inst, InstState *state){
	//compute timing for current state
	instComputeTimings(inst);
	//change state
	inst->currentState = state;
	//update the current timer
	gettimeofday(&inst->tv, &inst->tz);
}

void instSaveTimings(InstData *inst, char *filename, char *label){
	char fullName[MAX_IDIR_LENGTH + MAX_FNAME_LENGTH];
	sprintf(fullName, "%s/%s", inst->instDir, filename);
	FILE *fp = fopen(fullName, "a+");
	if (fp == NULL){
		fprintf(stderr, "%s %d: cannot open %s for writing timing results\n", __FILE__, __LINE__, fullName);
		return;
	}
	int i;
	fprintf(fp, "\nExecution time for %s\n", label);
	//we dont print last state
	for (i=0;i<TIMER_NUM_VOID_STATES-1;i++){
		fprintf(fp, "%-20s: %10ld.%06ld seconds\n", 
				inst->voidStates[i].stateName, 
				inst->voidStates[i].timing.tv_sec, 
				inst->voidStates[i].timing.tv_usec);
	}
	if (inst->numUserStates > 0){
	fprintf(fp, "User States:\n");
		for (i=0;i<inst->numUserStates;i++){
			fprintf(fp, "%-20s: %10ld.%06ld seconds\n", 
					inst->userStates[i].stateName,
				   	inst->userStates[i].timing.tv_sec, 
					inst->userStates[i].timing.tv_usec);
		}
	}
	fclose(fp);
}

/// \todo Copy the data from user
void instSetUserStates(InstData *inst, char **userStates, int numStates){
	inst->numUserStates = numStates;
	inst->userStates = (InstState*)malloc(sizeof(InstState)*numStates);
	int i;
	for (i=0;i<numStates;i++){
		timerclear(&inst->userStates[i].timing);
		inst->userStates[i].stateName = strdup(userStates[i]);
		inst->userStates[i].isUserState = 1;
	}
}

void backupUserStates(InstData *inst) {
#ifdef BMI_FT
	backupUserSt = (BackupUserSt *) malloc(sizeof(BackupUserSt));

	backupUserSt->numUserStates = inst->numUserStates;
	backupUserSt->userStates = (InstState *) malloc(inst->numUserStates*sizeof(InstState));

	int i;
	for(i = 0; i < inst->numUserStates; i++) {
		backupUserSt->userStates[i].stateName = strdup(inst->userStates[i].stateName);
		backupUserSt->userStates[i].isUserState = inst->userStates[i].isUserState;
	}
#endif
}

void restoreUserStates (InstData *inst) {
#ifdef BMI_FT
	inst->numUserStates = backupUserSt->numUserStates;
	inst->userStates = (InstState*) malloc(sizeof(InstState)*backupUserSt->numUserStates);

	int i;
	for (i = 0; i < backupUserSt->numUserStates; i++){
		timerclear(&inst->userStates[i].timing);
		inst->userStates[i].stateName = strdup(backupUserSt->userStates[i].stateName);
		inst->userStates[i].isUserState = backupUserSt->userStates[i].isUserState;
	}
#endif
}

void destroyUserStateBackup() {
#ifdef BMI_FT
	int i;

	for(i = 0; i < backupUserSt->numUserStates; i++) {
		free(backupUserSt->userStates[i].stateName);
	}

	if(backupUserSt->numUserStates > 0) {
		free(backupUserSt->userStates);
	}

	backupUserSt->numUserStates = 0;

#endif
}

#ifdef BMI_FT
void instSaveIntermediaryTimings(InstData *inst, char *filename, char *label){
	char fullName[MAX_IDIR_LENGTH + MAX_FNAME_LENGTH];
	sprintf(fullName, "%s/%s", inst->instDir, filename);
	FILE *fp = fopen(fullName, "w");
	if (fp == NULL){
		fprintf(stderr, "%s %d: cannot open %s for writing timing results\n", __FILE__, __LINE__, fullName);
		return;
	}
	int i;
	fprintf(fp, "\nExecution time for %s\n", label);
	//we dont print last state
	for (i=0;i<TIMER_NUM_VOID_STATES-1;i++){
		fprintf(fp, "%-20s: %10ld.%06ld seconds\n", 
				inst->voidStates[i].stateName, 
				inst->voidStates[i].timing.tv_sec, 
				inst->voidStates[i].timing.tv_usec);
	}
	if (inst->numUserStates > 0){
	fprintf(fp, "User States:\n");
		for (i=0;i<inst->numUserStates;i++){
			fprintf(fp, "%-20s: %10ld.%06ld seconds\n", 
					inst->userStates[i].stateName,
				   	inst->userStates[i].timing.tv_sec, 
					inst->userStates[i].timing.tv_usec);
		}
	}
	fclose(fp);
}
#endif
