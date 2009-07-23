
#include <string.h>
#include "Policies.h"
#include "../constants.h"

/// This functions returns the writePolicy given its name
writePolicy_t getWritePolicyByName(const char *name){
	if 		(strncasecmp(name, "random", MAX_PLNAME_LENGTH)==0)
		return RANDOM;
	else if (	(strncasecmp(name, "ls", MAX_PLNAME_LENGTH)==0)||
			(strncasecmp(name, "labeled_stream", MAX_PLNAME_LENGTH)==0))
		return LABELED_STREAM;
	else if (	(strncasecmp(name, "mls", MAX_PLNAME_LENGTH)==0)||
			(strncasecmp(name, "multicast_labeled_stream", MAX_PLNAME_LENGTH)==0))
		return MULTICAST_LABELED_STREAM;
	else if (	strncasecmp(name, "broadcast", MAX_PLNAME_LENGTH)==0)
		return BROADCAST;
	else if (	(strncasecmp(name, "rr", MAX_PLNAME_LENGTH)==0) || 
			(strncasecmp(name, "", MAX_PLNAME_LENGTH)==0) ||
			(strncasecmp(name, "round_robin", MAX_PLNAME_LENGTH)==0))
		return ROUND_ROBIN;
	else 
		return W_POLICY_ERROR;

}

