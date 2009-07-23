#ifndef API_UTIL_H
#define API_UTIL_H

#include <stddef.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <errno.h>

inline void dcmpi_endian_swap(void * v, size_t len)
{   
    char *_bs,*_be;
    char _btemp;
    _bs = ((char*)v);
    _be = ((char*)v)+len-1; 
    while (_bs<_be) {
        _btemp = *_bs; *_bs = *_be; *_be = _btemp;
        _bs++; _be--;
    }
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

int DC_read_all(int fd, void *buf, size_t count, int * hitEOF = NULL);
int DC_write_all(int fd, const void * buf, size_t count);

void dcmpi_args_shift(int & argc, char ** argv, int frompos = 1);

#endif
