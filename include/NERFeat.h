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

#ifndef NERFEAT_H_
#define NERFEAT_H_

#include "Config.h"
#include "StrFun.h"
#include "Vocabulary.h"

namespace nerp
{
    class NERFeat
    {
    public:
        NERFeat();
        ~NERFeat();
        int GetUnigramIndex(const char *feat);  // get unigram feat's index
        int GetBigramIndex(const char *feat);   // get bigram feat's index
        int GetTrigramIndex(const char *feat);  // get trigram feat's index
        int GetDictIndex(const char *feat);     // get dictionary feat's index
        int UnigramLen()
        {
            return _unigram->size();
        }
        int BigramLen()
        {
            return _bigram->size();
        }
        int TrigramLen()
        {
            return _trigram->size();
        }
        int DictFeatLen()
        {
            return _dict->size();
        }

        int InsertUnigramFeat(const char *feat);   //Insert and get the index of Unigram
        int InsertBigramFeat(const char *feat);    //Insert and get the index of Bigram
        int InsertTrigramFeat(const char *feat);   //Insert and get the index of Trigram
        int InsertDictFeat(const char *feat);      //Insert and get the index of DictFeat

        bool LoadFeatureFile(const char *UnigramFileName);
        bool SaveFeatureFile();
        bool ConvertToBinaryFile(const char* InputFileName, const char* OutputFileName);

    private:
        bool ReadFile(const char* FileName);
        bool ReadBinaryFile(const char* FileName);
        // vector<string> SplitString(string terms_str, string spliting_tag);
    private:
        Vocabulary *_unigram;
        Vocabulary *_bigram;
        Vocabulary *_trigram;
        Vocabulary *_dict;
        bool _modifiable;    // if _modifiable is "false", you cannot insert, load from file...
    };
}

#endif