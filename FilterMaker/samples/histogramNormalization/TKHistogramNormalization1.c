/*
 * MATLAB Compiler: 4.3 (R14SP3)
 * Date: Tue Jan 10 14:54:09 2006
 * Arguments: "-B" "macro_default" "-l" "TKHistogramNormalization1.m" 
 */

#include <stdio.h>
#define EXPORTING_TKHistogramNormalization1 1
#include "TKHistogramNormalization1.h"
#ifdef __cplusplus
extern "C" {
#endif

extern mclComponentData __MCC_TKHistogramNormalization1_component_data;

#ifdef __cplusplus
}
#endif


static HMCRINSTANCE _mcr_inst = NULL;


static int mclDefaultPrintHandler(const char *s)
{
    return fwrite(s, sizeof(char), strlen(s), stdout);
}

static int mclDefaultErrorHandler(const char *s)
{
    int written = 0, len = 0;
    len = strlen(s);
    written = fwrite(s, sizeof(char), len, stderr);
    if (len > 0 && s[ len-1 ] != '\n')
        written += fwrite("\n", sizeof(char), 1, stderr);
    return written;
}


/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_TKHistogramNormalization1_C_API 
#define LIB_TKHistogramNormalization1_C_API /* No special import/export declaration */
#endif

LIB_TKHistogramNormalization1_C_API 
bool TKHistogramNormalization1InitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler
)
{
    if (_mcr_inst != NULL)
        return true;
    if (!mclmcrInitialize())
        return false;
    if (!mclInitializeComponentInstance(&_mcr_inst,
                                        &__MCC_TKHistogramNormalization1_component_data,
                                        true, NoObjectType, LibTarget,
                                        error_handler, print_handler))
        return false;
    return true;
}

LIB_TKHistogramNormalization1_C_API 
bool TKHistogramNormalization1Initialize(void)
{
    return TKHistogramNormalization1InitializeWithHandlers(mclDefaultErrorHandler,
                                                           mclDefaultPrintHandler);
}

LIB_TKHistogramNormalization1_C_API 
void TKHistogramNormalization1Terminate(void)
{
    if (_mcr_inst != NULL)
        mclTerminateInstance(&_mcr_inst);
}


LIB_TKHistogramNormalization1_C_API 
void mlxTKHistogramNormalization1(int nlhs, mxArray *plhs[],
                                  int nrhs, mxArray *prhs[])
{
    mclFeval(_mcr_inst, "TKHistogramNormalization1", nlhs, plhs, nrhs, prhs);
}

LIB_TKHistogramNormalization1_C_API 
void mlfTKHistogramNormalization1(mxArray* inimage, mxArray* inimagemask
                                  , mxArray* datasetpath, mxArray* datasetname
                                  , mxArray* mag, mxArray* targetid)
{
    mclMlfFeval(_mcr_inst, "TKHistogramNormalization1", 0, 0, 6, inimage,
                inimagemask, datasetpath, datasetname, mag, targetid);
}
