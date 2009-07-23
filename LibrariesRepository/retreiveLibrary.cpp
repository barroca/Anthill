#include <stdio.h>

#include "MakoConnect.h"

using namespace std;

int main(int argc, char **argv) {
	string collectionName, realCollectionName, hostName, XQuery, libName, hostsFilePath;
	char *chHostName;
	bool status;
	FILE *mobiusHosts;
	vector<string> hosts;

	vector<string> docs;
	if(argc != 4){
		printf("./retrieveLibrary collectionName libName hosts.txt\n");
		exit(1);
	}

	collectionName = argv[1];
	libName = argv[2];
	hostsFilePath = argv[3];

	
	MakoConnect *mako = new MakoConnect();
	
	hostName = "dc06";

	// open file with the Mobius hosts
	mobiusHosts = fopen(hostsFilePath.c_str(), "r");
	if(mobiusHosts == NULL){
		printf("%s:%d Could not open file %s\n", __FILE__, __LINE__, hostsFilePath.c_str());
		exit(1);
	}
#ifdef DEBUG
	printf("Reading HostsFile\n");
#endif	
	while(fscanf(mobiusHosts, "%as", &chHostName) != EOF) {
		hosts.push_back(chHostName);
#ifdef DEBUG
		printf("host = %s\n", chHostName);
#endif	
		free(chHostName);
	}

	// add "/" to root collection
	realCollectionName = "/" + collectionName;

	// create Mobius Query
	XQuery = "/matlab:MatLabLibrary[@name='"+libName+"']";

#ifdef DEBUG
	printf("Collection = %s\n", realCollectionName.c_str());
	printf("XQuery = %s\n", XQuery.c_str());
	printf("Hosts File = %s\n", hostsFilePath.c_str());
#endif

	status = mako->xpathRequestFromAllMobius( hosts, 3940 , realCollectionName, XQuery, &docs);

	if(!status) {
		printf("ERROR\n");
		exit(1);
	}
	if(docs.size() < 2){
		printf("Could not find library %s in collection %s\n", libName.c_str(), realCollectionName.c_str());	
		exit(1);
	}
	if(docs.size() > 2){
		printf("Warnning: We have find %d libraries with the name %s. We are going to use the first that is located in machine %s\n", (docs.size()/2), libName.c_str(), docs[1].c_str());	
	}
	
#ifdef DEBUG	
	for(unsigned int i = 0; i < docs.size(); i++) {
		cout << docs[i] << "\n";
	}
#endif	
	mako->retrieveXMLWithLibrary (docs[1], 3940, realCollectionName, docs[0], libName);


}
