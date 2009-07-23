/*
 * MATLAB Compiler: 4.3 (R14SP3)
 * Date: Tue Jan 10 14:39:24 2006
 * Arguments: "-B" "macro_default" "-l" "TKHistogramNormalization.m" 
 */

#ifndef __TKHistogramNormalization_h
#define __TKHistogramNormalization_h 1

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

#ifdef EXPORTING_TKHistogramNormalization
#define PUBLIC_TKHistogramNormalization_C_API __global
#else
#define PUBLIC_TKHistogramNormalization_C_API /* No import statement needed. */
#endif

#define LIB_TKHistogramNormalization_C_API PUBLIC_TKHistogramNormalization_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_TKHistogramNormalization
#define PUBLIC_TKHistogramNormalization_C_API __declspec(dllexport)
#else
#define PUBLIC_TKHistogramNormalization_C_API __declspec(dllimport)
#endif

#define LIB_TKHistogramNormalization_C_API PUBLIC_TKHistogramNormalization_C_API


#else

#define LIB_TKHistogramNormalization_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_TKHistogramNormalization_C_API 
#define LIB_TKHistogramNormalization_C_API /* No special import/export declaration */
#endif

extern LIB_TKHistogramNormalization_C_API 
bool TKHistogramNormalizationInitializeWithHandlers(mclOutputHandlerFcn error_handler,
                                                    mclOutputHandlerFcn print_handler);

extern LIB_TKHistogramNormalization_C_API 
bool TKHistogramNormalizationInitialize(void);

extern LIB_TKHistogramNormalization_C_API 
void TKHistogramNormalizationTerminate(void);


extern LIB_TKHistogramNormalization_C_API 
void mlxTKHistogramNormalization(int nlhs, mxArray *plhs[],
                                 int nrhs, mxArray *prhs[]);


extern LIB_TKHistogramNormalization_C_API void mlfTKHistogramNormalization(int nargout
                                                                           , mxArray** outimage
                                                                           , mxArray** outimagemask
                                                                           , mxArray* inimage
                                                                           , mxArray* inimagemask
                                                                           , mxArray* datasetpath
                                                                           , mxArray* datasetname
                                                                           , mxArray* mag
                                                                           , mxArray* targetid);

#ifdef __cplusplus
}
#endif

#endif
