/*
 * MATLAB Compiler: 7.1 (R2019b)
 * Date: Wed May 29 18:09:20 2024
 * Arguments:
 * "-B""macro_default""-W""lib:jnd_mat""-T""link:lib""func_JND_modeling_pattern_
 * complexity.m"
 */

#ifndef jnd_mat_h
#define jnd_mat_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_jnd_mat_C_API 
#define LIB_jnd_mat_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_jnd_mat_C_API 
bool MW_CALL_CONV jnd_matInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_jnd_mat_C_API 
bool MW_CALL_CONV jnd_matInitialize(void);

extern LIB_jnd_mat_C_API 
void MW_CALL_CONV jnd_matTerminate(void);

extern LIB_jnd_mat_C_API 
void MW_CALL_CONV jnd_matPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_jnd_mat_C_API 
bool MW_CALL_CONV mlxFunc_JND_modeling_pattern_complexity(int nlhs, mxArray *plhs[], int 
                                                          nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

/* C INTERFACE -- MLF WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_jnd_mat_C_API bool MW_CALL_CONV mlfFunc_JND_modeling_pattern_complexity(int nargout, mxArray** jnd_map, mxArray** jnd_LA, mxArray** jnd_LC, mxArray** jnd_PM, mxArray** jnd_PM_p, mxArray* img);

#ifdef __cplusplus
}
#endif
/* C INTERFACE -- MLF WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#endif
