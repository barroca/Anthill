#ifndef DCMPI_UTIL_H
#define DCMPI_UTIL_H

/***************************************************************************
    $Id: dcmpi_util.h 32 2006-04-18 20:44:52Z ttavares $
    Copyright (C) 2004 by BMI Department, The Ohio State University.
    For license info, see LICENSE in root directory of source distribution.
 ***************************************************************************/

#include "dcmpi_clib.h"

#include "dcmpi.h"

#include "dcmpi_backtrace.h"

#include "DCMPIPacket.h"

#ifdef DCMPI_HAVE_PCRE
#include <pcre.h>
#endif

#ifdef DCMPI_HAVE_JAVA
#include <jni.h>
#endif

#ifdef DEBUG
#define checkrc(rc) if ((rc) != 0) { std::cerr << "ERROR: bad return code at " << __FILE__ << ":" << __LINE__ << std::endl << std::flush; assert(0); }
#else
#define checkrc(rc) if ((rc) != 0) { std::cerr << "ERROR: bad return code at " << __FILE__ << ":" << __LINE__ << std::endl << std::flush; exit(1); }
#endif

// string utils
inline char *StrDup(const char *sb) {
    if (!sb) return NULL;
    char *sbTmp = new char[strlen(sb) + 1];
    strcpy(sbTmp, sb);
    return sbTmp;
}

inline int Atoi(const std::string & s) {
    return atoi(s.c_str());
}

inline int8 Atoi8(std::string str)
{
    return strtoll(str.c_str(), NULL, 10);
}

// endian handling
inline void SwapByteArray_NC(void *p, int elemsz, int numelem)
{
    int i;
    for (i=0; i<numelem; ++i)
        dcmpi_endian_swap(((char *)p)+i*elemsz, elemsz);
}

inline void SwapByteArray(
    int nElem, int sElem, const void * pSrc, void * pDst = NULL)
{
    unsigned char *pBufSrc = (unsigned char *)pSrc;
    unsigned char *pBufDst = (unsigned char *)pDst;

    for (int i=0; i<nElem; i++) {
        for (int j=0; j<sElem; j++) {
            pBufDst[j] = pBufSrc[sElem - j - 1];
        }
        pBufSrc += sElem;
        pBufDst += sElem;
    }
}

// file routines
inline bool fileExists(const std::string & filename)
{
    /* stat returns 0 if the file exists */
    struct stat stat_out;
    return (stat(filename.c_str(), &stat_out) == 0);
}

inline bool fileIsDirectory(const std::string & filename)
{
    /* stat returns 0 if the file exists */
    struct stat stat_out;
    if (stat(filename.c_str(), &stat_out) != 0) {
        return false;
    }
    if (S_ISDIR(stat_out.st_mode)) {
        return true;
    }
    else {
        return false;
    }
}

inline off_t file_size(std::string filename)
{
    struct stat stat_out;
    if (stat(filename.c_str(), &stat_out) != 0) {
        std::cerr << "ERROR: file " << filename
                  << " does not exist"
                  << " at " << __FILE__ << ":" << __LINE__
                  << std::endl << std::flush;
        exit(1);
    }
    off_t out = stat_out.st_size;
    return out;
}

inline void file_create(std::string filename)
{
    FILE * toucher = fopen(filename.c_str(), "w");
    if (!toucher) {
        std::cerr << "ERROR:  cannot touch file " << filename
                  << " at " << __FILE__ << ":" << __LINE__
                  << std::endl << std::flush;
        exit(1);
    }
    fclose(toucher);
}

std::map<std::string, std::string> file_to_pairs(std::string filename);
std::map<std::string, std::string> file_to_pairs(FILE * f);
void pairs_to_file(std::map<std::string, std::string> pairs,
                   std::string filename);
inline std::string file_to_string(std::string filename)
{
    std::string out;
    off_t fs = file_size(filename);
    char * buf = new char[fs+1];
    buf[fs] = 0;
    FILE * f = fopen(filename.c_str(), "r");
    if (!f) {
        std::cerr << "ERROR: opening file " << filename
                  << " at " << __FILE__ << ":" << __LINE__
                  << std::endl << std::flush;
        exit(1);
    }
    if (fread(buf, fs, 1, f) < 1) {
        std::cerr << "ERROR: in fread()"
                  << " at " << __FILE__ << ":" << __LINE__
                  << std::endl << std::flush;
        exit(1);
    }
    out = buf;
    fclose(f);
    delete[] buf;
    return out;
}

/* The following methods are like UNIX read() and write(), except you don't
 * worry that only a portion of the number of bytes were written or read.
 * They both return a nonzero error code (from errno.h) if an error occured in
 * reading or writing.  For DC_read_all(), if an EOF is detected, a nonzero
 * error code will returned and in addition the hitEOF out-parameter is will
 * be set to true (nonzero) if it is passed as non-NULL. */
int DC_read_all(int fd, void *buf, size_t count, int * hitEOF = NULL);
int DC_write_all(int fd, const void * buf, size_t count);
inline int DC_write_all_string(int fd, const std::string & s)
{
    return DC_write_all(fd, s.c_str(), s.size());
}

// misc
inline std::string dcmpi_socket_read_line(int s) // returns stripped (no \n)
{
    char c;
    std::string str;
    while (1) {
        while (1) {
            if (DC_read_all(s, &c, 1) == 0) {
                break;
            }
            else {
                dcmpi_print_backtrace();
            }
        }
        if (c=='\n') {
            break;
        }
        str.push_back(c);
    }
    return str;
}

inline bool dcmpi_socket_data_ready_for_reading(int s)
{
    pollfd pfd;
    pfd.fd = s;
    pfd.events = POLLIN;
    pfd.revents = 0;
    if (poll(&pfd, 1, 0)) {
        return true;
    }
    else {
        return false;
    }
}

inline void dcmpi_socket_read_dcbuffer(int s, DCMPIPacket * p)
{
    DCBuffer * buf;
    int sz;
    int i;
    if (p->body_fragments == 1) {
        sz = p->body_len;
    }
    else {
        sz = (p->body_len * p->body_fragments-1) +
            p->last_fragment_len;
    }
    buf = new DCBuffer(sz);
    char * offset = buf->getPtr();
    for (i = 0; i < p->body_fragments; i++) {
        if (i && (i == (p->body_fragments-1))) {
            if (DC_read_all(s, offset, p->last_fragment_len) != 0) {
                std::cerr << "ERROR: DC_read_all"
                          << " at " << __FILE__ << ":" << __LINE__
                          << std::endl << std::flush;
                exit(1);
            }       
        }
        else {
            if (DC_read_all(s, offset, p->body_len) != 0) {
                std::cerr << "ERROR: DC_read_all"
                          << " at " << __FILE__ << ":" << __LINE__
                          << std::endl << std::flush;
                exit(1);
            }
        }
        offset += p->body_len;
    }
    buf->setUsedSize(sz);
    p->body = buf;
    p->defragment_body_according_to_mtu();
}

inline void dcmpi_socket_write_dcbuffer(int s, DCMPIPacket * p)
{
    int rc;
    int4 i;
    p->fragment_body_according_to_mtu();
    if (p->body_fragments == 1) {
        if ((rc = DC_write_all(s, p->body->getPtr(),
                               p->body_len)) != 0) {
            std::cerr << "ERROR: writing on socket "
                      << " at " << __FILE__ << ":" << __LINE__
                      << std::endl << std::flush;
            exit(1);
        }
    }
    else {
        char * offset = p->body->getPtr();
        for (i = 0; i < p->body_fragments - 1; i++) {
            if ((rc = DC_write_all(s, offset, p->body_len)) != 0) {
                std::cerr << "ERROR: writing on socket "
                          << " at " << __FILE__ << ":" << __LINE__
                          << std::endl << std::flush;
                exit(1);
            }
            offset += p->body_len;
        }
        if ((rc = DC_write_all(s, offset, p->last_fragment_len)) != 0) {
            std::cerr << "ERROR: writing on socket "
                      << " at " << __FILE__ << ":" << __LINE__
                      << std::endl << std::flush;
            exit(1);
        }
    }
}

void dcmpi_bash_select_emulator(
    int                              indentSpaces,
    const std::vector<std::string> & inChoices,
    const std::string &              description,
    const std::string &              exitDescription,
    bool                             allowMultipleChoices,
    bool                             allowDuplicates,
    std::vector<std::string> &       outChoices,
    std::vector<int> &               outIndexes);

inline std::string get_raw_run_command()
{
    FILE * f;
    std::string runf = dcmpi_get_home() + "/runcommand";
    char buf[512];
    if (!fileExists(runf) || ((f = fopen(runf.c_str(), "r"))==NULL)) {
        std::cerr << "ERROR: runcommand file (" << runf
                  << ") does not exist or is unreadable, aborting"
                  << std::endl << std::flush;
        std::cerr << "You should probably run the command 'dcmpiconfig'"
                  << " to correct this."
                  << std::endl << std::flush;
        exit(1);
    }
    while (1) {
        if (fgets(buf, sizeof(buf), f) == NULL) {
            std::cerr << "ERROR: no run command found in " << runf
                      << std::endl << std::flush;
            exit(1);
        }
        else if (buf[0] != '#') {
            break;
        }
    }
    buf[strlen(buf)-1]=0;
    fclose(f);
    std::string out = buf;
    return out;
}

inline void run_command_finalize(
    const std::string & executable,
    const std::vector<std::string> & hosts,
    std::map<std::string, std::string> & launch_params,
    std::string & run_command,
    std::vector<std::string> & mpi_run_temp_files)
{
    std::string executable_and_args =
        dcmpi_find_executable(executable) +
        " dcmpi_control_args_begin";
    std::map<std::string, std::string>::iterator it;
    for (it = launch_params.begin();
         it != launch_params.end();
         it++) {
        executable_and_args += " " + it->first + " " + it->second;
    }
    executable_and_args += " dcmpi_control_args_end";
    bool replaced_executable = false;
    while (1) {
        if (dcmpi_string_replace(run_command,
                              "%n", dcmpi_to_string(hosts.size()))) {
            ;
        }
        else if (dcmpi_string_replace(
                     run_command, "%e", executable_and_args)) {
            replaced_executable = true;
        }
        else if (run_command.find("%h") != std::string::npos) {
            std::string machinefile = dcmpi_get_temp_filename();
            mpi_run_temp_files.push_back(machinefile);
            FILE * f = fopen(machinefile.c_str(), "w");
            uint u;
            for (u = 0; u < hosts.size(); u++) {
                const char * s = hosts[u].c_str();
                if (fprintf(f, "%s\n", s) != (int)(strlen(s) + 1)) {
                    std::cerr << "ERROR: cannot write out machinefile"
                              << " at " << __FILE__ << ":" << __LINE__
                              << std::endl << std::flush;
                    exit(1);
                }
            }
            fclose(f);
            dcmpi_string_replace(run_command, "%h", machinefile);
        }
        else if (dcmpi_string_replace(run_command, "%F", hosts[0])) {
            ;
        }
        else if (run_command.find("%D") != std::string::npos) {
            if (getenv("DCMPI_DEBUG")) {
                dcmpi_string_replace(run_command, "%D", getenv("DCMPI_DEBUG"));
            }
            else {
                dcmpi_string_replace(run_command, "%D", "");
            }
        }
        else {
            break;
        }
    }
    if (!replaced_executable) { // put at end
        run_command += " ";
        run_command += executable_and_args;
    }
}

#ifdef DCMPI_HAVE_PCRE
inline bool dcmpi_pcre_matches(std::string regexp, std::string subject)
{
    pcre * re;
    const char *error;
    int erroffset;
    re = pcre_compile(
        regexp.c_str(),   /* the pattern */
        0,                /* default options */
        &error,           /* for error message */
        &erroffset,       /* for error offset */
        NULL);            /* use default character tables */
    if (!re) {
        std::cerr << "ERROR: compiling regexp " << regexp
                  << " at " << __FILE__ << ":" << __LINE__
                  << std::endl << std::flush;
        std::cerr << "ERROR: message from pcre was "
                  << error;
        exit(1);
    }
    int rc;
    int ovector[30];
    rc = pcre_exec(
        re,             /* result of pcre_compile() */
        NULL,           /* we didn't study the pattern */
        subject.c_str(),/* the subject string */
        subject.size(), /* the length of the subject string */
        0,              /* start at offset 0 in the subject */
        0,              /* default options */
        ovector,        /* vector of integers for substring information */
        30);            /* number of elements in the vector */
    pcre_free(re);
    if (rc > 0) {
        return true;
    }
    else {
        return false;
    }
}
#endif

#ifdef DCMPI_HAVE_JAVA
#if DCMPI_SIZE_OF_PTR == 4
#define DCMPI_JNI_CAST_FROM_POINTER_TO_JLONG(thing) (jlong)((int8)((uint4)(thing)))
#define DCMPI_JNI_CAST_FROM_JLONG_TO_POINTER(thing, type) (type)((uint4)(thing))
#elif DCMPI_SIZE_OF_PTR == 8
#define DCMPI_JNI_CAST_FROM_POINTER_TO_JLONG(thing) (jlong)(thing)
#define DCMPI_JNI_CAST_FROM_JLONG_TO_POINTER(thing, type) (type)(thing)
#else
#error "Unknown pointer size, this code won't work"
#endif

#define DCMPI_JNI_EXCEPTION_CHECK(env) if (env->ExceptionOccurred()) { env->ExceptionDescribe(); std::cerr << "ERROR: dcmpi bailing out after java exception at " << __FILE__ << ":" << __LINE__ << " on host " << dcmpi_get_hostname() << std::endl << std::flush; exit(1); }

class DCJavaFilter : public DCFilter
{
    std::string class_name;
    std::string lib_name;

    // will be global references
    JNIEnv * separate_thread_env;
    jclass cls;
    jobject obj;
public:
    DCJavaFilter(
        const std::string & classname,
        const std::string & libname) : class_name(classname), lib_name(libname)
    {}
    virtual int init();
    virtual int process();
    virtual int fini();
};
#endif


#endif /* #ifndef DCMPI_UTIL_H */
