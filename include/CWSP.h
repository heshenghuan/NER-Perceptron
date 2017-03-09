#ifndef _CWSP_API_
#define _CWSP_API_

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#ifdef WIN32
    #define CWSP_API extern "C" __declspec(dllexport)
#else
    #define CWSP_API extern "C"
#endif

CWSP_API bool CWSP_InitializeDefault();

CWSP_API bool CWSP_Initialize(bool is_char_bin, string dictfile, string &featfile, string &probfile, string &mpfile);

CWSP_API void CWSP_AddChar(char* c);

CWSP_API void CWSP_ClearCharBuf();

CWSP_API void CWSP_SegSent(string &output);

CWSP_API void CWSP_SegFile(const char* inputfile,const char* outputfile);

CWSP_API void CWSP_Exit();

#endif