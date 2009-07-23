#ifndef DC_BUFFER_H
#define DC_BUFFER_H

#include <iostream>
#include <string>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dcmpi_typedefs.h"

class DCBuffer
{
    friend std::ostream &operator<<(std::ostream &os, DCBuffer &buf);

 public:
    DCBuffer(void);
    DCBuffer(int wMax_in);
    DCBuffer(const DCBuffer & t)
    {
        pBuf = NULL;
        char * p = new char[t.getUsedSize()];
        memcpy(p, t.pBuf, t.getUsedSize());
        this->Set(p, t.getUsedSize(), t.getUsedSize(), true);
        this->forceEndian(t.isBigEndian());
    }

    DCBuffer & operator=(const DCBuffer & t)
    {
        if (&t != this) {
            pBuf = NULL;
            char * p = new char[t.getUsedSize()];
            memcpy(p, t.pBuf, t.getUsedSize());
            this->Set(p, t.getUsedSize(), t.getUsedSize(), true);
            this->forceEndian(t.isBigEndian());
        }
        return *this;
    }
    virtual ~DCBuffer(void);

    char *New(int wMax_in);
    void Delete(void); // delete inner-contained memory
    void Set(const char *pBuf_in, int wMax_in, int wSize_in, bool handoff);
    void Empty(void);

    void consume(void) {
        if (fConsume)
            delete this;
    }

    void setPtrExistingCharBuffer(char* buffer, int _wMax, int _wSize, int _wSizeExtracted){ 
	    pBuf = buffer;
	    wMax =_wMax;
	    wSize = _wSize;
	    wSizeExtracted = _wSizeExtracted;
    }	
   
    char *getPtr(void)                        { return pBuf; }               // to beginning
    char *getPtrFree(void)                    { return pBuf+wSize; }         // to free space
    char *getPtrExtract(void)                 { return pBuf+wSizeExtracted; }    

    int getUsedSize(void) const       { return wSize; }              // amount used at beg
    int setUsedSize(int wSize_in);
    void incrementUsedSize(int size) { this->setUsedSize(this->getUsedSize() + size); }

    int getFree(void) const           { return wMax-wSize; }         // free space at end
    int getMax(void) const            { return wMax; }               // total size of buf

    int getExtractAvailSize(void) const { return wSize-wSizeExtracted; } // amt left
    int getExtractedSize(void) const { return wSizeExtracted; } // amt already extracted
    void setExtractedSize(int extractedSize) { wSizeExtracted = extractedSize; return;}

    
    int resetExtract(int w=0)      { assert(w >= 0); return wSizeExtracted=w; }
    // don't extract, but increment the extract pointer
    void incrementExtractPointer(int size) {
        assert(size >= 0);
        wSizeExtracted += size;
        assert(wSizeExtracted <= wSize);
    }

    void forceEndian(bool is_big_endian)       { _isbigendian = is_big_endian; }
    bool isBigEndian() const { return _isbigendian; }

    // format string syntax:
    // b:int1; B:uint1
    // h:int2; H:uint2;
    // i:int4; I:uint4;
    // l:int8; L:uint8;
    // f:float; d:double.
    void pack(const char * format, ...);
    void unpack(const char * format, ...);

    /// if doConversion is true it assumes wSizeAdd a single item of size wSizeAdd
    char *Append(const char *pAdd, int wSizeAdd, bool doConversion=false);
    char *Append(int1 c)    { return Append((char*)&c, 1, false); }
    char *Append(uint1 c)   { return Append((char*)&c, 1, false); }
    char *Append(int2 w)    { return Append((char*)&w, 2, doConv()); }
    char *Append(uint2 w)   { return Append((char*)&w, 2, doConv()); }
    char *Append(int4 w)    { return Append((char*)&w, 4, doConv()); }
    char *Append(uint4 w)   { return Append((char*)&w, 4, doConv()); }
    char *Append(int8 w)    { return Append((char*)&w, 8, doConv()); }
    char *Append(uint8 w)   { return Append((char*)&w, 8, doConv()); }
    char *Append(float r)   { return Append((char*)&r, 4, doConv()); }
    char *Append(double r)  { return Append((char*)&r, 8, doConv()); }
    char * Append(const std::string & str) { return Append(str.c_str(), str.length() + 1); }

    /* the following Append methods are for adding arrays */
    char *Append(int1 * c,  int nelem)  { return _AppendArray((char*)c, 1, false,    nelem); }
    char *Append(uint1 * c, int nelem)  { return _AppendArray((char*)c, 1, false,    nelem); }
    char *Append(int2 * w,  int nelem)  { return _AppendArray((char*)w, 2, doConv(), nelem); }
    char *Append(uint2 * w, int nelem)  { return _AppendArray((char*)w, 2, doConv(), nelem); }
    char *Append(int4 * w,  int nelem)  { return _AppendArray((char*)w, 4, doConv(), nelem); }
    char *Append(uint4 * w, int nelem)  { return _AppendArray((char*)w, 4, doConv(), nelem); }
    char *Append(int8 * w,  int nelem)  { return _AppendArray((char*)w, 8, doConv(), nelem); }
    char *Append(uint8 * w, int nelem)  { return _AppendArray((char*)w, 8, doConv(), nelem); }
    char *Append(float * r, int nelem)  { return _AppendArray((char*)r, 4, doConv(), nelem); }
    char *Append(double * r,int nelem)  { return _AppendArray((char*)r, 8, doConv(), nelem); }

    /// if doConversion is true it assumes wSizeAdd a single item of size wSizeAdd
    int Extract(char *pBuf_in, int wSize_in, bool doConversion=false);
    int Extract(int1 *pch)     { return Extract((char*)pch, 1, false);    }
    int Extract(uint1 *pch)    { return Extract((char*)pch, 1, false);    }
    int Extract(int2 *pw)      { return Extract((char*)pw,  2, doConv()); }
    int Extract(uint2 *pw)     { return Extract((char*)pw,  2, doConv()); }
    int Extract(int4 *pw)      { return Extract((char*)pw,  4, doConv()); }
    int Extract(uint4 *pw)     { return Extract((char*)pw,  4, doConv()); }
    int Extract(int8 *pw)      { return Extract((char*)pw,  8, doConv()); }
    int Extract(uint8 *pw)     { return Extract((char*)pw,  8, doConv()); }
    int Extract(float *pr)     { return Extract((char*)pr,  4, doConv()); }
    int Extract(double *pr)    { return Extract((char*)pr,  8, doConv()); }
    int Extract(std::string * str);

    /* the following Extract methods are for extracting arrays */
    int Extract(int1 *pch,  int nelem)     { return _ExtractArray((char*)pch, 1, false, nelem);    }
    int Extract(uint1 *pch, int nelem)     { return _ExtractArray((char*)pch, 1, false, nelem);    }
    int Extract(int2 *pw,   int nelem)     { return _ExtractArray((char*)pw,  2, doConv(), nelem); }
    int Extract(uint2 *pw,  int nelem)     { return _ExtractArray((char*)pw,  2, doConv(), nelem); }
    int Extract(int4 *pw,   int nelem)     { return _ExtractArray((char*)pw,  4, doConv(), nelem); }
    int Extract(uint4 *pw,  int nelem)     { return _ExtractArray((char*)pw,  4, doConv(), nelem); }
    int Extract(int8 *pw,   int nelem)     { return _ExtractArray((char*)pw,  8, doConv(), nelem); }
    int Extract(uint8 *pw,  int nelem)     { return _ExtractArray((char*)pw,  8, doConv(), nelem); }
    int Extract(float *pr,  int nelem)     { return _ExtractArray((char*)pr,  4, doConv(), nelem); }
    int Extract(double *pr, int nelem)     { return _ExtractArray((char*)pr,  8, doConv(), nelem); }

    int sprintf(const char *sbFormat, ...);

    void saveToDCBuffer(DCBuffer * buf);
    void restoreFromDCBuffer(DCBuffer * buf);

    int saveToDisk(char * filename);      /* sends a DCBuffer to disk */
    int restoreFromDisk(char * filename); /* fills a DCBuffer from disk */

    int saveToFd(int fd);      /* sends a DCBuffer to a file descriptor */
    int restoreFromFd(int fd); /* fills a DCBuffer from a descriptor */

protected:
    char *pBuf;               // ptr to start of buffer
    bool fConsume;            // whether to assume ownership of pBuf
    int wMax;                 // max size of buffer
    int wSize;                // size of the "used" prefix in buffer
    int wSizeExtracted;
    bool _isbigendian;
    int _appendReallocs;

private:
    void commonInit();
    bool doConv();
    char * _AppendArray(const char *pAdd, int wSizeAdd,
                        bool doConversion, int nelem)
    {
        char * rv = NULL;
        int i = 0;
        for (i = 0; i < nelem; i++) {
            if (!(rv = Append(pAdd, wSizeAdd, doConversion))) {
                return NULL;
            }
            pAdd += wSizeAdd;
        }
        return rv;
    }
    int _ExtractArray(char *pBuf_in, int wSize_in,
                      bool doConversion, int nelem)
    {
        int bytesExtracted = 0;
        int i = 0;
        for (i = 0; i < nelem; i++) {
            bytesExtracted += Extract(pBuf_in, wSize_in, doConversion);
            pBuf_in += wSize_in;
        }
        return bytesExtracted;
    }
};

#if defined(DCMPI_BIG_ENDIAN)
#define dcmpi_is_big_endian() true
#elif defined(DCMPI_LITTLE_ENDIAN)
#define dcmpi_is_big_endian() false
#else
#error "one of DCMPI_BIG_ENDIAN, DCMPI_LITTLE_ENDIAN must be defined to build dcmpi"
#endif

class RecvBuf
{
public:
	RecvBuf() { createRecvBuf(MAX_RECV_BUF_SZ); }
	RecvBuf(int maxSize) { createRecvBuf(maxSize); }
	virtual ~RecvBuf(){}

	void * getBufPtr() { return recvBuf; }
	int getSize() {return recvBufSz; }

protected:
        void *recvBuf;
        int recvBufSz;

private:
	void createRecvBuf(int size) { recvBuf = malloc(size); recvBufSz = size;
}

};

#endif
