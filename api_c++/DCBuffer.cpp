#include "api_cpp.h"

#include "api_util.h"

using namespace std;

ostream &operator<<(ostream &os, DCBuffer &buf) {
    os << "DCBuffer{addr=" << hex << (long)(buf.pBuf) << dec
       << ",size=" << buf.getUsedSize() << ",max=" << buf.getMax()
       << ",contents=";
    int i;
    char byte[4];
    for (i = 0; i < 64 && i < buf.getUsedSize(); i++) {
        if (i) {
            os << " ";
        }
        sprintf(byte, "%02x", (int)((unsigned char)buf.pBuf[i]));
        os << "0x" << byte;
    }
    if (i == 64) {
        os << "...";
    }
    os << dec;
    return os;
}

// ****************************************************************************
//  DCBuffer
// ****************************************************************************

DCBuffer::DCBuffer(void)
    : pBuf(NULL), wMax(0), wSize(0), wSizeExtracted(0)
{ 
    commonInit();
}

DCBuffer::DCBuffer(int wMax_in)
{ 
    pBuf=NULL; 
    New(wMax_in); 
    commonInit();
}

void DCBuffer::commonInit()
{
    _isbigendian = dcmpi_is_big_endian();
    _appendReallocs = 0;
    fConsume = true;
}

DCBuffer::~DCBuffer() 
{ 
    Delete();
}

char *DCBuffer::New(int wMax_in)
{
    if (pBuf && wMax > 0 && fConsume) {
        delete [] pBuf; // assume array
    }
    if (wMax_in > 0) {
        pBuf = new char[wMax = wMax_in];
        wSize = wSizeExtracted = 0;
        fConsume = true;
    } else {
        pBuf = NULL;
        wMax = wSize = wSizeExtracted = 0;
    }
    commonInit();
    return pBuf;
}

void DCBuffer::Delete(void)
{
    if (pBuf && wMax > 0 && fConsume) {
        delete [] pBuf; // assume array
        pBuf=NULL;
    }
    wMax = wSize = wSizeExtracted = 0;
}

void DCBuffer::Set(const char *pBuf_in, int wMax_in, int wSize_in, bool handoff)
{
    if (pBuf) {
        if (wMax > 0) {
            if (fConsume) {
                delete [] pBuf; // assume array
            }
        }
    }
    if (wMax_in > 0) {
        pBuf = (char*)pBuf_in;
        wMax = wMax_in;
        wSize = wSize_in;
    } else {
        pBuf = NULL;
        wMax = wSize = wSizeExtracted = 0;
    }
    commonInit();
    fConsume = handoff;
    wSizeExtracted=0;
}

void DCBuffer::Empty(void) 
{ 
    wSize=0; 
    wSizeExtracted=0; 
}

int DCBuffer::setUsedSize(int wSize_in) 
{
    if (wSize_in <= wMax)
        wSize = wSize_in;
    else
        assert(0); // very very bad!
    return wSize;
}

int DCBuffer::Extract(std::string * str)
{
    *str = this->getPtrExtract();
    wSizeExtracted += str->size() + 1;
    return str->length() + 1;
}

// ****************************************************************************
//  Name: Append
//  Date: 08/12/1999 : mdb - Created.
// ****************************************************************************
/**
 * Appends the given buffer to the end of the current buffer.
 * @param pAdd - pointer of buffer to use
 * @param wSizeAdd - length of buffer to use
 * @return char * - ptr after added region
 */
char *DCBuffer::Append(const char *pAdd, int wSizeAdd, bool doConversion) 
{
    if (wSize + wSizeAdd > wMax) {
        /* if it's empty, allocate a reasonable default */ 
        if (!pBuf) {
            this->New((wSizeAdd < 8192) ? 8192 : wSizeAdd);
        }
        /* otherwise we need to realloc */
        else if (pBuf && (wMax > 0) && fConsume) {
            int newMax;

            _appendReallocs++;
            /* if realloc is happening a lot, then just start doubling the
             * buffer, to avoid performing a bunch of tiny reallocs. */
            if (_appendReallocs >= 4) {
                newMax = (wSize + wSizeAdd) * 2;
            }
            else {
                newMax = wSize +
                    ((wSizeAdd > 1024)
                                  ? wSizeAdd
                                  : 1024);
            }
            char *newpBuf = new char[newMax];
            memcpy(newpBuf, pBuf, (size_t)wSize);
            delete [] pBuf;
            pBuf = newpBuf;
            wMax = newMax;
        }
        /* otherwise a user-supplied (based on fFree being false) buffer
           was overflown */
        else {
            cerr << "DCBuffer::Append(): Cannot write " << wSizeAdd
                 << " bytes at offset " << wSize << " of a "
                 << wMax << " byte block.\n";
            return NULL;     // does not fit
        }
    }
    if (doConversion)
        SwapByteArray(1, wSizeAdd, pAdd, pBuf+wSize);
    else
        memcpy(pBuf+wSize, pAdd, wSizeAdd);
    wSize += wSizeAdd;
    return (pBuf+wSize);
}

// ****************************************************************************
//  Name: Extract
//  Date: 08/27/1999 : mdb - Created.
// ****************************************************************************
/**
 * Copies from the read ptr into the given ptr, and advances the
 * read ptr.
 * @param pBuf_in - ptr to buffer to write to (NULL = don't write)
 * @param wSize_in - length of buffer to write to
 * @return int - number of bytes extracted
 */
int DCBuffer::Extract(char *pBuf_in, int wSize_in, bool doConversion)
{
    if (wSizeExtracted + wSize_in > wSize) {
        return 0; /* couldn't extract anything.  This isn't
                   * necessarily a bug in the caller, especially if
                   * the preparer of the buffer is expecting the
                   * receiver to extract until it returns 0 */
    }
    if (pBuf_in) {
        if (doConversion)
            SwapByteArray(1, wSize_in, pBuf+wSizeExtracted, pBuf_in);
        else
            memcpy(pBuf_in, pBuf+wSizeExtracted, wSize_in);
    }
    wSizeExtracted += wSize_in;
    return wSize_in;
}

// ****************************************************************************
//  Name: sprintf
//  Date: 08/12/1999 : mdb - Created.
//        10/18/2000 : mdb - Now use snprintf to avoid buffer overwrites.
//        09/27/2001 : mdb - Adds an implicit \0 null byte if there is room.
//        05/11/2004 : rutt - prevented overflow for many sprintf calls
//  Note: Embedding a \0 in the format string will mess up the return value of
//        vsprintf, and also may not write the null byte.
// ****************************************************************************
/**
 * Does a std sprintf to the current buffer.
 * @param sbFormat - standard printf arg 
 * @param args - standard printf arg
 * @return int - number of characters printed
 */
int DCBuffer::sprintf(const char *sbFormat, ...)
{
    va_list ap;
    char sprintfBuf[4096];
    va_start(ap, sbFormat);
    int nBytes = vsnprintf(sprintfBuf, sizeof(sprintfBuf), sbFormat, ap);
    if (nBytes > 0) {
        this->Append(sprintfBuf, (int)nBytes);
    }
    va_end(ap);
    return nBytes;
}

// format string syntax:
// b:int1; B:uint1
// h:int2; H:uint2; i:int4; I:uint4;
// l:int8; L:uint8; f:float; d:double.
#ifdef GENCASE
#undef GENCASE
#endif
#define GENCASE(c, t, action) \
    case c: { t v = va_arg(ap, t); action(v); break; }
void DCBuffer::pack(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    while (*format) {
        switch(*format++) {
            GENCASE('i',int4, Append);
            GENCASE('s',char*, Append);
            GENCASE('d',double, Append);
            GENCASE('f',double, Append);
            case 'b':
            {
                int1 v = (int1)va_arg(ap, int);
                Append(v);
                break;
            }
            case 'B':
            {
                uint1 v = (uint1)va_arg(ap, int);
                Append(v);
                break;
            }
            case 'h':
            {
                int2 v = (int2)va_arg(ap, int);
                Append(v);
                break;
            }
            case 'H':
            {
                uint2 v = (uint2)va_arg(ap, int);
                Append(v);
                break;
            }
            GENCASE('I',uint4, Append);
            GENCASE('l',int8, Append);
            GENCASE('L',uint8, Append);
            default:
                assert(0);
        }
    }
    va_end(ap);
}

// format string syntax:
// b:int1; B:uint1
// h:int2; H:uint2; i:int4; I:uint4;
// l:int8; L:uint8; f:float; d:double.
#ifdef GENCASE2
#undef GENCASE2
#endif
#define GENCASE2(c, t) \
    case c: { t* ptr = (t*)va_arg(ap, char*); Extract(ptr); break; }
void DCBuffer::unpack(const char * format, ...)
{
    va_list ap;
    va_start(ap, format);
    while (*format) {
        switch(*format++) {
            GENCASE2('i',int4);
            case 's':
            {
                std::string * s = va_arg(ap, std::string*);
                this->Extract(s);
                break;
            }
            GENCASE2('d',double);
            GENCASE2('f',float);
            GENCASE2('b',int1);
            GENCASE2('B',uint1);
            GENCASE2('h',int2);
            GENCASE2('H',uint2);
            GENCASE2('I',uint4);
            GENCASE2('l',int8);
            GENCASE2('L',uint8);
            default:
                assert(0);
        }
    }
    va_end(ap);
}

void DCBuffer::saveToDCBuffer(DCBuffer * buf)
{
    uint4 count = this->getUsedSize();
    /* write endian byte of buffer creator, 1=is big endian, 0=is
     * little endian */
    uint1 big = isBigEndian()?1:0;
    buf->Append(big);
    buf->Append(count);
    buf->Append(this->getPtr(), count);
}
void DCBuffer::restoreFromDCBuffer(DCBuffer * buf)
{
    uint1 isBigEndian;
    bool big;
    char * rawbuf;
    uint4 count;

    buf->Extract(&isBigEndian);
    big = (isBigEndian==1)?true:false;
    buf->Extract(&count);
    rawbuf = new char[count];
    buf->Extract(rawbuf, count);
    this->Set(rawbuf, count, count, true);
    this->forceEndian(big); /* need to do this b/c this->Set sets the endianness
                               to auto */    
}

int DCBuffer::saveToDisk(char * filename)
{
    int fd;
    int rc = 0;

    if ((fd = open(filename, O_WRONLY|O_CREAT|O_TRUNC, 0666)) == -1) {
        rc = errno;
        goto Exit;
    }

    rc = this->saveToFd(fd);

Exit:
    if (fd != -1)
        close(fd);
    return rc;
}

int DCBuffer::restoreFromDisk(char * filename)
{
    int fd = -1;
    int rc;

    if ((fd = open(filename, O_RDONLY)) == -1) {
        rc = errno;
        goto Exit;
    }

    rc = restoreFromFd(fd);
Exit:
    if (fd != -1)
        close(fd);
    return rc;
}

int DCBuffer::saveToFd(int fd)
{
    uint4 count = this->getUsedSize();
    int rc;

    /* write endian byte of buffer creator, 1=is big endian, 0=is
     * little endian */
    uint1 big = isBigEndian()?1:0;
    if ((rc = DC_write_all(fd, &big, 1)) != 0) {
        goto Exit;
    }
    
    /* write uint4 little-endian length */
    if (dcmpi_is_big_endian()) {
        SwapByteArray_NC(&count, sizeof(count), 1);
    }
    if ((rc = DC_write_all(fd, &count, sizeof(count))) != 0) {
        goto Exit;
    }
    
    /* write buf */
    if ((rc = DC_write_all(fd, pBuf, wSize)) != 0) {
        goto Exit;
    }
Exit:
    return rc;
}

int DCBuffer::restoreFromFd(int fd)
{
    uint1 isBigEndian;
    bool big;
    char * buf = NULL;
    uint4 count;
    int rc;

    if ((rc = DC_read_all(fd, &isBigEndian, 1)) != 0) {
        goto Exit;
    }
    big = (isBigEndian==1)?true:false;

    if ((rc = DC_read_all(fd, &count, sizeof(count))) != 0) {
        goto Exit;
    }
    /* read uint4 little-endian length */
    if (dcmpi_is_big_endian()) {
        SwapByteArray(1, sizeof(count), &count);
    }
    
    buf = new char[count];

    if ((rc = DC_read_all(fd, buf, (size_t)count)) != 0) {
        goto Exit;
    }

    this->Set(buf, count, count, true);
    this->forceEndian(big); /* need to do this b/c this->Set sets the endianness
                               to auto */
Exit:
    return rc;
}

bool DCBuffer::doConv()
{
    return _isbigendian != dcmpi_is_big_endian();
}

