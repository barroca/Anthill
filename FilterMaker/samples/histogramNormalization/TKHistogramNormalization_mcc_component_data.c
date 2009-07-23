/*
 * MATLAB Compiler: 4.3 (R14SP3)
 * Date: Tue Jan 10 14:39:24 2006
 * Arguments: "-B" "macro_default" "-l" "TKHistogramNormalization.m" 
 */

#include "mclmcr.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_TKHistogramNormalization_session_key[] = {
        '7', '9', 'A', '2', '3', 'A', 'B', '2', '4', '2', 'A', '1', 'B', 'D',
        '7', 'F', '5', 'B', '6', '8', '9', 'F', 'A', 'D', '0', 'B', 'F', 'B',
        'E', '5', 'F', '6', '9', '5', '5', '6', 'C', 'F', 'F', '3', '8', '0',
        '6', '5', 'C', 'B', '1', '1', 'A', 'D', '4', 'F', '6', '1', 'B', 'C',
        '2', '9', '9', 'E', 'C', '8', '1', '0', '8', 'D', 'E', 'E', 'E', '9',
        'D', '8', '4', 'C', '2', 'E', 'C', '1', 'A', 'F', '2', 'F', '1', '5',
        '1', '5', 'B', '7', '2', '8', '0', '8', '9', '5', 'A', 'B', '0', '8',
        'F', 'F', '0', '2', '5', '8', '9', 'E', '4', '0', '3', '6', 'B', 'E',
        '5', '3', '3', 'B', 'E', '7', 'C', '6', '7', 'E', '3', '5', '0', '1',
        '9', 'B', '9', 'A', '3', '8', '4', '4', '7', '5', '1', '3', 'F', 'A',
        '7', '4', 'A', '9', '9', '0', '6', '3', '6', '9', '6', 'B', '4', 'B',
        '3', 'D', 'A', 'E', 'A', '6', '3', '6', '1', '4', 'E', '2', '8', '6',
        '4', 'D', '5', '5', '4', 'D', 'E', '5', 'F', '9', '5', 'F', 'B', 'C',
        'E', 'C', 'B', '9', '2', '7', '4', '5', '3', '6', '8', '4', '5', '3',
        '4', '7', '2', '2', 'D', '3', 'B', '8', '9', '9', 'B', 'E', '4', '0',
        '2', 'A', '6', '7', '9', 'B', '2', '5', '8', 'D', '9', '5', 'B', '0',
        '3', 'D', 'A', '4', '9', '9', '4', '2', '4', 'E', '5', 'E', '1', 'C',
        '3', 'B', '9', '6', 'E', '1', '9', 'A', '9', '7', '6', 'F', '9', '4',
        '0', '2', 'E', 'A', '\0'};

const unsigned char __MCC_TKHistogramNormalization_public_key[] = {
        '3', '0', '8', '1', '9', 'D', '3', '0', '0', 'D', '0', '6', '0', '9',
        '2', 'A', '8', '6', '4', '8', '8', '6', 'F', '7', '0', 'D', '0', '1',
        '0', '1', '0', '1', '0', '5', '0', '0', '0', '3', '8', '1', '8', 'B',
        '0', '0', '3', '0', '8', '1', '8', '7', '0', '2', '8', '1', '8', '1',
        '0', '0', 'C', '4', '9', 'C', 'A', 'C', '3', '4', 'E', 'D', '1', '3',
        'A', '5', '2', '0', '6', '5', '8', 'F', '6', 'F', '8', 'E', '0', '1',
        '3', '8', 'C', '4', '3', '1', '5', 'B', '4', '3', '1', '5', '2', '7',
        '7', 'E', 'D', '3', 'F', '7', 'D', 'A', 'E', '5', '3', '0', '9', '9',
        'D', 'B', '0', '8', 'E', 'E', '5', '8', '9', 'F', '8', '0', '4', 'D',
        '4', 'B', '9', '8', '1', '3', '2', '6', 'A', '5', '2', 'C', 'C', 'E',
        '4', '3', '8', '2', 'E', '9', 'F', '2', 'B', '4', 'D', '0', '8', '5',
        'E', 'B', '9', '5', '0', 'C', '7', 'A', 'B', '1', '2', 'E', 'D', 'E',
        '2', 'D', '4', '1', '2', '9', '7', '8', '2', '0', 'E', '6', '3', '7',
        '7', 'A', '5', 'F', 'E', 'B', '5', '6', '8', '9', 'D', '4', 'E', '6',
        '0', '3', '2', 'F', '6', '0', 'C', '4', '3', '0', '7', '4', 'A', '0',
        '4', 'C', '2', '6', 'A', 'B', '7', '2', 'F', '5', '4', 'B', '5', '1',
        'B', 'B', '4', '6', '0', '5', '7', '8', '7', '8', '5', 'B', '1', '9',
        '9', '0', '1', '4', '3', '1', '4', 'A', '6', '5', 'F', '0', '9', '0',
        'B', '6', '1', 'F', 'C', '2', '0', '1', '6', '9', '4', '5', '3', 'B',
        '5', '8', 'F', 'C', '8', 'B', 'A', '4', '3', 'E', '6', '7', '7', '6',
        'E', 'B', '7', 'E', 'C', 'D', '3', '1', '7', '8', 'B', '5', '6', 'A',
        'B', '0', 'F', 'A', '0', '6', 'D', 'D', '6', '4', '9', '6', '7', 'C',
        'B', '1', '4', '9', 'E', '5', '0', '2', '0', '1', '1', '1', '\0'};

static const char * MCC_TKHistogramNormalization_matlabpath_data[] = 
    { "TKHistogramNormalization/", "toolbox/compiler/deploy/",
      "$TOOLBOXMATLABDIR/general/", "$TOOLBOXMATLABDIR/ops/",
      "$TOOLBOXMATLABDIR/lang/", "$TOOLBOXMATLABDIR/elmat/",
      "$TOOLBOXMATLABDIR/elfun/", "$TOOLBOXMATLABDIR/specfun/",
      "$TOOLBOXMATLABDIR/matfun/", "$TOOLBOXMATLABDIR/datafun/",
      "$TOOLBOXMATLABDIR/polyfun/", "$TOOLBOXMATLABDIR/funfun/",
      "$TOOLBOXMATLABDIR/sparfun/", "$TOOLBOXMATLABDIR/scribe/",
      "$TOOLBOXMATLABDIR/graph2d/", "$TOOLBOXMATLABDIR/graph3d/",
      "$TOOLBOXMATLABDIR/specgraph/", "$TOOLBOXMATLABDIR/graphics/",
      "$TOOLBOXMATLABDIR/uitools/", "$TOOLBOXMATLABDIR/strfun/",
      "$TOOLBOXMATLABDIR/imagesci/", "$TOOLBOXMATLABDIR/iofun/",
      "$TOOLBOXMATLABDIR/audiovideo/", "$TOOLBOXMATLABDIR/timefun/",
      "$TOOLBOXMATLABDIR/datatypes/", "$TOOLBOXMATLABDIR/verctrl/",
      "$TOOLBOXMATLABDIR/codetools/", "$TOOLBOXMATLABDIR/helptools/",
      "$TOOLBOXMATLABDIR/demos/", "$TOOLBOXMATLABDIR/timeseries/",
      "$TOOLBOXMATLABDIR/hds/", "toolbox/local/",
      "toolbox/compiler/", "toolbox/control/control/",
      "toolbox/images/images/", "toolbox/images/imuitools/",
      "toolbox/images/iptutils/", "toolbox/shared/imageslib/",
      "toolbox/images/medformats/", "toolbox/optim/" };

static const char * MCC_TKHistogramNormalization_classpath_data[] = 
    { "java/jar/toolbox/control.jar", "java/jar/toolbox/images.jar" };

static const char * MCC_TKHistogramNormalization_libpath_data[] = 
    { "bin/glnx86/" };

static const char * MCC_TKHistogramNormalization_app_opts_data[] = 
    { "" };

static const char * MCC_TKHistogramNormalization_run_opts_data[] = 
    { "" };

static const char * MCC_TKHistogramNormalization_warning_state_data[] = 
    { "" };


mclComponentData __MCC_TKHistogramNormalization_component_data = { 

    /* Public key data */
    __MCC_TKHistogramNormalization_public_key,

    /* Component name */
    "TKHistogramNormalization",

    /* Component Root */
    "",

    /* Application key data */
    __MCC_TKHistogramNormalization_session_key,

    /* Component's MATLAB Path */
    MCC_TKHistogramNormalization_matlabpath_data,

    /* Number of directories in the MATLAB Path */
    40,

    /* Component's Java class path */
    MCC_TKHistogramNormalization_classpath_data,
    /* Number of directories in the Java class path */
    2,

    /* Component's load library path (for extra shared libraries) */
    MCC_TKHistogramNormalization_libpath_data,
    /* Number of directories in the load library path */
    1,

    /* MCR instance-specific runtime options */
    MCC_TKHistogramNormalization_app_opts_data,
    /* Number of MCR instance-specific runtime options */
    0,

    /* MCR global runtime options */
    MCC_TKHistogramNormalization_run_opts_data,
    /* Number of MCR global runtime options */
    0,
    
    /* Component preferences directory */
    "TKHistogramNormalization_E41C44A02C7C0E16CB3642DBB1EAC3EE",

    /* MCR warning status data */
    MCC_TKHistogramNormalization_warning_state_data,
    /* Number of MCR warning status modifiers */
    0,

    /* Path to component - evaluated at runtime */
    NULL

};

#ifdef __cplusplus
}
#endif


