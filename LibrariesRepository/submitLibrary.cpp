#include <stdio.h>

#include "MakoConnect.h"

using namespace std;

void myCreateCollection (MakoConnect *mako, string newCollectionName, string hostName) {
	vector<string> collections;
	bool createStatus;
	string resultCollectionName;

	// Create Collection in the mako
	createStatus = mako->createCollection (hostName, 3940, "", newCollectionName);
	if (!createStatus) {
		printf("ERROR creating collection %s. XML was not find.\n", newCollectionName.c_str());
		exit(1);
	}
		
#ifdef DEBUG
	mako->getCollections(hostName, 3940, "/", &collections);
		
	// Verify if the collection was realy created
	vector<string>::iterator itCollections;
	int success = 0;
	for(itCollections = collections.begin(); itCollections != collections.end(); itCollections++) {
		string oldCollection = *(itCollections);
		// Warning: the collections will be created in /
		if(oldCollection == "/"+newCollectionName) {
			success = 1;
			printf("Collection %s was created succesfully\n", newCollectionName.c_str());
			break;
		}
	}
	if(!success) {
		printf("Collection %s was not created succesfully\n", newCollectionName.c_str());
		exit(1);
	}
#endif
}

vector<string> getHostsName (FILE *hostsFile) {
	vector<string> hostsName;
	char *chHostName;
	
	while (fscanf(hostsFile, "%as", &chHostName) != EOF) {
		hostsName.push_back(chHostName);
	}

	return hostsName;
}

void mySubmitSchemaRequest (MakoConnect *mako, string newCollectionName, string schemaName, string hostName) {
	vector<string> schemas;
	bool submitStatus;

	submitStatus =	mako->submitSchemaRequest (hostName, 3940, newCollectionName, schemaName);
	if (!submitStatus) {
		printf("ERROR submiting schema %s. XML was not find.\n", schemaName.c_str());
		exit(1);
	}
#ifdef DEBUG
	printf("schema %s. submited succesfully\n", schemaName.c_str());
#endif	
}

int main(int argc, char **argv) {
	TiXmlDocument tixml;
	string newCollectionName, realCollectionName;
	string schemaName, resultSchemaName;
	string hostName;
	string xmlFilePath, binaryPath, XMLFile,includePath;
	

	if(argc != 7) {
		printf("usage: %s <collection name> <schema name> <mobius host> <xml file> <lib Path> <include Path>\n", argv[0]);
		exit(1);
	}
	
	MakoConnect *mako = new MakoConnect();
	
	newCollectionName = argv[1];

	schemaName = argv[2];
	hostName = argv[3];

	xmlFilePath =  argv[4];
	binaryPath = argv[5];
	includePath = argv[6];

	// Create Collection in the mako
	myCreateCollection(mako, newCollectionName, hostName);

	// Warning: the collections will be created in /
	realCollectionName = "/"+newCollectionName;

	// Submit the schema in the mako
	mySubmitSchemaRequest(mako, realCollectionName, schemaName, hostName);

	if(!mako->readFile( xmlFilePath, &XMLFile ) ){
		printf("%s:%d Could not open File %s", __FILE__, __LINE__, xmlFilePath.c_str());
		exit(1);
	}

//////////////// reading library and Include


	FILE *f = fopen (binaryPath.c_str(), "rb");
	if( !f ){
		printf("Could not open file: %s\n", binaryPath.c_str());	
		exit(1);
	}	
	fseek( f, 0, SEEK_END );
	unsigned int libLen = ftell( f );
	fseek( f, 0, SEEK_SET );
	void *lib = malloc (libLen);
	memset (lib, 0, libLen);
	fread (lib, libLen, 1, f );

	fclose (f);

	f = fopen (includePath.c_str(), "rb");
	if( !f ){
		printf("Could not open file: %s\n", includePath.c_str());	
		exit(1);
	}	

	fseek( f, 0, SEEK_END );
	unsigned int includeLen = ftell( f );
	fseek( f, 0, SEEK_SET );
	void *include = malloc (includeLen);
	memset (include, 0, includeLen);
	fread (include, includeLen, 1, f );

	fclose (f);


//////////////end


	
mako->submitXMLWithBinaryRequestFromMemoryLibrary(hostName, 3940, realCollectionName, XMLFile, (char*) lib, libLen, (char*)include, includeLen);


//	mako->submitXMLWithBinaryRequest(hostName, 3940, realCollectionName, XMLFile , binaryPath);
// TEST
//	mako->retrieveXMLWithBinaryRequest( "dc06", 3940,"/teste", "460000000001" , "mobius.so");	
}
