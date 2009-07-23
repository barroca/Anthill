#include "api_cpp.h"

DCFilter *filter = NULL;
RecvBuf *recvBuf = NULL;

DCBuffer* void_to_dcbuffer(void *buf, int bufSz) {
	DCBuffer * dcBuf = new DCBuffer(bufSz);
	memcpy(dcBuf->getPtr(), buf, bufSz);
	dcBuf->setUsedSize(bufSz);
	return dcBuf;
}


void registerFilterObj(DCFilter* f) {
	filter = f;
}

int initCppAPI(char* cppFilterLib) {
	char *error = NULL;

	// try load lib in local directory, so we need 2 strigns,
	// the first with ./, em the second whithout
	char* libnameLocal = (char*)malloc(strlen(cppFilterLib) + 3);
	sprintf(libnameLocal, "./%s", cppFilterLib);

	// get the library handler
	void* filterLibHandler = NULL;
	if (((filterLibHandler = dlopen(libnameLocal, RTLD_NOW)) == NULL) &&
		((filterLibHandler = dlopen(cppFilterLib, RTLD_NOW)) == NULL )) {
			fprintf(stderr, "api_cpp.cpp: could not load filter %s library, %s\n", cppFilterLib, dlerror());
			dlclose(cppFilterLib);
			return -1;
	}
	free(libnameLocal);

	// load filter from dynamic library : filterFactory()
	// warnnig: here we only try to load simbols in C
	filterFactory_t* filterLibFactory = (filterFactory_t *)dlsym(filterLibHandler, "filterFactory");
	if ((error = dlerror()) != NULL)  {
		fprintf (stderr, "Erro no dlsym: %s\n", error);
		dlclose(filterLibHandler);
		return -1;
	}

	// call filterFactory() to register the filter object
	filterLibFactory();
	return 1;
}

int initFilterCpp(void *work, int workSize) {
/*	if(filter == NULL) {
		// Create a filter object
		filter = get_filter();
	}*/

	if(recvBuf == NULL) {
		// Create a recvBuf object to receive the data from Anthill
		recvBuf = new RecvBuf();
	}

	// Get Anthill work as DCBuffer
	DCBuffer* work_buffer = void_to_dcbuffer(work, workSize);
	filter->set_init_filter_broadcast(work_buffer);

	// Call filter init function
	filter->init();
	return 0;
}

int processFilterCpp(void *work, int workSize) {
	// Call filter process function
	filter->process();

	return 0;
}

int finalizeFilterCpp() {
	// Call filter finalize function
	filter->fini();

	return 0;
}

// Returns an unique id for every transparent copy (instance) of one filter
// Ranges from 0 to #filterinstances-1
int DCFilter::get_my_rank() {
	if (filter != NULL) {
		return dsGetMyRank();
	}

	return -1;
}

int DCFilter::get_total_instances() {
	return dsGetTotalInstances();
}

int DCFilter::get_num_readers(string outPortName) {
	OutputPortHandler outPort = dsGetOutputPortByName((char *) outPortName.c_str());

	return dsGetNumReaders(outPort);
}

int DCFilter::get_num_writers(string inPortName) {
	InputPortHandler inPort = dsGetInputPortByName((char *) inPortName.c_str());

	return dsGetNumWriters(inPort);
}

int DCFilter::advertise_fault() {
#ifdef BMI_FT
	return dsAdvertiseFault();
#else
  return 0;
#endif
}

bool DCFilter::last_filter() {
#ifdef BMI_FT
	if(dsLastFilter() == 1) {
		return true;
	} else {
		return false;
	}
#else
  return false;
#endif
}

// Writes packed buffer to other filter
void DCFilter::write(DCBuffer *buf, const std::string & portName) {
	// Gets Anthill output port handler
	OutputPortHandler outPort = dsGetOutputPortByName((char *) portName.c_str());

	if(outPort == -1) {
		string exitMsg = "write: There is no port "+portName+" in filter "+get_filter_name();
		dsExit((char *)exitMsg.c_str());
	}

	// Gets the packed buffer that will be written
	void *dsBuf = (void *) buf->getPtr();
	int dsBufSz = buf->getUsedSize();

	// Writes the packed buffer using Anthill
	dsWriteBuffer(outPort, dsBuf, dsBufSz);
}

// There is no way to do this in Anthill, so we call write.
void DCFilter::write_nocopy(DCBuffer *buf, const std::string & portName) {
	return write(buf, portName);
}

// TODO: We must create a function in Anthill that dynamicaly changes the
// communication type.
void DCFilter::write_broadcast(DCBuffer *buf, const std::string & portName) {
	return write(buf, portName);
}

// Reads and packs the data sent by other filter
// TODO: This function should not return NULL, if you are going to change it, 
//       remember that read_until_upstream_exit uses it.
DCBuffer * DCFilter::read(const std::string & portName) {
	// Gets Anthill input port handler
	InputPortHandler inPort = dsGetInputPortByName((char *) portName.c_str());

	if(inPort == -1) {
		string exitMsg = "read: There is no port "+portName+" in filter "+get_filter_name();
		dsExit((char *)exitMsg.c_str());
	}

	// Reads the data using Anthill
	int bufSz = dsReadBuffer(inPort, recvBuf->getBufPtr(), recvBuf->getSize());

	if (bufSz != EOW) {
		// Pack the data in a DCBuffer
		DCBuffer * dcBuf = new DCBuffer(bufSz);
		memcpy(dcBuf->getPtr(), recvBuf->getBufPtr(), bufSz);
		dcBuf->setUsedSize(bufSz);
		return dcBuf;
	}

	// Received an EOW. There isn't more data to be received.
	return NULL;
}

// Returns NULL or non-NULL, depending on if anything is available or not
// when it is called.
DCBuffer * DCFilter::read_non_blocking(const std::string & portName) {
	// Gets Anthill input port handler
	InputPortHandler inPort = dsGetInputPortByName((char *) portName.c_str());

	if(inPort == -1) {
		string exitMsg = "read_non_blocking: There is no port "+portName+" in filter "+get_filter_name();
		dsExit((char *)exitMsg.c_str());
	}

	// Reads the data using Anthill
	int bufSz = dsReadNonBlockingBuffer(inPort, recvBuf->getBufPtr(), recvBuf->getSize());

	if (bufSz == 0) {
		// There is no data to be received now.
		return NULL;
	}

	if (bufSz != EOW) {
		// Pack the data in a DCBuffer
		DCBuffer * dcBuf = new DCBuffer(bufSz);
		memcpy(dcBuf->getPtr(), recvBuf->getBufPtr(), bufSz);
		dcBuf->setUsedSize(bufSz);
		return dcBuf;
	}

	// Received an EOW. There isn't more data to be received.
	return NULL;
}

DCBuffer * DCFilter::readany(std::string *exceptionPorts, int numExc, std::string * port) {
	char *inPortName;
	int i;

	// Reads the data using Anthill
	char **c_exceptionPorts = (char **) malloc(numExc);
	for(i = 0; i < numExc; i++) {
		c_exceptionPorts[i] = (char *) (exceptionPorts[i]).c_str();
	}
	int bufSz = dsReadBufferAnyPort(&inPortName, c_exceptionPorts, numExc, recvBuf->getBufPtr(), recvBuf->getSize());

	if(port != NULL)
		*port = inPortName;

	if (bufSz != EOW) {
		// Pack the data in a DCBuffer
		DCBuffer * dcBuf = new DCBuffer(bufSz);
		memcpy(dcBuf->getPtr(), recvBuf->getBufPtr(), bufSz);
		dcBuf->setUsedSize(bufSz);
		return dcBuf;
	}

	// Received an EOW. There isn't more data to be received.
	return NULL;
}

// will read from port, or return NULL if all upstream writers to me have exited
DCBuffer *DCFilter::read_until_upstream_exit(const std::string & portName) {
	return DCFilter::read(portName);
}

std::string DCFilter::get_filter_name() {
	return dsGetFilterName();
}

int DCFilter::get_global_filter_thread_id() {
	return dsGetFilterId();
}

// TODO: fazer essa funcao
std::string DCFilter::get_distinguished_name() {
	return dsGetFilterName();
}

std::vector<std::string> DCFilter::get_in_port_names() {
	int numInputPorts = dsGetNumInputPorts();
	char **inputPorts = dsGetInputPortNames();
	std::vector<std::string> out;
	int i;

	for(i = 0; i < numInputPorts; i++) {
		out.push_back(inputPorts[i]);
	}

#ifdef DEBUG
	cout << "Inputports:";
	for(i=0;i < (signed)out.size(); i++)	{
		std::string strd = out.at(i);
		cout << " " << strd.c_str();
	}
	cout << "\n";
#endif

	return out;
}

std::vector<std::string> DCFilter::get_out_port_names() {
	int numOutputPorts = dsGetNumOutputPorts();
	char **outputPorts = dsGetOutputPortNames();
	std::vector<std::string> out;
	int i;

	for(i = 0; i < numOutputPorts; i++) {
		out.push_back(outputPorts[i]);
	}

#ifdef DEBUG
	cout << "Outputports:";
	for(i=0; i < (signed)out.size(); i++)	{
		std::string strd = out.at(i);
		cout << " " << strd.c_str();
	}
	cout << "\n";
#endif

	return out;
}

bool DCFilter::has_in_port(const std::string & portName) {
	int numInputPorts = dsGetNumInputPorts();
	char **inputPorts = dsGetInputPortNames();
	int i;

	for(i = 0; i < numInputPorts; i++) {
		if(!strcmp(inputPorts[i], portName.c_str()))
			return true;
	}

	return false;
}

bool DCFilter::has_out_port(const std::string & portName) {
	int numOutputPorts = dsGetNumOutputPorts();
	char **outputPorts = dsGetOutputPortNames();
	int i;

	for(i = 0; i < numOutputPorts; i++) {
		if(!strcmp(outputPorts[i], portName.c_str()))
			return true;
	}

	return false;
}

bool DCFilter::any_upstream_running() {
	return (dsGetNumUpStreamsRunning() > 0);
}

void DCFilter::set_param(std::string key, std::string val) {
	user_params[key] = val;
}

std::string DCFilter::get_param(const std::string & key)	{
	if (user_params.count(key) == 0) {
		std::cerr << "ERROR: invalid get_param call on key '" << key
			<< "' in filter " << get_filter_name() << " rank " << get_my_rank()
			<< std::endl << std::flush;
		exit(1);
	}
	return user_params[key];
}

int DCFilter::get_param_as_int(const std::string & key) {
	std::string s = get_param(key);
	return atoi(s.c_str());
}

bool DCFilter::has_param(const std::string & key)	{
	return user_params.count(key) != 0;
}

bool DCFilter::has_param_buffer(const std::string & key) {
	return user_buffer_params.count(key) != 0;
}

void DCFilter::set_param_buffer(std::string key, const DCBuffer & val) {
	user_buffer_params[key] = DCBuffer(val);
}

DCBuffer DCFilter::get_param_buffer(std::string key)
{
    if (user_buffer_params.count(key) == 0) {
        std::cerr << "ERROR: invalid get_param_buffer call on key '" << key
                  << "' in filter " << get_filter_name() << " rank "
                  << get_my_rank() << std::endl << std::flush;
        exit(1);
    }
    return user_buffer_params[key];
}

int DCFilter::close_out_port(std::string portName) {
	// Gets Anthill output port handler
	OutputPortHandler outPort = dsGetOutputPortByName((char *) portName.c_str());

	if(outPort == -1) {
		string exitMsg = "close_out_port: There is no port "+portName+" in filter "+get_filter_name();
		dsExit((char *)exitMsg.c_str());
	}

	return dsCloseOutputPort(outPort);
}

int DCFilter::filter_exit(std::string mesg) {
	return dsExit((char *) mesg.c_str());
}

DCBuffer * DCFilter::get_init_filter_broadcast()
{
    return init_filter_broadcast;
}

