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

#ifndef TAGSET_H_
#define TAGSET_H_

#include "Config.h"

namespace nerp
{
    class Tagset
    {
    public:
        Tagset():_TagsetSize(0){}
        ~Tagset();

        // 查找或加入标注序号
        int GetAndInsertIndex(const char *tag)
        {
            map<string, int>::iterator it = tag2index.find(tag);
            if(it != tag2index.end())
                return (*it).second;
            else
                return Insert(tag);
        }

        void InserTagAndIndex(string tag, int index)
        {    
            index2tag.insert(map<int, string>::value_type(index, tag));
            tag2index.insert(map<string, int>::value_type(tag, index));
            _TagsetSize++;
        }

        int GetIndex(const char *tag)
        {
            map<string, int>::iterator it=tag2index.find(tag);
            if(it != tag2index.end())
                return (*it).second;
            else
                return Vocab_None;
        };

        //将序号转化为标注
        string GetTag(int index)
        {
            if( index<_TagsetSize )
                return index2tag[index].c_str();
            else
                return "NULL";
        };

        int size()
        {
            return _TagsetSize;
        };

        int GetAndInsertTagsIndics(char** tags, int *tids, int max);        //将词转化为词号或插入
        int GetAndInsertTagsIndics(char** tags, vector<int> &tids, int max);
        int GetTagIndics(char** tags, int *tids, int max, int &UnkownNum);    //将此转化为词号，并统计未知词的数量

        bool SaveTagsetFile();
        bool LoadTagsetFile(const char *TagsetFileName);
        bool ConvertToBinaryFile(const char* InputFileName, const char* OutputFileName);

    private:
        bool WriteTagsetIndex(FILE * file);
        bool WriteTagsetText(FILE * file);
        bool ReadFile(const char* FileName);
        bool ReadBinaryFile(const char* FileName);
    private:
        int Insert(string tag)
        {
            // _TagsetSize++;
            index2tag.insert(map<int, string>::value_type(_TagsetSize, tag));
            tag2index.insert(map<string, int>::value_type(tag, _TagsetSize));
            return _TagsetSize++;
        }
        int _TagsetSize;
        map<string, int> tag2index;
        map<int, string> index2tag;
    };
}