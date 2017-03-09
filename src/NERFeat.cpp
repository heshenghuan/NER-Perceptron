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

#include "NERFeat.h"

namespace nerp
{
    NERFeat::NERFeat()
    {
        _unigram = new Vocabulary;
        _bigram = new Vocabulary;
        _trigram = new Vocabulary;
        _dict = new Vocabulary;
        _modifiable = true;
    }
    
    NERFeat::~NERFeat()
    {
        delete _unigram;
        delete _bigram;
        delete _trigram;
        delete _dict;
    }
    
    int NERFeat::GetUnigramIndex(const char *feat)
    {
        return this->_unigram->GetIndex(feat);
    }

    int NERFeat::GetBigramIndex(const char *feat)
    {
        return this->_bigram->GetIndex(feat);
    }

    int NERFeat::GetTrigramIndex(const char *feat)
    {
        return this->_trigram->GetIndex(feat);
    }

    int NERFeat::GetDictIndex(const char *feat)
    {
        return this->_dict->GetIndex(feat);
    }

    int NERFeat::InsertUnigramFeat(const char *feat)
    {
        return this->_unigram->GetAndInsertIndex(feat);
    }
    
    int NERFeat::InsertBigramFeat(const char *feat)
    {
        return this->_bigram->GetAndInsertIndex(feat);
    }

    int NERFeat::InsertTrigramFeat(const char *feat)
    {
        return this->_trigram->GetAndInsertIndex(feat);
    }

    int NERFeat::InsertDictFeat(const char *feat)
    {
        return this->_dict->GetAndInsertIndex(feat);
    }

    bool NERFeat::LoadFeatureFile(const char *FeatureFileName)
    {
        FILE * FeatureFile;
        FeatureFile = fopen(FeatureFileName, "rb");
        if( !FeatureFile )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Can not open the Feature Model File: "<<FeatureFileName<<endl;
            return false;
        }
        char headBuf[UNIGRAM_LEN_MAX];
        fread(&headBuf, g_Header_Len, 1, FeatureFile);
        fclose(FeatureFile);
        string header = string(headBuf, g_Header_Len);
        if (header == g_Model_Header)
        {
            return ReadBinaryFile(FeatureFileName);
        }
        else
        {
            return ReadFile(FeatureFileName);
        }
    }

    bool NERFeat::SaveFeatureFile()
    {
        if( ( _unigram==NULL ) || (_unigram->size()<=0) )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Does not have any feature in Unigram."<<endl;
            return false;
        }
        if( ( _bigram==NULL ) || (_bigram->size()<=0) )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Does not have any feature in Bigram."<<endl;
            return false;
        }
        if( ( _trigram==NULL ) || (_trigram->size()<=0) )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Does not have any feature in Trigram."<<endl;
            return false;
        }
#ifdef WIN32
        string _datapath = "model\\";
#else
        string _datapath = "model/";
#endif
        string FileName = _datapath + "Feat";
        FILE * FeatureFile;
        FeatureFile = fopen(FileName.c_str(), "w");
        
        // write unigram feature
        string unigramSize = toString(_unigram->size()) + '\n';
        string unigramHeader = "#UnigramFeature\n"; 
        fwrite(unigramHeader.data(), unigramHeader.length(), 1, FeatureFile);
        fwrite(unigramSize.data(), unigramSize.length(), 1, FeatureFile);
        _unigram->WriteVocabText(FeatureFile);

        // write bigram feature
        string bigramSize = toString(_bigram->size()) + '\n';
        string bigramHeader = "#BigramFeature\n"; 
        fwrite(bigramHeader.data(), bigramHeader.length(), 1, FeatureFile);
        fwrite(bigramSize.data(), bigramSize.length(), 1, FeatureFile);
        _bigram->WriteVocabText(FeatureFile);

        // write trigram feature
        string trigramSize = toString(_trigram->size()) + '\n';
        string trigramHeader = "#TrigramFeature\n"; 
        fwrite(trigramHeader.data(), trigramHeader.length(), 1, FeatureFile);
        fwrite(trigramSize.data(), trigramSize.length(), 1, FeatureFile);
        _trigram->WriteVocabText(FeatureFile);

        // write dict feature
        string dictSize = toString(_dict->size()) + '\n';
        string dictHeader = "#DictFeature\n"; 
        fwrite(dictHeader.data(), dictHeader.length(), 1, FeatureFile);
        fwrite(dictSize.data(), dictSize.length(), 1, FeatureFile);
        _dict->WriteVocabText(FeatureFile);

        fclose(FeatureFile);
        std::cout<<"NERFeat file have been saved.\n";
        return true;
    }

    bool NERFeat::ReadFile(const char* FileName)
    {
        ifstream fin;
        fin.open(FileName);
        if( !fin.is_open() )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Can not open the Feature model file: "<<FileName<<endl;
            return false;
        }

        string myTextLine;
        vector<string> tmp;
        delete this->_unigram;
        delete this->_bigram;
        delete this->_trigram;
        delete this->_dict;
        this->_bigram = new Vocabulary;
        this->_unigram = new Vocabulary;
        this->_trigram = new Vocabulary;
        this->_dict = new Vocabulary;

        // UnigramFeature
        getline(fin, myTextLine);  // skip the first line
        getline(fin, myTextLine);
        int unigramSize = fromString<int>(myTextLine);
        for(int i=0; i<unigramSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            tmp = string_split(myTextLine, " ");
            string feat = tmp.front();
            int index = fromString<int>(tmp.back());
            this->_unigram->InserWordAndIndex(feat, index);
        }
        // BigramFeature
        getline(fin, myTextLine);
        getline(fin, myTextLine);
        int bigramSize = fromString<int>(myTextLine);
        for(int i=0; i<bigramSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            tmp = string_split(myTextLine, " ");
            string feat = tmp.front();
            int index = fromString<int>(tmp.back());
            this->_bigram->InserWordAndIndex(feat, index);
        }
        //TrigramFeature
        getline(fin, myTextLine);
        getline(fin, myTextLine);
        int trigramSize = fromString<int>(myTextLine);
        for(int i=0; i<trigramSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            tmp = string_split(myTextLine, " ");
            string feat = tmp.front();
            int index = fromString<int>(tmp.back());
            this->_trigram->InserWordAndIndex(feat, index);
        }
        // DitcFeature
        getline(fin, myTextLine);
        getline(fin, myTextLine);
        int dictSize = fromString<int>(myTextLine);
        for(int i=0; i<dictSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            tmp = string_split(myTextLine, " ");
            string feat = tmp.front();
            int index = fromString<int>(tmp.back());
            this->_dict->InserWordAndIndex(feat, index);
        }
        fin.close();
        return true;
    }

    bool NERFeat::ReadBinaryFile(const char* FileName)
    {
        FILE * FeatureFile;
        FeatureFile = fopen(FileName, "rb");
        if( !FeatureFile )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Can not open the Feature model file: "<<FileName<<endl;
            return false;
        }
        char headBuf[UNIGRAM_LEN_MAX];
        fread(&headBuf, g_Header_Len, 1, FeatureFile);
        string header = string(headBuf, g_Header_Len);

		delete this->_unigram;
        delete this->_bigram;
        delete this->_trigram;
        this->_bigram = new Vocabulary;
        this->_unigram = new Vocabulary;
        this->_trigram = new Vocabulary;

        int unigramSize, bigramSize, trigramSize, dictSize;
        fread(&unigramSize, sizeof(int), 1, FeatureFile);
        int index, unit;
        string word;
        for (int i=0; i<unigramSize; i++)
        {
            fread(&unit, sizeof(int), 1, FeatureFile);
            char buf[UNIGRAM_LEN_MAX];
            fread((void*)&buf, unit, 1, FeatureFile);
            word = string(buf, unit);
            fread(&index, sizeof(int), 1, FeatureFile);
            this->_unigram->InserWordAndIndex(word, index);
        }

        fread(&bigramSize, sizeof(int), 1, FeatureFile);
        for (int i=0; i<bigramSize; i++)
        {
            fread(&unit, sizeof(int), 1, FeatureFile);
            char buf[UNIGRAM_LEN_MAX];
            fread((void*)&buf, unit, 1, FeatureFile);
            word = string(buf, unit);
            fread(&index, sizeof(int), 1, FeatureFile);
            this->_bigram->InserWordAndIndex(word, index);
        }

        fread(&trigramSize, sizeof(int), 1, FeatureFile);
        for (int i=0; i<trigramSize; i++)
        {
            fread(&unit, sizeof(int), 1, FeatureFile);
            char buf[UNIGRAM_LEN_MAX];
            fread((void*)&buf, unit, 1, FeatureFile);
            word = string(buf, unit);
            fread(&index, sizeof(int), 1, FeatureFile);
            this->_trigram->InserWordAndIndex(word, index);
        }

        fread(&dictSize, sizeof(int), 1, FeatureFile);
        for (int i=0; i<dictSize; i++)
        {
            fread(&unit, sizeof(int), 1, FeatureFile);
            char buf[UNIGRAM_LEN_MAX];
            fread((void*)&buf, unit, 1, FeatureFile);
            word = string(buf, unit);
            fread(&index, sizeof(int), 1, FeatureFile);
            this->_dict->InserWordAndIndex(word, index);
        }
        fclose(FeatureFile);
        return true;
    }

    bool NERFeat::ConvertToBinaryFile(const char* InputFileName, const char* OutputFileName)
    {
        if (!ReadFile(InputFileName)) return false;
        std::cout<<"Load text Feature file finished."<<endl;
        FILE* str_lm_file;
        FILE* bin_lm_file;
        str_lm_file=fopen(InputFileName,"r");
        if( !str_lm_file )
        {
            cerr << "\nNERFeat ERROR" << endl;
            cerr << "Can not open the Language Model File: "<<InputFileName<<endl;
            return false;
        }
        bin_lm_file=fopen(OutputFileName,"wb");
        fwrite(g_Model_Header.data(), g_Header_Len, 1, bin_lm_file);
        int unigramSize = this->_unigram->size();
        fwrite(&unigramSize, sizeof(int), 1, bin_lm_file);
        this->_unigram->WriteVocabIndex(bin_lm_file);

        int bigramSize = this->_bigram->size();
        fwrite(&bigramSize, sizeof(int), 1, bin_lm_file);
        this->_bigram->WriteVocabIndex(bin_lm_file);

        int trigramSize = this->_trigram->size();
        fwrite(&trigramSize, sizeof(int), 1, bin_lm_file);
        this->_trigram->WriteVocabIndex(bin_lm_file);

        int dictSize = this->_dict->size();
        fwrite(&dictSize, sizeof(int), 1, bin_lm_file);
        this->_dict->WriteVocabIndex(bin_lm_file);

        fclose(str_lm_file);
        fclose(bin_lm_file);
        return true;
    }
}