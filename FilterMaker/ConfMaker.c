#include "ConfMaker.h"

/*
* 13/10/2006 ITAMAR changes in method generateLayout to permit that one 'from' has various 'to'
*/

typedef struct{
	char existingResources [MAXFILTERS][MAX_RNAME_LENGTH+1];
	int numInsertedResources;
} Resources;

Resources *createResources(){
	Resources *resource =  (Resources *) malloc(sizeof(Resources));	
	resource->numInsertedResources = 0;
	return resource;
}
int addResource(Resources *resource, char *resourceName){
	int i;
	//check resources
	if (resource->numInsertedResources >= MAXFILTERS){
		printf("Hosts.c: warning, reached maximum number of resources(%d)\n", MAXRESOURCES);
		return -1;
	}
	// check if the resource have been inserted
	for(i = 0; i < resource->numInsertedResources; i++){
		if(!strcmp(resourceName, resource->existingResources[i]))break;		
	}

	if(i == resource->numInsertedResources){
		//add the resource
		strncpy(resource->existingResources[i], resourceName, MAX_RNAME_LENGTH);
		resource->numInsertedResources++;
#ifdef DEBUG
		printf("FILE %s:%d - AddResource = %s numInserdedResources = %d\n", __FILE__, __LINE__, resourceName, resource->numInsertedResources);
#endif

	}
	//return the resource index
	return resource->numInsertedResources;

}

int generateHostDec(FILE *confFile, FILE *cacheConfFile, Layout *layout){
	int i, j;
	fprintf(confFile, "<hostdec>\n");
	fprintf(cacheConfFile, "<hostdec>\n");

	for(i = 0; i < layout->hostsStruct->numHosts; i++){
		fprintf(confFile, "\t<host name=\"%s\">\n", layout->hostsStruct->hosts[i].name);
		fprintf(cacheConfFile, "\t<host name=\"%s\">\n", layout->hostsStruct->hosts[i].name);

			for(j = 0; j < layout->hostsStruct->hosts[i].numResources; j++){
				fprintf(confFile, "\t\t<resource name=\"%s\"/>\n", layout->hostsStruct->hosts[i].resources[j]);
				fprintf(cacheConfFile, "\t\t<resource name=\"%s\"/>\n", layout->hostsStruct->hosts[i].resources[j]);

			}
		fprintf(confFile, "\t</host>\n");
		fprintf(cacheConfFile, "\t</host>\n");
	}	

	fprintf(confFile, "</hostdec>\n");
	fprintf(cacheConfFile, "</hostdec>\n");

	return 1;
}

int generatePlacement(FILE *confFile, FILE *cacheConfFile, Layout *layout){
	int i, j;
	Resources *resources = createResources();
	

	fprintf(confFile, "<placement>\n");
	// Generate the placement for each filter
	for(i = 0; i < layout->numFilters; i++)	{
		fprintf(confFile, "\t<filter name=\"%s\" libname=\"%s\" instances=\"%d\">\n", layout->filters[i]->name, layout->filters[i]->libname, layout->filters[i]->filterPlacement.numInstances);
		for(j = 0; j < layout->filters[i]->filterPlacement.numInstances; j++){
			if(layout->filters[i]->filterPlacement.resources[j] != NULL){
				fprintf(confFile, "\t\t<instance demands=\"%s\"/>\n", layout->filters[i]->filterPlacement.resources[j]);
				addResource(resources, layout->filters[i]->filterPlacement.resources[j]);
			}else{
				break;	
			}
		}
		fprintf(confFile, "\t</filter>\n");
	}

	fprintf(confFile, "\t<!-- Filters Cache and Pool -->\n");

	// add filterCache in the placement
	if(resources->numInsertedResources > 0){
		fprintf(confFile, "\t<filter name=\"f-cache\" libname=\"f-cache.so\" instances=\"%d\" attached=\"yes\">\n", resources->numInsertedResources);
	}else{
		printf("Warnnig: You should use demands for all filter instances that need a f-cache\n");
		fprintf(confFile, "\t<filter name=\"f-cache\" libname=\"f-cache.so\" instances=\"1\" attached=\"yes\">\n");
	
	}
	for(i = 0; i < resources->numInsertedResources; i++){
		fprintf(confFile, "\t\t<instance demands=\"%s\"/>\n", resources->existingResources[i]);
	}
	fprintf(confFile, "\t</filter>\n");

	// adding filterPool to placement
	fprintf(confFile, "\t<filter name=\"f-pool\" libname=\"f-pool.so\" instances=\"1\" attached=\"yes\"/>\n");
	fprintf(confFile, "</placement>\n");



	///// Generating cache and pool conf
	fprintf(cacheConfFile, "<placement>\n");
	if(resources->numInsertedResources > 0){
		fprintf(cacheConfFile, "\t<filter name=\"f-cache\" libname=\"f-cache.so\" instances=\"%d\" attach=\"yes\">\n", resources->numInsertedResources);
	}else{
		printf("Warnnig: You should use demands for all filter instances that need a f-cache\n");
		fprintf(cacheConfFile, "\t<filter name=\"f-cache\" libname=\"f-cache.so\" instances=\"1\" attach=\"yes\">\n");
	
	}
	for(i = 0; i < resources->numInsertedResources; i++){
		fprintf(cacheConfFile, "\t\t<instance demands=\"%s\"/>\n", resources->existingResources[i]);
	}
	fprintf(cacheConfFile, "\t</filter>\n");


	// adding filterPool to placement
	fprintf(cacheConfFile, "\t<filter name=\"f-pool\" libname=\"f-pool.so\" instances=\"1\" attach=\"yes\"/>\n");

	fprintf(cacheConfFile, "</placement>\n");


	return 1;
}

//ITAMAR changes to adequate de xml, to permit that one 'from', has various 'to'
int generateLayout(FILE *confFile, FILE *cacheConfFile,Layout *layout){
	int i, j;
	fprintf(confFile, "<layout>\n");
	fprintf(cacheConfFile, "<layout>\n");

	fprintf(confFile, "\t<!-- Application Streams -->\n");
	for(i = 0; i < layout->numStreams; i++){
		fprintf(confFile, "\t<stream>\n");
		
		fprintf(confFile, "\t\t<from filter=\"%s\" port=\"%s\" ", 
		layout->streams[i]->fromFilter->name, layout->streams[i]->fromPortName);
		fprintf(confFile, "/>\n");
		for(j = 0; j < layout->streams[i]->numToSend; j++){
			fprintf(confFile, "\t\t<to filter=\"%s\" port=\"%s\" policy=\"%s\" ", layout->streams[i]->toFilter[j]->name, layout->streams[i]->toPortName[j], layout->streams[i]->writePolicyName[j]);
			if(!strcmp("ls", layout->streams[i]->writePolicyName[j])){
				fprintf(confFile, "policylib=\"%s\"", layout->streams[i]->lsLibName[j]);
			}
			fprintf(confFile, "/>\n");
		}
		
		fprintf(confFile, "\t</stream>\n");
	}
	fprintf(confFile, "\n\t<!-- Cache to Application Streams -->\n");

	for(i = 0; i < layout->numFilters; i++){
		char *auxPortName = (char *) malloc(MAX_FNAME_LENGTH + 8); // filterName+ToCache
		strcpy(auxPortName,layout->filters[i]->name);
		strcat(auxPortName,"ToCache");

		//stream from cache to filter
		fprintf(confFile, "\t<stream>\n");
		fprintf(confFile, "\t\t<from filter=\"%s\" port=\"%s\" policy=\"ls\" policylib=\"%s\"/>\n", layout->filters[i]->name, auxPortName, POLICYLIBFILTERCACHE);
		fprintf(confFile, "\t\t<to filter=\"f-cache\" port=\"%s\"/>\n", auxPortName);
		fprintf(confFile, "\t</stream>\n");

		strcpy(auxPortName, "cacheTo");
		strcat(auxPortName, layout->filters[i]->name);

		//stream from filter to cache
		fprintf(confFile, "\t<stream>\n");
		fprintf(confFile, "\t\t<from filter=\"f-cache\" port=\"%s\" policy=\"ls\" policylib=\"%s\"/>\n", auxPortName, POLICYLIBCACHEFILTER);
		fprintf(confFile, "\t\t<to filter=\"%s\" port=\"%s\"/>\n", layout->filters[i]->name, auxPortName);
		fprintf(confFile, "\t</stream>\n");

		free(auxPortName);
	}

	fprintf(confFile, "\n\t<!-- Cache to Pool Streams -->\n");

		
	fprintf(confFile, "\t<stream>\n\t\t<from filter=\"f-cache\" port=\"cacheToPool\"/>\n\t\t<to filter=\"f-pool\" port=\"cacheToPool\"  />\n\t</stream>\n\t<stream>\n\t\t<from filter=\"f-pool\" port=\"poolToCache\"  policy=\"ls\" policylib=\"%s\" />\n\t\t<to filter=\"f-cache\" port=\"poolToCache\"  />\n\t</stream>\n", POLICYLIBPOOLTOCACHE);

	fprintf(confFile, "</layout>\n");
	fprintf(cacheConfFile, "</layout>\n");

	return 1;	
}
/* APAGAR
int generateLayout(FILE *confFile, FILE *cacheConfFile,Layout *layout){
	int i, j;
	fprintf(confFile, "<layout>\n");
	fprintf(cacheConfFile, "<layout>\n");

	fprintf(confFile, "\t<!-- Application Streams -->\n");
	for(i = 0; i < layout->numStreams; i++){
		fprintf(confFile, "\t<stream>\n");
		
		fprintf(confFile, "\t\t<from filter=\"%s\" port=\"%s\" policy=\"%s\" ", layout->streams[i]->fromFilter->name, layout->streams[i]->fromPortName, layout->streams[i]->writePolicyName);
		if(!strcmp("ls", layout->streams[i]->writePolicyName)){
			fprintf(confFile, "policylib=\"%s\"", layout->streams[i]->lsLibName);
		}
		fprintf(confFile, "/>\n");
		
		fprintf(confFile, "\t\t<to filter=\"%s\" port=\"%s\"/>\n", layout->streams[i]->toFilter->name, layout->streams[i]->toPortName);
		fprintf(confFile, "\t</stream>\n");
	}
	fprintf(confFile, "\n\t<!-- Cache to Application Streams -->\n");

	for(i = 0; i < layout->numFilters; i++){
		char *auxPortName = (char *) malloc(MAX_FNAME_LENGTH + 8); // filterName+ToCache
		strcpy(auxPortName,layout->filters[i]->name);
		strcat(auxPortName,"ToCache");

		//stream from cache to filter
		fprintf(confFile, "\t<stream>\n");
		fprintf(confFile, "\t\t<from filter=\"%s\" port=\"%s\" policy=\"ls\" policylib=\"%s\"/>\n", layout->filters[i]->name, auxPortName, POLICYLIBFILTERCACHE);
		fprintf(confFile, "\t\t<to filter=\"f-cache\" port=\"%s\"/>\n", auxPortName);
		fprintf(confFile, "\t</stream>\n");

		strcpy(auxPortName, "cacheTo");
		strcat(auxPortName, layout->filters[i]->name);

		//stream from filter to cache
		fprintf(confFile, "\t<stream>\n");
		fprintf(confFile, "\t\t<from filter=\"f-cache\" port=\"%s\" policy=\"ls\" policylib=\"%s\"/>\n", auxPortName, POLICYLIBCACHEFILTER);
		fprintf(confFile, "\t\t<to filter=\"%s\" port=\"%s\"/>\n", layout->filters[i]->name, auxPortName);
		fprintf(confFile, "\t</stream>\n");

		free(auxPortName);
	}

	fprintf(confFile, "\n\t<!-- Cache to Pool Streams -->\n");

		
	fprintf(confFile, "\t<stream>\n\t\t<from filter=\"f-cache\" port=\"cacheToPool\"/>\n\t\t<to filter=\"f-pool\" port=\"cacheToPool\"  />\n\t</stream>\n\t<stream>\n\t\t<from filter=\"f-pool\" port=\"poolToCache\"  policy=\"ls\" policylib=\"%s\" />\n\t\t<to filter=\"f-cache\" port=\"poolToCache\"  />\n\t</stream>\n", POLICYLIBPOOLTOCACHE);

	fprintf(confFile, "</layout>\n");
	fprintf(cacheConfFile, "</layout>\n");

	return 1;	
}
*/
int generateConfXML(char *filterDir, Layout *layout){
	// open dir .....
	char *cacheConfFileName = (char *) malloc(strlen(filterDir) + 10);
	char *confFileName = (char *) malloc(strlen(filterDir) + 10); // size(dir) + conf.xml
	strcpy(confFileName, filterDir);
	strcat(confFileName, "conf1.xml");

	strcpy(cacheConfFileName, filterDir);
	strcat(cacheConfFileName, "conf2.xml");

	FILE *confFile = fopen(confFileName, "w");
	if(!confFile) {
		printf("Error (%s:%d). Could not open file %s.\n", __FILE__, __LINE__, confFileName);
		exit(1);
	}

	FILE *cacheConfFile = fopen(cacheConfFileName, "w");
	if(!confFile) {
		printf("Error (%s:%d). Could not open file %s.\n", __FILE__, __LINE__, cacheConfFileName);
		exit(1);
	}

	fprintf(confFile, "<config>\n");
	fprintf(cacheConfFile, "<config>\n");
	
	generateHostDec(confFile, cacheConfFile, layout);

	generatePlacement(confFile, cacheConfFile, layout);

	generateLayout(confFile, cacheConfFile, layout);

	
	fprintf(confFile, "</config>\n");
	fprintf(cacheConfFile, "</config>\n");


	fclose(confFile);
	fclose(cacheConfFile);
	
	free(confFileName);
	free(cacheConfFileName);

	return 1;
}
