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
#include "Recognizer.h"

// NUSTM_CWSP* NUSTM_CWSP::Create()
// {
//     return new cwsp::Recognizer();
// }

namespace nerp
{
    Recognizer::Recognizer()
    {
        _features = new NERFeat;
        _probs = new Probability;
        _dict = new NERDict;
        _char_type = new CharType;
        _mp = new MultiPerceptron;
        _tagset = new Tagset;

        is_tagset_ready = false;
    }

    Recognizer::~Recognizer()
    {
        delete _features;
        delete _probs;
        delete _dict;
        delete _char_type;
        delete _mp;
    }

    bool Recognizer::Initialize()
    {
#ifdef WIN32
        string _datapath = "model\\";
#else
        string _datapath = "model/";
#endif
        bool is_char_bin = false;
        string dictfile = _datapath + "Dict";
        string featfile = _datapath + "Feat";
        string probfile = _datapath + "Prob";
        string mpfile = _datapath + "Model";
        string tagsetfile = _datapath + "Tagset";
        return Initialize(is_char_bin, dictfile, featfile, probfile, tagsetfile, mpfile);
    }

    bool Recognizer::Initialize(bool is_char_bin, string dictfile, string &featfile, string &probfile, string &tagsetfile, string &mpfile)
    {
        if(!_char_type->Initialize(is_char_bin))
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "Initialization failed!";
            cerr << "Can not initialize the CharType."<<endl;
            return false;
        }
        std::cout<<"Loading CharType finished." <<endl;

        if(!_dict->LoadDictFile(dictfile.c_str()))
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "Initialization failed!";
            cerr << "Can not initialize the NERDict."<<endl;
            return false;
        }
        std::cout<<"Loading NERDict finished." <<endl;

        if(!_features->LoadFeatureFile(featfile.c_str()))
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "Initialization failed!";
            cerr << "Can not initialize the NERFeat."<<endl;
            return false;
        }
        std::cout<<"Loading NERFeat finished." <<endl;

        if(!_probs->LoadProbFile(probfile.c_str()))
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "Initialization failed!";
            cerr << "Can not initialize the NER Probability."<<endl;
            return false;
        }
        std::cout<<"Loading NER Probability finished." <<endl;

        if(!_tagset->LoadTagsetFile(tagsetfile.c_str()))
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "Initialization failed!";
            cerr << "Can not initialize the NER Tagset."<<endl;
            return false;
        }
        std::cout<<"Loading NER Tagset finished." <<endl;
        is_tagset_ready = true;

        if(!_mp->read_model(mpfile))
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "Initialization failed!";
            cerr << "Can not initialize the MultiPerceptron."<<endl;
            return false;
        }
        is_initial = true;
        return true;
    }

    string Recognizer::GetTag(int index)
    {
        return _tagset->GetTag(index);
    }

    void Recognizer::ReadSentence(ifstream &fin, vector<string> &charVec) {
        if (!fin.is_open()) {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "On " << __FILE__ << ", line:" << __LINE__ << endl;
            return;
        }
        string myTextLine;
        charVec.clear();

        charVec.push_back("B_1");
        charVec.push_back("B_0");

        getline(fin, myTextLine);
        TrimLine(myTextLine);
        while(!fin.eof() && myTextLine.size()) {
            // vector<string> item = cwsp::string_split(myTextLine, " ");
            charVec.push_back(myTextLine);
            getline(fin, myTextLine);
            TrimLine(myTextLine);
            // std::cout<<myTextLine << endl;
        }

        charVec.push_back("E_0");
        charVec.push_back("E_1");
        return;
    }

    void Recognizer::RecogFile(const char * inputfile, const char * outputfile)
    {
        if (!is_initial)
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "On " << __FILE__ << ", line:" << __LINE__ << endl;
            cerr << "The Recognizer have not been initialized yet." <<endl;
            return;
        }
        clock_t begin, finish;
        begin = clock();

        ifstream fin;
        fin.open(inputfile);
        if (!fin.is_open())
        {
            cerr << "\nRecognizer ERROR" << endl;
            cerr << "On " << __FILE__ << ", line:" << __LINE__ << endl;
            cerr << "Cannot open file\"" << inputfile <<"\"\n";
            return;
        }
        else
        {
            ofstream fout;
            fout.open(outputfile);
            string myTextLine;
            getline(fin, myTextLine); // Skip the first line
            getline(fin, myTextLine); // Read the number of sentences
            int Length = fromString<int>(myTextLine);
            int numIndex = 0;
            vector<string> charVec;
            std::cout << "Processing . . . ." << endl;
            fout << "#Name Entity Recognition Perceptron" << endl;
            fout << Length << endl;
            while (numIndex < Length) {
                numIndex++;
                if (numIndex%100 == 0) {
                    std::cout << ".";
                    std::cout.flush();
                    if (numIndex%5000 == 0) {
                        float percent = float(numIndex) * 100.0 / float(Length);
                        std::cout << " " << percent << "\%" << endl;
                    }
                }
                ReadSentence(fin, charVec);
                string outputSen;
                RecogSentence(charVec, outputSen);
                // TrimLine(outputSen);
                fout << outputSen << endl;
            }
            fout.close();
            std::cout << endl;
            std::cout << numIndex << " sentences in total." << endl;
        }
        fin.close();

        finish = clock();
        std::cout << "It takes " << (double) (finish - begin) / CLOCKS_PER_SEC << " secs." << endl;
    }

    void Recognizer::RecogSentence(vector<string> myCharVec, string & outputSen)
    {
        // vector<string> myCharVec;
        // SplitLine(inputSen, myCharVec);
        vector<vector<string> > myFeats;
        GenerateFeats(myCharVec, myFeats);
        vector<vector<string> > myFeatsVec;
        Feature2vec(myFeats, myFeatsVec);
        vector<string> myTagVec;
        Viterbi(myFeatsVec, myTagVec);
        Tag2Word(myCharVec, myTagVec, outputSen);
        TrimLine(outputSen);
    }

    void Recognizer::Viterbi(vector<vector<string> > &myFeatsVec, vector<string> &tagVec)
    {
        size_t n = myFeatsVec.size();
        vector<vector<double> > toward;
        vector<vector<int> > back;

        for (size_t i=0;i<n;i++)
        {
            vector<double> a (4, -HUGE_VAL);
            toward.push_back(a);
            vector<int> b (4, -1);
            back.push_back(b);
        }

        vector<vector<double> > emit_prob;
        GetEmitProb(myFeatsVec, emit_prob);
        // run viterbi algorithm
        for (size_t i=0;i<4;i++)
        {
            toward.at(0).at(i) = _probs->GetInitProb(i) + emit_prob.at(0).at(i);
            back.at(0).at(i) = -2;
        }
        // toward algorithm to calculate each t's state
        for (size_t t=1;t<n;t++)
        {
            for (size_t d=0;d<4;d++)
            {
                double prob = -HUGE_VAL;
                double prob_max = -HUGE_VAL;
                int tag_max = 3; // the index of tag “S”
                for(size_t s=0;s<4;s++)
                {
                    prob = toward.at(t-1).at(s) + _probs->GetTransProb(s, d) + emit_prob.at(t).at(d);
                    if (prob > prob_max)
                    {
                        prob_max = prob;
                        tag_max = s;
                    }
                }
                toward.at(t).at(d) = prob_max;
                back.at(t).at(d) = tag_max;
            }
        }
        // backward algorithm to get the best tag sequence
        deque<int> taglist;
        size_t t = n - 1;
        double prob;
        double prob_max = -HUGE_VAL;
        int tag_max = 3;
        for(size_t i=0;i<4;i++)
        {
            prob = toward.at(t).at(i);
            if (prob > prob_max)
            {
                prob_max = prob;
                tag_max = i;
            }
        }
        taglist.push_back(tag_max);
        while(t>=1)
        {
            int preTag = back.at(t).at(taglist.front());
            taglist.push_front(preTag);
            --t;
        }
        // int tttt = (taglist.size()==n)?1:0;
        // std::cout<<"Check if the length of taglist equal to charVec:"<<tttt<<endl;
        tagVec.clear();
        for(size_t i=0;i<n;i++)
        {
            tagVec.push_back(GetTag(taglist.at(i)));
        }
        if(tagVec.back()=="B")
            tagVec.back() = "S";
        else if (tagVec.back() == "M")
            tagVec.back() = "E";
        return;
    }

    void Recognizer::GenerateFeats(vector<string> charVec, vector<vector<string> > &featsVec)
    {
        featsVec.clear();
        for (size_t i = 2; i<charVec.size()-2;i++)
        {
            vector<string> feat;
            string feature;

            // Pu(0) 0
            feature = toString(_char_type->GetPuncType(charVec.at(i)));
            feat.push_back(feature);

            // C-2 1
            feature = charVec.at(i-2);
            feat.push_back(feature);
            // C-1 2
            feature = charVec.at(i-1);
            feat.push_back(feature);
            // C0 3
            feature = charVec.at(i);
            feat.push_back(feature);
            // C1 4
            feature = charVec.at(i+1);
            feat.push_back(feature);
            // C2 5
            feature = charVec.at(i+2);
            feat.push_back(feature);

            // C-2C-1 6
            feature = charVec.at(i-2) + charVec.at(i-1);
            feat.push_back(feature);
            // C-1C0 7
            feature = charVec.at(i-1) + charVec.at(i);
            feat.push_back(feature);
            // C0C1 8
            feature = charVec.at(i) + charVec.at(i+1);
            feat.push_back(feature);
            // C1C2 9
            feature = charVec.at(i+1) + charVec.at(i+2);
            feat.push_back(feature);

            // C-1C1 10
            feature = charVec.at(i-1) + charVec.at(i+1);
            feat.push_back(feature);

            /* dict features */
            //get MWL, t0
            pair<int, string> ans = _dict->GetDictInfo(charVec.at(i).c_str());
            // MWL+t0 11
            feature = toString(ans.first) + ans.second;
            feat.push_back(feature);
            // C-1+t0 12
            feature = charVec.at(i-1) + ans.second;
            feat.push_back(feature);
            // C0+t0 13
            feature = charVec.at(i) + ans.second;
            feat.push_back(feature);
            // C1+t0 14
            feature = charVec.at(i+1) + ans.second;
            feat.push_back(feature);

            /* type features */
            //T(-1)T(0)T(1) 15
            int index;
            index = 1 + _char_type->GetCharType(charVec.at(i-1));
            index += 6 * _char_type->GetCharType(charVec.at(i));
            index += 36 * _char_type->GetCharType(charVec.at(i+1));
            feat.push_back(toString(index));

            //N(-1)N(0)N(1) 16
            index = 1 + _char_type->GetCNameType(charVec.at(i-1));
            index += 6 * _char_type->GetCNameType(charVec.at(i));
            index += 36 * _char_type->GetCNameType(charVec.at(i+1));
            feat.push_back(toString(index));

            //F(-1)F(0)F(1) 17
            index = 1 + _char_type->GetFNameType(charVec.at(i-1));
            index += 2 * _char_type->GetFNameType(charVec.at(i));
            index += 4 * _char_type->GetFNameType(charVec.at(i+1));
            feat.push_back(toString(index));

            featsVec.push_back(feat);
        }
    }

    void Recognizer::Feature2vec(vector<vector<string> > feats, vector<vector<string> > &featsVec)
    {
        int unigramlen = _features->UnigramLen();
        int bigramlen = _features->BigramLen();
        int trigramlen = _features->TrigramLen();
        int dictlen = _features->DictFeatLen();
        for(size_t i=0; i<feats.size();i++)
        {
            vector<string> featVec;
            if (feats.at(i).at(0) == "1")
            {
                featVec.push_back("0:1.0");
            }
            for(size_t j=1;j<feats.at(i).size();j++)
            {
                if (j<6)
                {
                    int index = _features->GetUnigramIndex(feats.at(i).at(j).c_str());
                    index += (j-1)*unigramlen;
                    featVec.push_back(toString(index) + ":1.0");
                }
                else if (j < 10)
                {
                    int index = _features->GetBigramIndex(feats.at(i).at(j).c_str());
                    index += 5 * unigramlen + (j-6) * bigramlen;
                    featVec.push_back(toString(index) + ":1.0");
                }
                else if (j < 11)
                {
                    int index = _features->GetTrigramIndex(feats.at(i).at(j).c_str());
                    index += 5 * unigramlen + 4 * bigramlen;
                    featVec.push_back(toString(index) + ":1.0");
                }
                else if (j < 15)
                {
                    int index = _features->GetDictIndex(feats.at(i).at(j).c_str());
                    index += 5 *unigramlen + 4*bigramlen + trigramlen + (j-11)*dictlen;
                    featVec.push_back(toString(index) + ":1.0");
                }
                else if (j==15)
                {
                    int index = fromString<int>(feats.at(i).at(j));
                    index += 5*unigramlen + 4*bigramlen + trigramlen + 4*dictlen;
                    featVec.push_back(toString(index) + ":1.0");
                }
                else if (j==16)
                {
                    int index = fromString<int>(feats.at(i).at(j));
                    index += 5*unigramlen + 4*bigramlen + trigramlen + 4*dictlen + TYPE_FEAT_SIZE;
                    featVec.push_back(toString(index) + ":1.0");
                }
                else if (j==17)
                {
                    int index = fromString<int>(feats.at(i).at(j));
                    index += 5*unigramlen + 4*bigramlen + trigramlen + 4*dictlen + TYPE_FEAT_SIZE + CNAME_FEAT_SIZE;
                    featVec.push_back(toString(index) + ":1.0");
                }
            }
            featsVec.push_back(featVec);
        }
        return ;
    }

    void Recognizer::GetEmitProb(vector<vector<string> > featsVec, vector<vector<double> > &emit_prob)
    {
        _mp->classify_samps_withprb(featsVec, emit_prob);
    }

    void Recognizer::Tag2Word(vector<string> charVec, vector<string> tagVec, string &line)
    {
        line.clear();
        string word;
        for(size_t i=0;i<tagVec.size();i++)
        {
            word = charVec.at(i+2);
            line += word + " " + tagVec.at(i) + '\n';
        }
        return;
    }
}