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

#include "Probability.h"

namespace nerp
{
    Probability::Probability(int size)
    {
        _init_prob = new vector<double>;
        _trans_prob = new vector< vector<double> >;
        _tagsetSize = size
        for(int i=0; i<_tagsetSize; i++)
        {
            _init_prob->push_back(0.0);
            vector<double> tmp (_tagsetSize, 0.0);
            _trans_prob->push_back(tmp);
        }

        // tag2index->insert(make_pair("B", 0));
        // tag2index->insert(make_pair("M", 1));
        // tag2index->insert(make_pair("E", 2));
        // tag2index->insert(make_pair("S", 3));
    }

    Probability::Probability()
    {
        _init_prob = new vector<double>;
        _trans_prob = new vector< vector<double> >;
        _tagsetSize = 0
    }

    Probability::~Probability()
    {   
        delete _init_prob;
        delete _trans_prob;
    }

    void Probability::InitProbCount(int tag)
    {
        if (this->_init_prob == NULL)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Error: the init_prob point to NULL!"<<endl;
            return;
        }
        if (tag < 0 || tag >= _tagsetSize)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Given tag beyond the range of tagset size!"<<endl;
            return;
        }
        if (_tagsetSize <= 0)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Not given the tagset size!"<<endl;
            return;
        }
        (this->_init_prob->at(tag))++;
        return;
    }

    void Probability::TransProbCount(int s, int d)
    {
        if (this->_trans_prob == NULL)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Error: the trans_prob point to NULL!"<<endl;
            return;
        }
        if (s < 0 || s >= _tagsetSize || d < 0|| d >= _tagsetSize)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Given tag beyond the range of 4-tag-system!"<<endl;
            return;
        }
        if (_tagsetSize <= 0)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Not given the tagset size!"<<endl;
            return;
        }
        (this->_trans_prob->at(s).at(d))++;
        return;
    }

    bool Probability::CalcAllProb()
    {
        double init_sum = 0.0;
        vector<double> trans_sum (_tagsetSize, 0.0);

        for (int i=0; i<_tagsetSize; i++)
        {
            init_sum += this->_init_prob->at(i);
            for(int j=0;j<_tagsetSize;j++)
                trans_sum[i] += this->_trans_prob->at(i).at(j);
        }

        for (int i=0;i<_tagsetSize;i++)
        {
            this->_init_prob->at(i) /= init_sum;
            for(int j=0;j<_tagsetSize;j++)
                this->_trans_prob->at(i).at(j) /= trans_sum[i];
        }

        for (int i=0;i<_tagsetSize;i++)
        {
            if (this->_init_prob->at(i) != 0.0)
                this->_init_prob->at(i) = log(this->_init_prob->at(i));
            else 
                this->_init_prob->at(i) = LogP_Zero;
            for(int j=0;j<_tagsetSize;j++)
            {
                if (this->_trans_prob->at(i).at(j) != 0.0)
                    this->_trans_prob->at(i).at(j) = log(this->_trans_prob->at(i).at(j));
                else 
                    this->_trans_prob->at(i).at(j) = LogP_Zero;
            }
        }
        return true;
    }

    double Probability::GetInitProb(int tag)
    {
        if (this->_init_prob == NULL)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Error: the init_prob point to NULL!"<<endl;
            return 0.0;
        }
        if (tag < 0 || tag >= _tagsetSize)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Given tag beyond the range of 4-tag-system!"<<endl;
            return 0.0;
        }
        return this->_init_prob->at(tag);
    }

    double Probability::GetTransProb(int s, int d)
    {
        if (this->_trans_prob == NULL)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Error: the trans_prob point to NULL!"<<endl;
            return 0.0;
        }
        if (s < 0 || s >= _tagsetSize || d < 0|| d >= _tagsetSize)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Given tag beyond the range of 4-tag-system!"<<endl;
            return 0.0;
        }
        return this->_trans_prob->at(s).at(d);
    }

    bool Probability::LoadProbFile(const char *FileName)
    {
        FILE * ProbFile;
        ProbFile = fopen(FileName, "rb");
        if (!ProbFile)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Can not open the Probability info file: "<<FileName<<endl;
            return false;
        }
        char headBuf[UNIGRAM_LEN_MAX];
        fread(&headBuf, g_Header_Len, 1, ProbFile);
        fclose(ProbFile);
        string header = string(headBuf, g_Header_Len);
        if (header == g_Model_Header)
        {
            return ReadBinaryFile(FileName);
        }
        else
        {
            return ReadFile(FileName);
        }
    }

    bool Probability::SaveProbFile()
    {
        if (this->_init_prob == NULL || this->_trans_prob == NULL)
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Does not have any probability information." << endl;
            cerr << "The init_prob & trans_prob point to NULL!" << endl;
            return false;
        }
#ifdef WIN32
        string _datapath = "model\\";
#else
        string _datapath = "model/";
#endif
        string FileName = _datapath + "Prob";
        FILE * ProbFile;
        ProbFile = fopen(FileName.c_str(), "w");

        // write init prob
        string Header = "#Probability\n";
        string tagsetSize = toString(_tagsetSize) + "\n";
        fwrite(Header.data(), Header.length(), 1, ProbFile);
        fwrite(tagsetSize.data(), tagsetSize.length(), 1, ProbFile);

        Header = "#InitProb\n";
        fwrite(Header.data(), Header.length(), 1, ProbFile);
        for(int i=0;i<_tagsetSize;i++)
        {
            string prob;
            if (_init_prob->at(i) == LogP_Zero)
                prob = "-Inf\n";
            else
                prob = toString(_init_prob->at(i)) + "\n";
            fwrite(prob.data(), prob.length(), 1, ProbFile);
        }

        Header = "#TransProb\n";
        fwrite(Header.data(), Header.length(), 1, ProbFile);
        for(int i=0;i<_tagsetSize;i++)
        {
            string prob = "";
            for(int j=0;j<_tagsetSize;j++)
            {
                if (_trans_prob->at(i).at(j) == LogP_Zero)
                    prob += "-Inf ";
                else
                    prob += toString(_trans_prob->at(i).at(j)) + " ";
            }
            prob += "\n";
            fwrite(prob.data(), prob.length(), 1, ProbFile);
        }
        std::cout<<"Probability file have been saved.\n";
        return true;
    }

    bool Probability::ConvertToBinaryFile(const char* InputFileName, const char* OutputFileName)
    {
        if (!ReadFile(InputFileName)) return false;
        std::cout<<"Load text Probability file finished."<<endl;
        FILE* bin_lm_file;
        bin_lm_file=fopen(OutputFileName,"wb");
        fwrite(g_Model_Header.data(), g_Header_Len, 1, bin_lm_file);
        fwrite(&_tagsetSize, sizeof(int), 1, bin_lm_file);

        for(int i=0; i<_tagsetSize; i++)
        {
            fwrite(&this->_init_prob->at(i), sizeof(double), 1, bin_lm_file);
        }

        for(int i=0; i<_tagsetSize; i++)
        {
            for(int j=0; j<_tagsetSize; j++)
            {
                fwrite(&this->_trans_prob->at(i).at(j), sizeof(double), 1, bin_lm_file);
            }
        }
        fclose(bin_lm_file);
        std::cout<<"Convert to binary file finished!"<<endl;
        return true;
    }

    bool Probability::ReadFile(const char *FileName)
    {
        ifstream fin;
        fin.open(FileName);
        if( !fin.is_open() )
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Can not open the Probability info file: "<<FileName<<endl;
            return false;
        }

        string myTextLine;
        vector<string> tmp;
        delete this->_init_prob;
        delete this->_trans_prob;
        this->_init_prob = new vector<double>;
        this->_trans_prob = new vector< vector<double> >;

        getline(fin, myTextLine);  // skip header line
        getline(fin, myTextLine);  // Read tagset size
        _tagsetSize = fromString<int>(myTextLine);

        getline(fin, myTextLine);  // skip header line
        for(int i=0; i<_tagsetSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            double prob;
            if (myTextLine != "-Inf")
            {
                prob = fromString<double>(myTextLine);
                this->_init_prob->push_back(prob);
            }
            else
            {
                this->_init_prob->push_back(LogP_Zero);
            }
        }

        getline(fin, myTextLine);  // skip header line
        for(int i=0; i<_tagsetSize; i++)
        {
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            istringstream is(myTextLine.c_str());
            string tmp;
            vector<double> probs;
            while(is >> tmp){
                if (tmp != "-Inf" && tmp != "-inf")
                {
                    double prob = fromString<double>(tmp);
                    probs.push_back(prob);
                }
                else
                    probs.push_back(LogP_Zero);
            }
            this->_trans_prob->push_back(probs);
        }
        fin.close();
        return true;
    }

    bool Probability::ReadBinaryFile(const char *FileName)
    {
        FILE * probFile;
        probFile = fopen(FileName, "rb");
        if( !probFile )
        {
            cerr << "\nProbability ERROR" << endl;
            cerr << "Can not open the Probability info file: "<<FileName<<endl;
            return false;
        }
        char headBuf[UNIGRAM_LEN_MAX];
        fread(&headBuf, g_Header_Len, 1, probFile);
        string header = string(headBuf, g_Header_Len);
        delete this->_init_prob;
        delete this->_trans_prob;
        this->_init_prob = new vector<double>;
        this->_trans_prob = new vector< vector<double> >;

        fread(&_tagsetSize, sizeof(int), 1, probFile);
        for(int i=0; i<_tagsetSize; i++)
        {
            double prob;
            fread(&prob, sizeof(double), 1,probFile);
            this->_init_prob->push_back(prob);
        }

        for(int i=0; i<_tagsetSize; i++)
        {
            vector<double> probs;
            for(int j=0; j<_tagsetSize; j++)
            {
                double prob;
                fread(&prob, sizeof(double), 1,probFile);
                probs.push_back(prob);
            }
            this->_trans_prob->push_back(probs);
        }
        return true;
    }
}