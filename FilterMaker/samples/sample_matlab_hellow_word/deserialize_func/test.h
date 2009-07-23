/*
 * MATLAB Compiler: 4.2 (R14SP2)
 * Date: Wed Oct  5 10:20:22 2005
 * Arguments: "-B" "macro_default" "-l" "-v" "test.m" 
 */

#ifndef __test_h
#define __test_h 1

#if defined(__cplusplus) && !defined(mclmcr_h) && defined(__linux__)
#  pragma implementation "mclmcr.h"
#endif
#include "mclmcr.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_test
#define PUBLIC_test_C_API __global
#else
#define PUBLIC_test_C_API /* No import statement needed. */
#endif

#define LIB_test_C_API PUBLIC_test_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_test
#define PUBLIC_test_C_API __declspec(dllexport)
#else
#define PUBLIC_test_C_API __declspec(dllimport)
#endif

#define LIB_test_C_API PUBLIC_test_C_API


#else

#define LIB_test_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_test_C_API 
#define LIB_test_C_API /* No special import/export declaration */
#endif

extern LIB_test_C_API 
bool testInitializeWithHandlers(mclOutputHandlerFcn error_handler,
                                mclOutputHandlerFcn print_handler);

extern LIB_test_C_API 
bool testInitialize(void);

extern LIB_test_C_API 
void testTerminate(void);


extern LIB_test_C_API 
void mlxTest(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);


extern LIB_test_C_API void mlfTest(mxArray* msg);

#ifdef __cplusplus
}
#endif

#endif
