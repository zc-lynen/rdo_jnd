/*
 * MATLAB Compiler: 7.1 (R2019b)
 * Date: Wed May 29 18:09:20 2024
 * Arguments:
 * "-B""macro_default""-W""lib:jnd_mat""-T""link:lib""func_JND_modeling_pattern_
 * complexity.m"
 */

#define EXPORTING_jnd_mat 1
#include "jnd_mat.h"

static HMCRINSTANCE _mcr_inst = NULL; /* don't use nullptr; this may be either C or C++ */

#if defined( _MSC_VER) || defined(__LCC__) || defined(__MINGW64__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#define NOMINMAX
#include <windows.h>
#undef interface

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

static int mclDefaultPrintHandler(const char *s)
{
    return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern C block */
#endif

#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

static int mclDefaultErrorHandler(const char *s)
{
    int written = 0;
    size_t len = 0;
    len = strlen(s);
    written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
    if (len > 0 && s[ len-1 ] != '\n')
        written += mclWrite(2 /* stderr */, "\n", sizeof(char));
    return written;
}

#ifdef __cplusplus
} /* End extern C block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_jnd_mat_C_API
#define LIB_jnd_mat_C_API /* No special import/export declaration */
#endif

LIB_jnd_mat_C_API 
bool MW_CALL_CONV jnd_matInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
    if (_mcr_inst)
        return true;
    if (!mclmcrInitialize())
        return false;
    if (!GetModuleFileName(GetModuleHandle("jnd_mat"), path_to_dll, _MAX_PATH))
        return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream(path_to_dll);
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(&_mcr_inst,
                                                             error_handler, 
                                                             print_handler,
                                                             ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
    return true;
}

LIB_jnd_mat_C_API 
bool MW_CALL_CONV jnd_matInitialize(void)
{
    return jnd_matInitializeWithHandlers(mclDefaultErrorHandler, mclDefaultPrintHandler);
}

LIB_jnd_mat_C_API 
void MW_CALL_CONV jnd_matTerminate(void)
{
    if (_mcr_inst)
        mclTerminateInstance(&_mcr_inst);
}

LIB_jnd_mat_C_API 
void MW_CALL_CONV jnd_matPrintStackTrace(void) 
{
    char** stackTrace;
    int stackDepth = mclGetStackTrace(&stackTrace);
    int i;
    for(i=0; i<stackDepth; i++)
    {
        mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
        mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
    }
    mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_jnd_mat_C_API 
bool MW_CALL_CONV mlxFunc_JND_modeling_pattern_complexity(int nlhs, mxArray *plhs[], int 
                                                          nrhs, mxArray *prhs[])
{
    return mclFeval(_mcr_inst, "func_JND_modeling_pattern_complexity", nlhs, plhs, nrhs, 
                  prhs);
}

LIB_jnd_mat_C_API 
bool MW_CALL_CONV mlfFunc_JND_modeling_pattern_complexity(int nargout, mxArray** jnd_map, 
                                                          mxArray** jnd_LA, mxArray** 
                                                          jnd_LC, mxArray** jnd_PM, 
                                                          mxArray** jnd_PM_p, mxArray* 
                                                          img)
{
    return mclMlfFeval(_mcr_inst, "func_JND_modeling_pattern_complexity", nargout, 5, 1, jnd_map, jnd_LA, jnd_LC, jnd_PM, jnd_PM_p, img);
}

