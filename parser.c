/**
*Alterado: Itamar 03/10/2006
*/

#include "parser.h"
#include "structs.h"
#include "constants.h"
#include "FilterSpec.h"
#include "StreamSpec.h"
#include "Layout.h"
#include "FilterData/Policies.h"
#include "Hosts.h"
#include "color.h"
#ifdef MATLAB
#include "MatLabDesc.h"
#endif
// global: parser state
static State state = start, lastState = start;
static char errorMsg[200];

static char *stateNames[] = {
	        "start", "prehost", "hostdec","preplace", 
		"placement", "prelay", "layout", 
		"preend", "end", "resources", "filter", 
		"stream", "fromto", "tostream", 
		"error", "parseerror", "matLab", 
		"matLabFilter", "matLabFunction", 
		"matLabFunctionArg", "preMatLab"
};

static int currentHostIndex; //the host index we are reading resources
static FilterSpec *currentFilter = NULL; // the filter we are reading
static StreamSpec *currentStream = NULL; //the stream we are reading
static int remainingInstances, instances; //each filter runs on X hosts, we use this variable to control this

#ifdef MATLAB
static FilterSpec *currentMatLabFilter = NULL;
#endif

/*! \file parser.c This file parses the conf.xml, using the expat lib  */

/// this functions is called whenever a tag is open. 
void abreTag(void *userData, const XML_Char *name, const XML_Char **atts){
	Layout *systemLayout = (Layout*)userData;

	lastState = state;
	switch (state){
		case start:
			//we expect config tag here
			if (strcasecmp("config", name) ==  0)
				state = prehost;
			else {
				strcpy(errorMsg, "Expecting <config>\n");
				state = error;
			}
		break;
		case prehost:
			//we expect hostdec tag here
			if (strcasecmp("hostdec", name) ==  0){
				state = hostdec;
			}
			else {
				strcpy(errorMsg, "Expecting <hostdec>\n");
				state = error;
				return;
			}
		break;
		case hostdec:
			//we expect host tag here
			if (strcasecmp("host", name) !=  0){
				strcpy(errorMsg, "Expecting <host name=\"\" [weight=\"\"]/>\n");
				state = error;
				return;
			}


			int wi = -1, ni = -1, mi = -1, weight, mem = -1;
			int i=0;
			char *hname;

			while (atts[i] != NULL){
				if (strcasecmp(atts[i], "name") == 0){
					ni= i+1;	
				}
				else if (strcasecmp(atts[i], "weight") == 0){
					wi = i+1;
				}
				else if (strcasecmp(atts[i], "mem") == 0){
					mi = i+1;
				}
				else {
					strcpy(errorMsg, "Expecting name or weight attribute\n");
					state = error;
					return;
				}
				i += 2;
			}

			if (ni == -1){
				strcpy(errorMsg, "Expecting <host name=\"\" [weight="" mem=""]/>\n");
				state = error;
				return;
			}
			else {
				hname = strdup(atts[ni]);
			}

			if (wi == -1){
				weight = 1;
			}
			else {
				weight = atoi(atts[wi]);
			}

			//memory: if mem == -1, filter must autodetect memory
			//reading /proc/meminfo
			if (!((mi == -1) || (strcasecmp(atts[mi], "auto") == 0 ))){
				int len = strlen(atts[mi]);
				int unitType = 0; // 0 = MB(default), 1 = GB

				//skip trailing B if exists
				if ((atts[mi][len-1] == 'B') || (atts[mi][len-1] == 'b')){
					len--;
				}
				
				//find the unit of memory
				if ((atts[mi][len-1] == 'G') || (atts[mi][len-1] == 'g')){
					unitType = 1;
					len --;
				}
				else if ((atts[mi][len-1] == 'M') || (atts[mi][len-1] == 'm')){
					unitType = 0;
					len --;
				}
				else if ((atts[mi][len-1] == 'T') || (atts[mi][len-1] == 't')){
					unitType = 2;
					len --;
				}
				mem = atoi(atts[mi]) * pow((double)1024, (double)unitType);
			}


			//add the host, but no duplicates
			int hindex = hostsAdd(systemLayout->hostsStruct);
			if (hostsSetName(systemLayout->hostsStruct, hindex, hname) == -1 ) {
				state = error;
				sprintf(errorMsg, "Could not add host %s", hname);
				return;
			}
			hostsSetWeight(systemLayout->hostsStruct, hindex, weight);
			hostsSetMemory(systemLayout->hostsStruct, hindex, mem);
			printf("\tparser.c: host %s added to the Void enviroment, weight %d\n", hname, weight);
			free(hname);
			state = resources; //we go reading resources
			currentHostIndex = hindex;
		break;
		case resources:
		{
			//here we read resources for a given host
			if (strcasecmp(name, "resource") != 0){
				sprintf(errorMsg, "Error, expecting <resource tag>");
				state = error;
				return;
			}

			int rni = -1, i=0; //resource name index
			char resourceName[MAX_RNAME_LENGTH+1];
			//read resource name
			while (atts[i] != NULL) {
				if (strcasecmp("name", atts[i]) == 0){
					rni = i+1;
				}
				else {
					sprintf(errorMsg, "Error, expecting <resource name=\"\">");
					state = error;
					return;
				}
				i+=2;
			}

			if (rni == -1){
				sprintf(errorMsg, "Resource name is mandatory");
				state=error;
				return;
			}
			else {
				strncpy(resourceName, atts[rni], MAX_RNAME_LENGTH);
			}

			HostsStruct *hs = systemLayout->hostsStruct;
			if (hostsAddResource(hs, currentHostIndex, resourceName) < 0){
				sprintf(errorMsg, "Error adding resource %s: too many resources for host %s", 
						resourceName, hostsGetName(hs, currentHostIndex));
				state = error;
				return;
			}
			printf("\t\tparser.c: resource %s added to host %s\n", resourceName, hostsGetName(hs, currentHostIndex));
			//no state change
		}
		break;
		case preplace:
			//expect placement tag
			if (strcasecmp("placement", name) == 0)
				state = placement;
			else {
				strcpy(errorMsg, "Expecting <placement>");
				state = error;
			}
		break;
		case placement:
		{
			char filterName[MAX_FNAME_LENGTH+1], libname[MAX_LNAME_LENGTH+1];
			int nameInd = -1, libInd = -1, instInd=-1, i=0, 
#ifdef ATTACH
			attachedInd = -1, attached=-1, // used to sinalize tha a filter has been attached 
			attachInd = -1, attach=-1, // used to inform that this is a filter that works
			   			   // as a attachment filter so it can be used attached in
						   // other filters pipeline
#endif
#ifdef BMI_FT
			lastFilterInd = -1, lastFilter = -1,
#endif
			instances=-1;

			// we read the filter entry here
                        if ( (strcasecmp("filter", name) != 0) ){
				state = error;
				strcpy(errorMsg, "Expecting <filter name=\"filtername\" [instances=\"N\" libname=\"library.so\"] >");
				return;
			}

			//filter attributes
			while (atts[i] != NULL){
				if ( strcasecmp(atts[i], "name") == 0 ){
					nameInd = i+1;
					//mandatory
				}
				else if ( strcasecmp(atts[i], "libname") == 0 ){
					libInd = i+1;
					//optional: default is to use libNAME.so
				}
				else if ( strcasecmp(atts[i], "instances") == 0 ){
					instInd = i+1;
					//optional: defaults to 1
				}
#ifdef ATTACH				
				else if ( strcasecmp(atts[i], "attached") == 0){
					attachedInd = i+1;
				}
				else if ( strcasecmp(atts[i], "attach") == 0){
					attachInd = i+1;
				}
#endif
#ifdef BMI_FT
				else if ( strcasecmp(atts[i], "lastfilter") == 0){
					lastFilterInd = i+1;
				}
#endif
				else {
					state = error;
					strcpy(errorMsg, "Expecting <filter name=\"filtername\" [instances=\"N\" libname=\"library.so\" attached=\"yes or no\"] >");
					break;
				}
				i+=2;
			}

			//name is mandatory
			if (nameInd == -1){
				state = error;
				strcpy(errorMsg, "Name attribute is mandatory, expecting <filter name=\"filtername\" [instances=\"N\" libname=\"library.so\"] >");
				return;
			}
			else { //get the name
				strncpy(filterName, atts[nameInd], MAX_FNAME_LENGTH);
				filterName[MAX_FNAME_LENGTH] = '\0';
			}

			// if we did not receive the library name, we add lib and .so to filtername
			if (libInd == -1){ 
				sprintf(libname, "lib%s.so", filterName);
			}
			else { //else we use library from user
				strncpy(libname, atts[libInd], MAX_LNAME_LENGTH);
				libname[MAX_LNAME_LENGTH] = '\0';
			}

			//get optional number of instances
			if (instInd == -1){
				instances = -1; //default value, auto detect
				remainingInstances = 1; //we spawn at least 1 instances
			}
			else {
				instances = atoi(atts[instInd]);
				remainingInstances = instances; //we'll spawn this number of instances
			}
#ifdef ATTACH			
			// used to know if a filter is attached or no.
			if (attachedInd == -1){
				// this filter is not attached so i need spawn it
				attached = 0;	
			}
			else{
				if ( strcasecmp(atts[attachedInd], "yes") == 0){
					attached = 1;	
				}else if ( strcasecmp(atts[attachedInd], "no") == 0){
					attached = 0;
				} else{
					state = error;
					strcpy(errorMsg, "erro parser.c:In filter define the attached must be \"yes\" or \"no\"");
				}
				printf("Attached found = %s int value = %d", atts[attachedInd], attached);
			}
			// used to know if a filter can be attached or no.
			if (attachInd == -1){
				// this filter is not attached so i need spawn it
				attach = 0;	
			}
			else{
				if ( strcasecmp(atts[attachInd], "yes") == 0){
					attach = 1;	
				}else if ( strcasecmp(atts[attachInd], "no") == 0){
					attach = 0;
				} else{
					state = error;
					strcpy(errorMsg, "erro parser.c:In filter define the attach must be \"yes\" or \"no\"");
				}
				printf("Attach found = %s int value = %d", atts[attachInd], attach);
			}

#endif
#ifdef BMI_FT
			// used to know if a filter is the last one or no.
			if (lastFilterInd == -1){
				// this filter is not the last one
				lastFilter = 0;	
			}
			else{
				if ( strcasecmp(atts[lastFilterInd], "yes") == 0){
					lastFilter = 1;	
				}else if ( strcasecmp(atts[lastFilterInd], "no") == 0){
					lastFilter = 0;
				} else{
					state = error;
					strcpy(errorMsg, "erro parser.c:In filter define the attached must be \"yes\" or \"no\"");
				}
				printf("lastfilter found = %s int value = %d", atts[lastFilterInd], lastFilter);
			}
#endif
			//create the filter!
			currentFilter = createFilterSpec(filterName, libname);

#ifdef ATTACH
			setAttached(currentFilter, attached);
			setAttach(currentFilter, attach);
#endif
#ifdef BMI_FT
			setLastFilter(currentFilter, lastFilter);
#endif
			if (instances == -1){
				printf("\tparser.c: reading filter %s, library %s, auto number of instances\n", filterName, libname);
			}
			else {
				printf("\tparser.c: reading filter %s, library %s, %d instances\n", filterName, libname, instances);
			}

			//update state
			state = filter;
		}
		break;
		case filter:
		{
			//read the hosts of each filter
			if (strcasecmp("instance", name) != 0){
				//error, did not receive host
				state = error;
				strcpy(errorMsg, "Expecting <instance demands=\"hostname or resource\" [qty=\"n\"]/>, n >");
				return;
			}

			int demandInd = -1, numInstInd = -1; //attribute index
			int i = 0;
			
			while (atts[i] != NULL){
				// even indexes are attribute names
				// odd indexes are values
				//ex: <host name="bla"/>
				//atts[0]=name, atts[1]=bla, atts[2]=NULL
				if (strcasecmp(atts[i], "demands") == 0){
					demandInd = i+1;
				}
				else if (strcasecmp(atts[i], "numinstances") == 0){
					numInstInd = i+1;
				} 
				else {
					state = error;
					strcpy(errorMsg, "Expecting <host name=\"hostname\" [numinstances=\"n\"]/>, n > 0");
					return;
				}
				i+=2;
			}

			if (demandInd == -1){
				state = error;
				strcpy(errorMsg, "Instance must demand something, either a hostname or a resource");
				return;
			}

			//we declared N instances, and are trying to spawn more
			if ((instances != -1) && (remainingInstances <= 0)){
				printf("\t\tparser.c: WARNING: ignoring instance, %d declared\n", instances);
				return;
			}

			int numInstances = 1;
			//how many instances, default value is 1
			if (numInstInd != -1){
				numInstances = atoi(atts[numInstInd]);
			}
			
			for (i=0;i<numInstances;i++){
				HostsStruct *hs = systemLayout->hostsStruct;
				//resource name
				char resourceName[MAX_RNAME_LENGTH+1];
				strncpy(resourceName, atts[demandInd], MAX_RNAME_LENGTH);

				//here we add the host to the filter
				//first we try to get a host with the resource. If not found, we try to get the host with the given name
				int hostIndex = hostsGetIndexByResource(hs, resourceName);
				if (hostIndex == -1){
					//try to get host by name
					hostIndex = hostsGetIndexByName(hs, resourceName);
					if (hostIndex == -1){
						sprintf(errorMsg, "Error, resource %s invalid or host not found", resourceName);
						state = error;
						return;
					}
				}

				char hostname[MAX_HNAME_LENGTH+1];
				strcpy(hostname, hostsGetName(hs, hostIndex));
				printf("\t\tparser.c: host %s running this filter\n", hostname);

				addHostToFilter(currentFilter, hs->hosts[hostIndex].name);
				addResourceToFilter(currentFilter, resourceName);
				remainingInstances--;
			}
			break;
		}
		case prelay:
			//reads <layout>
			if (strcasecmp("layout", name) == 0)
				state = layout;
			else {
				state = error;
				strcpy(errorMsg, "Expecting <layout>");
				return;
			}
		break;
		case layout:
			//reads stream
                        if (strcasecmp("stream", name) == 0){
				state = stream;

				//creates the stream
				currentStream = createEmptyStreamSpec();
			}
                        else {
                                state = error;
                                strcpy(errorMsg, "Expecting <stream>");
				return;
                        }

		break;
		case stream:
			//reads <from />
			if (strcasecmp(name, "from") == 0){
				int filInd = -1, portInd = -1;
#ifdef VOID_TERM
				int breakLoop = 0;
#endif
				char filterName[MAX_FNAME_LENGTH+1], portName[MAX_PTNAME_LENGTH+1];
				int i=0;
				//dont want to read junk
				filterName[0] = '\0';
				portName[0] = '\0';

				while (atts[i] != NULL){
					if ( strcasecmp(atts[i], "filter") == 0 ){
						filInd = i+1;
						strncpy(filterName, atts[i+1], MAX_FNAME_LENGTH);
						filterName[MAX_FNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "port") == 0 ){
						portInd = i+1;
						strncpy(portName, atts[i+1], MAX_PTNAME_LENGTH);
						portName[MAX_PTNAME_LENGTH] = '\0';
					}
#ifdef VOID_TERM
					else if ( strcasecmp(atts[i], "breakLoop") == 0){
						if ( strcasecmp(atts[i+1], "yes") == 0){
							breakLoop = 1;	
						}else if ( strcasecmp(atts[i+1], "no") != 0){
							state = error; // exist break loop but its not true or false
						}
					}
#endif
					else {
						//invalid attribute
						state = error;
					}

					i+=2;
				}

				if ( (filInd == -1) || (portInd == -1) || (state == error) ) {
					state = error;
					strcpy(errorMsg, "Expecting <from filter=\"filter_name\" port=\"port_name\"/>  policy and policylib are defined in tag to");
				}
				else {

					// fills stream FROM part
					if (!setFrom(currentStream,
						getFilterSpecByName(systemLayout, filterName),
						portName
#ifdef VOID_TERM
						, breakLoop
#endif
						)){
						state = error;
						strcpy(errorMsg, "Error in tag FROM");
						return;
					}
					printf("\tparser.c: filter %s:%s writes to\n", filterName, portName);


				}
			}
                        else {
                                state = error;
				strcpy(errorMsg, "Expecting <from filter=\"filter_name\" port=\"port_name\" /> policy and policylib are defined in tag to");
                        }

		break;
		case fromto:
			//reads stream <to />
			if (strcasecmp(name, "to") == 0){
				int filInd = -1, portInd = -1, polInd = -1, polibInd = -1;
				int i=0;
				//ITAMAR: The policyName will be set on
				char filterName[MAX_FNAME_LENGTH+1], portName[MAX_PTNAME_LENGTH+1], 
					policyName[MAX_PLNAME_LENGTH+1], polibName[MAX_PLNAME_LENGTH+1];
				//dont want to read junk
				policyName[0] = '\0';
				polibName[0] = '\0';
				filterName[0] = '\0';
				portName[0] = '\0';

				while (atts[i] != NULL){
					if ( strcasecmp(atts[i], "filter") == 0 ){
						filInd = i+1;
						strncpy(filterName, atts[i+1], MAX_FNAME_LENGTH);
						filterName[MAX_FNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "port") == 0 ){
						portInd = i+1;
						strncpy(portName, atts[i+1], MAX_PTNAME_LENGTH);
						portName[MAX_PTNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "policy") == 0 ){
						polInd = i+1;
						strncpy(policyName, atts[i+1], MAX_PLNAME_LENGTH);
						policyName[MAX_PLNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i],  "policyLib") == 0 ){
						polibInd = i+1;
						strncpy(polibName, atts[i+1], MAX_PLNAME_LENGTH);
						policyName[MAX_PLNAME_LENGTH] = '\0';
					}
					else {
						//invalid attribute
						state = error;
					}
					i+=2;
				}

				if ( (filInd == -1) || (portInd == -1) || (state == error) ){
					state = error;
					strcpy(errorMsg, "Expecting <to filter=\"filter_name\" port=\"port_name\" [policy=\"policy_name\"] />");
				}
				else {
					//default policy
					if (strlen(policyName) == 0)  //ITAMAR 
						strcpy(policyName, "rr");
					
					// fills stream TO part
					if (!setTo(currentStream, 
						getFilterSpecByName(systemLayout, filterName), 
						portName, policyName, polibName)){
						state = error;
						strcpy(errorMsg, "Error setting stream destination.\nInvalid Policy or labeled stream requires a policy lib name");
						return;
					}
					if (polInd != -1)
						printf("\tfilter %s:%s(%s)\n", filterName, portName, policyName);
					else
						printf("\tfilter %s:%s(default policy)\n", filterName, portName);
				}
			}
			else {
                                state = error;
				strcpy(errorMsg, "Expecting <to filter=\"filter_name\" port=\"port_name\" />");
			}
		break;
		case tostream:
			//reads stream <to /> ITAMAR: Alteration to permit that one 'from' have various 'to'
			if (strcasecmp(name, "to") == 0){
				int filInd = -1, portInd = -1, polInd = -1, polibInd = -1;
				int i=0;
				//ITAMAR: The policyName will be set on
				state= fromto;
				char filterName[MAX_FNAME_LENGTH+1], portName[MAX_PTNAME_LENGTH+1], 
					policyName[MAX_PLNAME_LENGTH+1], polibName[MAX_PLNAME_LENGTH+1];
				//dont want to read junk
				policyName[0] = '\0';
				polibName[0] = '\0';
				filterName[0] = '\0';
				portName[0] = '\0';

				while (atts[i] != NULL){
					if ( strcasecmp(atts[i], "filter") == 0 ){
						filInd = i+1;
						strncpy(filterName, atts[i+1], MAX_FNAME_LENGTH);
						filterName[MAX_FNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "port") == 0 ){
						portInd = i+1;
						strncpy(portName, atts[i+1], MAX_PTNAME_LENGTH);
						portName[MAX_PTNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "policy") == 0 ){
						polInd = i+1;
						strncpy(policyName, atts[i+1], MAX_PLNAME_LENGTH);
						policyName[MAX_PLNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i],  "policyLib") == 0 ){
						polibInd = i+1;
						strncpy(polibName, atts[i+1], MAX_PLNAME_LENGTH);
						policyName[MAX_PLNAME_LENGTH] = '\0';
					}
					else {
						//invalid attribute
						state = error;
					}
					i+=2;
				}

				if ( (filInd == -1) || (portInd == -1) || (state == error) ){
					state = error;
					strcpy(errorMsg, "Expecting <to filter=\"filter_name\" port=\"port_name\" [policy=\"policy_name\"] />");
				}
				else {
					//default policy
					if (strlen(policyName) == 0)  //ITAMAR 
						strcpy(policyName, "rr");
					
					// fills stream TO part
					if (!setTo(currentStream, 
						getFilterSpecByName(systemLayout, filterName), 
						portName, policyName, polibName)){
						state = error;
						strcpy(errorMsg, "Error setting stream destination.\nInvalid Policy or labeled stream requires a policy lib name");
						return;
					}
					if (polInd != -1)
						printf("\tfilter %s:%s(%s)\n", filterName, portName, policyName);
					else
						printf("\tfilter %s:%s(default policy)\n", filterName, portName);
				}
			}
			else {
                                state = error;
				strcpy(errorMsg, "Expecting <to filter=\"filter_name\" port=\"port_name\" [policy=\"policy_name\"] />");
			}/*
			// we cant open tag in this state
			state = error;
			strcpy(errorMsg, "Expecting </stream>");*/
		break;
		case preend:
#ifdef MATLAB		
			// same
			if ( strcasecmp("matLab", name) == 0){
				state = matLab;
				printf("Open MatLabSection:\n");
			}else{
#endif				
				state = error;
				strcpy(errorMsg, "Expecting </config>");
#ifdef MATLAB		
				strcpy(errorMsg, "Expecting </config> or </matLab> or <matLab>");
			}	
#endif			
		break;

#ifdef MATLAB
		case matLab:
			if (strcasecmp(name, "matLabFilter") == 0){
				int filterNameInd = -1, matLabLibNameInd = -1, matLabfirstFilterInd = -1;
				int i = 0, firstFilter = 0;
				char matLabLibName[MAX_LNAME_LENGTH];
				char filterName[MAX_FNAME_LENGTH]; 
#ifdef DEBUG
				printf("Open matLabFilter:");
#endif				
				while (atts[i] != NULL){
					if ( strcasecmp(atts[i], "name") == 0 ){
						filterNameInd = i+1;
						strncpy(filterName, atts[i+1], MAX_FNAME_LENGTH);
						filterName[MAX_FNAME_LENGTH] = '\0';
						
					}
					else if ( strcasecmp(atts[i], "matlablibname") == 0 ){
						matLabLibNameInd = i+1;
						strncpy(matLabLibName, atts[i+1], MAX_LNAME_LENGTH);
						matLabLibName[MAX_LNAME_LENGTH] = '\0';
					}else if( strcasecmp(atts[i], "firstFilter") == 0 ){
						matLabfirstFilterInd = i+1;

						if ( strcasecmp(atts[matLabfirstFilterInd], "yes") == 0){
							firstFilter = 1;	
						}else if ( strcasecmp(atts[matLabfirstFilterInd], "no") == 0){
							firstFilter = 0;
						} else{
							state = error;
							strcpy(errorMsg, "erro parser.c:In matLabfilter define the firstFilter must be \"yes\" or \"no\"");
						}
					}else {
						state = error;	
					}
					i+=2;
				}
				if( (filterNameInd == -1) || (matLabLibNameInd == -1) || (state == error) ){
					state = error;
					strcpy(errorMsg, "Expecting 	<matLabFilter name=\"filterName\" matlablibname=\"matLabFunction.so\">");
				}else {
					// setar isso usando currentMatlabFilter
					currentMatLabFilter = getFilterSpecByName(systemLayout, filterName);
					setMatLabLibName(&(currentMatLabFilter->matLabDesc), matLabLibName);
					setFirstFilter(&(currentMatLabFilter->matLabDesc), firstFilter);
#ifdef DEBUG					
					printf("MatLab: filterName = %s and matlablibname = %s\n", filterName, matLabLibName);
#endif					
				}
				state = matLabFilter;
			}	
		break;
		case matLabFilter:
			if (strcasecmp(name, "function") == 0){
				int headernameInd = -1, numoutputsInd = -1, numinputoutputsInd = -1, numinputsInd = -1;
				int i = 0;
			
				char headerName[MAX_FUNCNAME_LENGTH]; 
				int numoutputs = 0, numinputoutputs = 0, numinputs = 0;
				state = matLabFunction;

				while (atts[i] != NULL){
					if ( strcasecmp(atts[i], "headername") == 0 ){
						headernameInd = i+1;
						strncpy(headerName, atts[i+1], MAX_FUNCNAME_LENGTH);
						headerName[MAX_FUNCNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "numoutputs") == 0 ){
						numoutputsInd = i+1;
					}
					else if ( strcasecmp(atts[i], "numinputoutpus") == 0 ){
						numinputoutputsInd = i+1;
					}
					else if ( strcasecmp(atts[i], "numinputs") == 0 ){
						numinputsInd = i+1;
					}
					else{
						state = error;
					}	
					i += 2;
				}	
				if ( (headernameInd == -1) || (state == error)) {
					state = error;
					strcpy(errorMsg, "<function headername=\"functionHeaderName\" [numoutputs=\"num\" numimputsoutpus=\"num\" numimputs=\"num\"]/>");
				}else{
					if(numoutputsInd != -1){
						numoutputs = atoi(atts[numoutputsInd]);	
					}
					if(numinputsInd != -1){
						numinputs = atoi(atts[numinputsInd]);
					}
					if(numinputoutputsInd != -1){
						numinputoutputs = atoi(atts[numinputoutputsInd]);
					}
					setMatLabFunctionName(&(currentMatLabFilter->matLabDesc), headerName);

					setNumArgs(&(currentMatLabFilter->matLabDesc), numinputs, numinputoutputs, numoutputs);
				}
			}else {
				strcpy(errorMsg, "Expecting <function>");
				state = error;
			}	
		break;
		case matLabFunction:
			if (strcasecmp(name, "argument") == 0){
				int argTypeInd = -1, inputTypeInd = -1, userArgindexInd = -1, 
				deserializeInd = -1, deslibnameInd = -1, serializeInd = -1, 
				serializelibnameInd = -1, orderInd = -1, msgIndexInInd = -1,
				msgIndexOutInd = -1;


				int i = 0;
				char argType[MAX_ARGTYPE_NAME_LENGTH];
				char inputType[MAX_INPUTYPE_NAME_LENGTH];
				int userArg = 0;
				char deserializeFunctionName[MAX_FUNCNAME_LENGTH];
				char deserializeLibName[MAX_LNAME_LENGTH];

				char serializeFunctionName[MAX_FUNCNAME_LENGTH];
				char serializeLibName[MAX_LNAME_LENGTH];
				int parameterOrder;
				int msgIndexIn = 0;
				int msgIndexOut = 0;
				
				state = matLabFunctionArg;
				while (atts[i] != NULL){
					if ( strcasecmp(atts[i], "argtype") == 0 ){
						argTypeInd = i+1;	
						strncpy(argType, atts[i+1], MAX_ARGTYPE_NAME_LENGTH);
						argType[MAX_ARGTYPE_NAME_LENGTH] = '\0';
					}
					else if (strcasecmp(atts[i], "inputtype") == 0 ){
						inputTypeInd = i+1;	
						strncpy(inputType, atts[i+1], MAX_INPUTYPE_NAME_LENGTH);
						inputType[MAX_INPUTYPE_NAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "userargindex") == 0 ){
						userArgindexInd = i+1;
						userArg = atoi(atts[userArgindexInd]);
					}
					else if ( strcasecmp(atts[i], "deserializefunction") == 0 ){
						deserializeInd = i+1;
						strncpy(deserializeFunctionName, atts[i+1], MAX_FUNCNAME_LENGTH);
						deserializeFunctionName[MAX_FUNCNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "deserializelibname") == 0 ){
						deslibnameInd = i+1;
						strncpy(deserializeLibName, atts[i+1], MAX_LNAME_LENGTH);
						deserializeFunctionName[MAX_LNAME_LENGTH] = '\0';

					}
					else if ( strcasecmp(atts[i], "serializefunction") == 0 ){
						serializeInd = i+1;
						strncpy(serializeFunctionName, atts[i+1], MAX_FUNCNAME_LENGTH);
						serializeFunctionName[MAX_FUNCNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "serializelibname") == 0 ){
						serializelibnameInd = i+1;
						strncpy(serializeLibName, atts[i+1], MAX_LNAME_LENGTH);
						serializeLibName[MAX_LNAME_LENGTH] = '\0';
					}
					else if ( strcasecmp(atts[i], "order") == 0 ){
						orderInd = i+1;
						parameterOrder = atoi(atts[orderInd]);
					}
					else if ( strcasecmp(atts[i], "msgindexin") == 0 ){
						msgIndexInInd = i+1;
						msgIndexIn = atoi(atts[msgIndexInInd]);
					}
					else if ( strcasecmp(atts[i], "msgindexout") == 0 ){
						msgIndexOutInd = i+1;
						msgIndexOut = atoi(atts[msgIndexOutInd]);
					}
					else{	
						state = error;	
					}
					i += 2;
				}

				// input parameter only
				if((deserializeInd != -1 && serializeInd == - 1) ){
					if((argTypeInd == -1)|| (inputTypeInd == -1) || 
					(deserializeInd == -1)|| (deslibnameInd == -1)|| 
					(orderInd == -1)||state == error){
						strcpy(errorMsg, "Input: Expecting <argument argtype=\"mxArray*\" inputtype=\"userArg\" userargindex=\"num\" deserializefunction=\"intToMxArray\" deserializelibname=\"deserializeLib.so\" order=\"2\"/>");
						state = error;
					}else {
						// if you are using userArg you must declare userargindex
						if(strcasecmp(atts[inputTypeInd], "userArg") == 0){
							if(userArgindexInd == -1){
								strcpy(errorMsg, "Expecting <argument argtype=\"mxArray*\" inputtype=\"userArg\" userargindex=\"1\" deserializefunction=\"intToMxArray\" deserializelibname=\"deserializeLib.so\" order=\"2\"/>");
								state = error;
							}
						}else if(strcasecmp(atts[inputTypeInd], "msg") == 0){
							if(msgIndexInInd == -1){
								strcpy(errorMsg, "Expecting <argument ... inputtype=\"msg\" msgindexin=\"num\"...");
								state = error;
							}
						}	
						
					}
					addArgInput(&(currentMatLabFilter->matLabDesc), argType, deserializeFunctionName, 
					deserializeLibName, userArg, inputType, parameterOrder, msgIndexIn);
				}
				else if( deserializeInd == -1 && serializeInd != -1) {
				// outPut parameter only
					if ((argTypeInd == -1) || ( serializeInd == -1)|| 
					(serializelibnameInd == -1)|| (msgIndexOutInd == -1)|| 
					( orderInd == -1) || state == error){
						strcpy(errorMsg, "OutPut <argument argtype=\"type\" order=\"num\" serializefunction=\"mxArray**ToDCBuffer\" serializelibname=\"serializeLib.so\" mesgIndexOut=\"1\"/>");
						state = error;
					}else{
						addArgOutput(&(currentMatLabFilter->matLabDesc), argType, serializeFunctionName, serializeLibName, parameterOrder, msgIndexOut);
					}	

				}
				else if(deserializeInd != -1 && serializeInd != -1){
				// input Output Parameter
					if ((argTypeInd == -1) || ( serializeInd == -1)|| 
					(serializelibnameInd == -1)|| (msgIndexOutInd == -1)|| 
					( orderInd == -1) || (inputTypeInd == -1) || 
					(deserializeInd == -1)|| (deslibnameInd == -1)|| 
					state == error ){

						state = error;
					}else if(strcasecmp(atts[inputTypeInd], "userArg") == 0){
						// if you are using userArg you must declare userargindex
							if(userArgindexInd == -1){
								strcpy(errorMsg, "Expecting <argument argtype=\"mxArray*\" inputtype=\"userArg\" userargindex=\"1\" deserializefunction=\"intToMxArray\" deserializelibname=\"deserializeLib.so\" order=\"2\"/>");
								state = error;
							}
						}else if(strcasecmp(atts[inputTypeInd], "msg") == 0){
							if(msgIndexInInd == -1){
								strcpy(errorMsg, "Expecting <argument ... inputtype=\"msg\" msgindexin=\"num\"...");
								state = error;
							}
						}
 					addArgInputOutput(&(currentMatLabFilter->matLabDesc), argType, deserializeFunctionName, 
					 deserializeLibName, userArg, inputType, serializeFunctionName, serializeLibName, parameterOrder,
					msgIndexIn, msgIndexOut);

				}

			}else {
				strcpy(errorMsg, "Expecting <argument>");
				state = error;
			}
		break;
#endif
		case end:
			//junk after </config>
			strcpy(errorMsg, "Expecting eof after </config>");
			state = error;
		break;
		default:
		break;
	}

}

/// this function is called whenever a tag is closed
void fechaTag(void *userData, const XML_Char *name){
	Layout *systemLayout = (Layout*)userData;

	lastState = state;
	
	switch (state){
		case hostdec:
			//close hostdec
			if ( strcasecmp("hostdec", name) == 0){
				state = preplace;
				printf("\tparser.c: ...\n");
			}
			else {
				state = error;
				strcpy(errorMsg, "Expecting </hostdec>");
			}
		break;
		case resources:
		{
			if (strcasecmp("resource", name) == 0){
				//do nothing
			}
			else if ( strcasecmp("host", name) == 0){
				state = hostdec; //go back to hostdec
			}
		}
		break;
		case prelay:
		break;
		case filter:
		{
			// host empty tag, no state change
			// or filter close tag, change state
			if ( strcasecmp("filter", name) == 0){
				while (remainingInstances > 0){
					//spawn the remaing hosts
					HostsStruct *hs = systemLayout->hostsStruct;
					int hid = hostsGetIndex(hs);
					if (hid == -1){
						state = error;
						sprintf(errorMsg, "Error filling remaining hosts");
						return;
					}
					
					
					if (addHostToFilter(currentFilter, hostsGetName(hs, hid)) == -1){
						state = error;
						sprintf(errorMsg, "Error adding host %s to filter %s", hostsGetName(hs, hid), currentFilter->name);
						return;
					}

					printf("\t\tparser.c: instance %s(chosen randomly)\n", systemLayout->hostsStruct->hosts[hid].name);
					remainingInstances--;
				}
				
				state = placement;
				addFilterSpec(systemLayout, currentFilter);
			}
			else if ( strcasecmp("instance", name) == 0 ){
				//do nothing
				
			}
			else {
				state = error;
				strcpy(errorMsg, "Expecting </filter> or <instance />");
			}
		}
		break;
		case placement:
			if (strcasecmp("placement", name) == 0){
				state = prelay;
				printf("\tparser.c: ...\n");	
			}
			else {
				strcpy(errorMsg, "Expecting </placement>");
				state = error;
			}
		break;
		case stream:
			if ( strcasecmp("from", name) == 0 ){
				state = fromto;
			}
			else {
				strcpy(errorMsg, "Expecting <from />");
				state = error;
			}
		break;
		case fromto:
			if ( strcasecmp("to", name) == 0 ){
				state = tostream;
			}
			else {
				strcpy(errorMsg, "Expecting <to />");
				state = error;
			}
		break;
		case tostream:
			if ( strcasecmp("stream", name) == 0 ){
				state = layout;
				addStreamSpec(systemLayout, currentStream);
			}
			else {
				strcpy(errorMsg, "Expecting </stream>");
				state = error;
			}

		break;
		case layout:
			if ( strcasecmp("layout", name) == 0){
				state = preend;
			}
			else {
				strcpy(errorMsg, "Expecting </layout>");
				state = error;
			}
		break;
		case preend:
			if (strcasecmp(name, "config") == 0)
				state = end;
		break;
#ifdef MATLAB		
		case matLab:
			if (strcasecmp(name, "matLab") == 0){
				state = preend;
			}else{
				printf("name = %s\n", name);
				strcpy(errorMsg, "Expecting </matLab>");
				state = error;
			}	
		break;	
		case matLabFunction:
			if (strcasecmp(name, "function") == 0){
				state = preMatLab;
			}else{
				printf("name = %s\n", name);
				strcpy(errorMsg, "Expecting </function>");
				state = error;
			}	

		break;
		case matLabFunctionArg:
			if (strcasecmp(name, "argument") == 0){
				state = matLabFunction;	
			}else{
				strcpy(errorMsg, "Expecting </argument>");
				state = error;

			}
		break;	
		case preMatLab:
			if (strcasecmp(name, "matLabFilter") == 0) {
				state = matLab;
			}else{
				strcpy(errorMsg, "Expecting </matLabFilter>");
				state = error;
			}
		break;	
#endif			
		case end:
			//junk after </config>
			state = error;
		break;
		default:
		break;
	}
}

int readConfig(char *fileName, Layout *l) {
	FILE *arq;
	char buffer[1]; //read char by char
	XML_Parser parser = XML_ParserCreate(NULL);
	
	arq = fopen(fileName, "ro");
	if (arq == NULL){
		char erro[200];
		sprintf(erro, "Error opening configuration file: %s", fileName);
		perror(erro);
		exit(1);
	}

	//handlers and user data
	XML_SetElementHandler(parser, abreTag, fechaTag);
	XML_SetUserData(parser, (void*)l);

	//loop till the end
	while ( (state != end) && (state != error) && (state != parseerror) && (!feof(arq)) ){
		fread(buffer, sizeof(char), 1, arq);

		if (XML_Parse(parser, buffer, 1, 0)== 0)
			state = parseerror;
	}

	// possible errors: 
	// parseerror- XML parse error
	// error: parse error too, a tag Void does not recognize

	// last one: unexpected eof
	if (state == parseerror){
		//XML parse error
		printf(ERR"\tparser.c: parse error!!!\n");
		printf("\tparser.c: %s line %d\n", XML_ErrorString(XML_GetErrorCode(parser)), XML_GetCurrentLineNumber(parser) );
		printf("\tparser.c: state = %s\n"OFF, stateNames[lastState]);
		return (-1);
	}
	else if (state == error){
		//DS parse error
		printf(ERR"\tparser.c: Parse error!!!\n");
		printf("\tparser.c: Wrong file format, line: %d\n", XML_GetCurrentLineNumber(parser));
		printf("\tparser.c: %s\n", errorMsg);
		printf("\tparser.c: State = %s\n"OFF, stateNames[lastState]);	
		return (-1);
	}
	else if (state != end){
		//eof b4 </config>
		printf(ERR"\tparser.c: Unexpected end of file!!!\n");
		printf("\tparser.c: State = %s\n"OFF, stateNames[lastState]);	
		return (-1);
	}

	// got here, ok!
	fclose(arq);

	//free the parser
	XML_ParserFree(parser);
 	return 1;
}

