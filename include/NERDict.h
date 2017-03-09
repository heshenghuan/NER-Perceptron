/********************************************************************
* nerp: Chinese Named Entity Recognition Toolkit used multi-perceptron.
* Copyright (C) 2017 Shenghuan He
* Contact: heshenghuan328@gmail.com

* Permission to use, copy, modify, and distribute this software for
* any non-commercial purpose is hereby granted without fee, provided
* that the above copyright notice appear in all copies and that both
* that copyright notice. 
* It is provided "as is" without express or implied warranty.
*********************************************************************/

#ifndef NERDICT_H_
#define NERDICT_H_

#include "Config.h"
#include "StrFun.h"

namespace nerp
{
    class NERDict
    {
    public:
        NERDict(const char *DictFileName);
        NERDict();
        ~NERDict();

        pair<int, string> GetDictInfo(const char *feat);       // get dictionary feat info
        int size();
        bool LoadDictFile(const char *DictFileName);
        bool SaveDictFile();
        bool ConvertToBinaryFile(const char* InputFileName, const char* OutputFileName);
    private:
        bool ReadFile(const char* FileName);
        bool ReadBinaryFile(const char* FileName);
    private:
        map<string, pair<int, string> > *_dict;
        typedef map<string, pair<int, string> >::iterator _Str2PairMapIter;
    };
}

#endif