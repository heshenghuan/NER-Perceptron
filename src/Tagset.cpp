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

#include "Tagset.h"

namespace nerp
{
    bool Tagset::SaveTagsetFile()
    {
        if(_TagsetSize == 0)
        {
            cerr << "\nTagset ERROR" << endl;
            cerr << "Does not have any tag so far." << endl;
            return false;
        }
#ifdef WIN32
        string _datapath = "model\\";
#else
        string _datapath = "model/";
#endif
        string FileName = _datapath + "Tagset";
        FILE * TagsetFile;
        TagsetFile = fopen(FileName.c_str(), "w");

        string tagsetSize = toString(_TagsetSize) + '\n';
        string tagsetHeader = "#Tagset\n";
        fwrite(tagsetHeader.data(), tagsetHeader.length(), 1, TagsetFile);
        fwrite(tagsetSize.data(), tagsetSize.length(), 1, TagsetFile);
        WriteTagsetText(TagsetFile);

        fclose(TagsetFile);
        std::cout << "Tagset file have been saved.\n";
        return true;
    }

    bool Tagset::LoadTagsetFile(const char *TagsetFileName)
    {
        FILE * TagsetFile;
        TagsetFile = fopen(TagsetFileName, "rb");
        if(!TagsetFile)
        {
            cerr << "\nTagset Loading ERROR" << endl;
            cerr << "Can not open the Tagset File: "<<TagsetFileName<<endl;
            return false;
        }
        char headBuf[UNIGRAM_LEN_MAX];
        fread(&headBuf, g_Header_Len, 1, TagsetFile);
        fclose(TagsetFile);
        string header = string(headBuf, g_Header_Len);
        if (header == g_Model_Header)
        {
            return ReadBinaryFile(TagsetFileName);
        }
        else
        {
            return ReadFile(TagsetFileName);
        }
    }

    bool Tagset::ConvertToBinaryFile(const char* InputFileName, const char* OutputFileName)
    {
        if(!ReadFile(InputFileName)) return false;
        std::cout<<"Load text Tagset file finished."<<endl;
        FILE* bin_out_file;

        bin_out_file=fopen(OutputFileName,"wb");
        fwrite(g_Model_Header.data(), g_Header_Len, 1, bin_out_file);
        fwrite(&_TagsetSize, sizeof(int), 1, bin_out_file);
        WriteTagsetIndex(bin_out_file);

        fclose(bin_out_file);
        return true;
    }

    bool Tagset::WriteTagsetIndex(FILE * file)
    {
        map<string, int>::iterator iter;
        for (iter = tag2index.begin();iter != tag2index.end();++iter)
        {
            string myTag = (*iter).first;
            int unit = (int)myTag.length();
            int myId = (*iter).second;
            fwrite(&unit, sizeof(int), 1, file);
            fwrite(myTag.data(), myTag.length(), 1, file);
            fwrite(&myId, sizeof(int), 1, file);
        }
        return true;
    }

    bool Tagset::WriteTagsetText(FILE * file)
    {
        map<string, int>::iterator iter;
        for (iter = tag2index.begin(); iter != tag2index.end(); ++iter)
        {
            string myTag = (*iter).first + " " + toString((*iter).second) + '\n';
            fwrite(myTag.data(), myTag.length(), 1, file);
        }
        return true;
    }

    bool Tagset::ReadFile(const char* FileName)
    {
        ifstream fin;
        fin.open(FileName);
        if( !fin.is_open() )
        {
            cerr << "\nTagset readfile ERROR" << endl;
            cerr << "Can not open the Tagset file: "<<FileName<<endl;
            return false;
        }

        string myTextLine;
        vector<string> tmp;
        _TagsetSize = 0;
        tag2index.clear();
        index2tag.clear();

        getline(fin, myTextLine);  // skip the first line
        getline(fin, myTextLine);
        int tagsetSize = fromString<int>(myTextLine);
        for(int i=0; i<tagsetSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            tmp = string_split(myTextLine, " ");
            string tag = tmp.front();
            int index = fromString<int>(tmp.back());
            InserTagAndIndex(tag, index);
        }
        fin.close();
        return true;
    }

    bool Tagset::ReadBinaryFile(const char* FileName)
    {
        FILE * TagsetFile;
        TagsetFile = fopen(FileName, "rb");
        if(!TagsetFile)
        {
            cerr << "\nTagset read ERROR" << endl;
            cerr << "Can not open the tagset file: "<<FileName<<endl;
            return false;
        }
        char headBuf[UNIGRAM_LEN_MAX];
        fread(&headBuf, g_Header_Len, 1, TagsetFile);
        string header = string(headBuf, g_Header_Len);

        _TagsetSize = 0;
        tag2index.clear();
        index2tag.clear();

        int tagsetSize;
        fread(&tagsetSize, sizeof(int), 1, TagsetFile);
        int index, unit;
        string tag;
        for (int i=0; i<tagsetSize; i++)
        {
            fread(&unit, sizeof(int), 1, TagsetFile);
            char buf[UNIGRAM_LEN_MAX];
            fread((void*)&buf, unit, 1, TagsetFile);
            tag = string(buf, unit);
            fread(&index, sizeof(int), 1, TagsetFile);
            InserTagAndIndex(tag, index);
        }

        fclose(TagsetFile);
        return true;
    }
}