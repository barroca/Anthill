#ifndef __APICPP_H__
#define __APICPP_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern "C" { 
	#include <void.h>
}

#include "dcmpi_typedefs.h"
#include "DCBuffer.h"

#define KB_1          1024
#define KB_2          2048
#define KB_4          4096
#define KB_8          8192
#define KB_16        16384
#define KB_32        32768
#define KB_64        65536
#define KB_128      131072
#define KB_256      262144
#define KB_512      524288
#define MB_1       1048576
#define MB_2       2097152
#define MB_4       4194304
#define MB_8       8388608
#define MB_16     16777216 
#define MB_32     33554432
#define MB_64     67108864
#define MB_128   134217728
#define MB_256   268435456
#define MB_512   536870912
#define GB_1    1073741824

using namespace std;

class DCFilter 
{
public:
	std::map<std::string, std::string> user_params;
	std::map<std::string, DCBuffer> user_buffer_params;
	
public:
	DCFilter(){}
	virtual ~DCFilter(){}

	// inquire about my name
	std::string get_filter_name();
	int get_global_filter_thread_id();
	std::string get_distinguished_name();

	// iquire about input, output ports
	std::vector<std::string> get_in_port_names();
	std::vector<std::string> get_out_port_names();
	bool has_in_port(const std::string & portName);
	bool has_out_port(const std::string & portName);

	// filter authors should define the 3 methods
	virtual int init() { return 0; }
	virtual int process(){ return 0; }
	virtual int fini() { return 0; }

	// write methods
	void write(DCBuffer *buf, const std::string & portName);
	void write_nocopy(DCBuffer *buf, const std::string & portName);
	void write_broadcast(DCBuffer *buf, const std::string & portName);

	// read mehtods
	DCBuffer *read(const std::string & portName);
	DCBuffer *read_non_blocking(const std::string & portName);
	DCBuffer *read_until_upstream_exit(const std::string & portName);
	DCBuffer * readany(std::string *exceptionPorts, int numExc, string *port=NULL);
	bool any_upstream_running();

	// key/value pair lookup
 	bool has_param(const std::string & key);
	void set_param(std::string key, std::string val); 
	std::string get_param(const std::string & key);	
	int get_param_as_int(const std::string & key);

	bool has_param_buffer(const std::string & key);
	DCBuffer get_param_buffer(std::string key);
	void set_param_buffer(std::string key, const DCBuffer & val);

	// Methods that came from Anthill
	int get_my_rank();
	int close_out_port(std::string portName);
	int filter_exit(std::string mesg);
	int get_total_instances();
	int get_num_readers(string outPortName);
	int get_num_writers(string inPortName);
	int advertise_fault();
	bool last_filter();

	// Methods to compile ocvm application
	DCBuffer * get_init_filter_broadcast();
	void set_init_filter_broadcast(DCBuffer * buf) {
		init_filter_broadcast = new DCBuffer(*buf);
	}	

private:
	DCBuffer * init_filter_broadcast;
};

extern "C" int initCppAPI(char* cppFilterLib); ///< loaded by anthill to init the C++ API
extern "C" int initFilterCpp(void *work, int workSize);
extern "C" int processFilterCpp(void *work, int workSize);
extern "C" int finalizeFilterCpp();

// functions to help loading the C++ filter
void registerFilterObj(DCFilter* f);
typedef void (filterFactory_t)(); 

extern "C" void filterFactory();
#define provide1( c1 )  void filterFactory() { registerFilterObj(new c1()); }


//DCFilter * get_filter();
//#define provide1( c1 )  DCFilter * get_filter() {  return new c1(); }


class DCSerializable
{
public:
    virtual ~DCSerializable() {}
    virtual void serialize(DCBuffer * buf) const = 0;
    virtual void deSerialize(DCBuffer * buf)=0;
};

template<typename T>
inline std::string dcmpi_to_string(const T & value)
{
    ostr streamOut;
    streamOut << value;
    return streamOut.str();
}

inline double dcmpi_doubletime()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return (double) tp.tv_sec + (double) tp.tv_usec / 1000000.0;
}

inline std::vector<std::string> dcmpi_string_tokenize(
    const std::string & str, const std::string & delimiters=" \t\n")
{
    std::vector<std::string> tokens;
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    return tokens;
}

inline void dcmpi_string_trim_front(std::string & s)
{
    s.erase(0,s.find_first_not_of(" \t\n"));
}

inline void dcmpi_string_trim_rear(std::string & s)
{
    s.erase(s.find_last_not_of(" \t\n")+1);
}

inline void dcmpi_string_trim(std::string & s)
{
    dcmpi_string_trim_front(s);
    dcmpi_string_trim_rear(s);
}

inline void dcmpi_hostname_to_shortname(std::string & hostname)
{
    std::string::size_type dotpos = hostname.find(".");
    if (dotpos != std::string::npos) {
        hostname.erase(dotpos, hostname.size() - dotpos);    
    }
}

inline std::string dcmpi_get_hostname(bool force_short_name=false)
{
    std::string out;
    char localhost[1024];
    gethostname(localhost, sizeof(localhost));
    localhost[sizeof(localhost)-1] = 0;
    out = localhost;
    if (force_short_name) {
        dcmpi_hostname_to_shortname(out);
    }
    return out;
}

inline unsigned long long dcmpi_csnum(const std::string & num)
{
    std::string num_part(num);
    int lastIdx = num.size() - 1;
    char lastChar = num[lastIdx];
    unsigned long long multiplier = 1;
    unsigned long long kb_1 = (unsigned long long)1024;
    if ((lastChar == 'k') || (lastChar == 'K')) {
        multiplier = kb_1;
    }
    else if ((lastChar == 'm') || (lastChar == 'M')) {
        multiplier = kb_1*kb_1;
    }
    else if ((lastChar == 'g') || (lastChar == 'G')) {
        multiplier = kb_1*kb_1*kb_1;
    }
    else if ((lastChar == 't') || (lastChar == 'T')) {
        multiplier = kb_1*kb_1*kb_1*kb_1;
    }
    else if ((lastChar == 'p') || (lastChar == 'P')) {
        multiplier = kb_1*kb_1*kb_1*kb_1*kb_1;
    }

    if (multiplier != 1) {
        num_part.erase(lastIdx, 1);
    }
    unsigned long long n = strtoull(num_part.c_str(), NULL, 10);
    return n * multiplier;
}

inline bool dcmpi_string_replace(std::string & s,
                                 const std::string & before,
                                 const std::string & after,
                                 bool replace_all = true)
{
    bool out = false;
    std::string::size_type pos;
    while ((pos = s.find(before)) != std::string::npos) {
        out = true;
        s.replace(pos, before.size(), after);
        if (!replace_all) {
            break;
        }
    }
    return out;
}

inline bool dcmpi_file_exists(const std::string & filename)
{
    /* stat returns 0 if the file exists */
    struct stat stat_out;
    return (stat(filename.c_str(), &stat_out) == 0);
}

#endif
