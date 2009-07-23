#ifndef _STREAM_SPEC_H_
#define _STREAM_SPEC_H_

#include "constants.h"
#include "structs.h"
#include "FilterSpec.h"

/* Constructor and destroyer **************************************/
StreamSpec *createEmptyStreamSpec();
void destroyStreamSpec(StreamSpec *s);
/******************************************************************/

/* setting source and destination**********************************/
int setFrom(StreamSpec *s, FilterSpec *filter, char *portName
#ifdef VOID_TERM		
, int breakLoop
#endif
);
int setTo(StreamSpec *s, FilterSpec *filter, char *portName,  char *writePolicyName, char *polibName);
/******************************************************************/
void resetStream(StreamSpec *s);

#endif
