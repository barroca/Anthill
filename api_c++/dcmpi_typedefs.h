/* NOTE:  machine generated file, do not edit! */
#ifndef DCMPI_TYPEDEFS_H
#define DCMPI_TYPEDEFS_H

#include <sstream>
typedef std::ostringstream ostr;
typedef std::istringstream istr;

typedef          signed char   int1;
typedef        unsigned char   uint1;
typedef         signed short   int2;
typedef       unsigned short   uint2;
typedef           signed int   int4;
typedef         unsigned int   uint4;
typedef     signed long long   int8;
typedef   unsigned long long   uint8;

typedef unsigned int           uint;
#define DCMPI_LITTLE_ENDIAN
#define DCMPI_SIZE_OF_PTR 4

#define DCMPI_HAVE_GETHOSTBYNAME_R
#define DCMPI_HAVE_RAND_R

#define MAX_RECV_BUF_SZ 60000000

/* NOTE:  this version of dcmpi was built with the compilers g++ and mpiCC */
#endif
