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
#include "Config.h"
#include <string>
using namespace std;

void print_help() {
    cout << nerp::g_copyright << nerp::g_version
        << "\n\nNAME\n"
        << "    nerp_recog -- Using existing resource files to Named Entity recognition work.\n"
        << "USAGE:\n"
        << "    nerp_recog [-b 0|1] [-mdfpt file] corpus output\n"
        << "OPTIONS:\n"
        << "     -h    Print help infomation\n\n"
        << "     Following command can be used to specify resources files.\n"
        << "     -b [0,1]    0: CharType resource file is a text file\n"
        << "                 1: CharType resource file is a binary file (default)\n\n"
        << "     -m <model>   Specify the model file (default: model\\Model)\n\n"
        << "     -d <dict>   Specify the dictionary file (default: model\\Dict)\n\n"
        << "     -f <feat>   Specify the feature file (default: model\\Feat)\n\n"
        << "     -p <prob>   Specify the probability file (default: model\\Prob)\n\n"
        << "     -t <tagset> Specify the tagset file (default: model\\Tagset)\n\n"
        << endl;
}

void read_parameters(int argc, char *argv[], string &model_file, string &corpus_file, string &reced_file,
                        bool &is_bin, string &dictfile, string &featfile, string &probfile, string &tagsetfile)
{
    // set default options
#ifdef WIN32
    string _modelpath = "model\\";
    // string _datapath = "data\\";
#else
    string _modelpath = "model/";
    // string _datapath = "data/";
#endif
    is_bin = true;
    dictfile = _modelpath + "Dict";
    featfile = _modelpath + "Feat";
    probfile = _modelpath + "Prob";
    tagsetfile = _modelpath + "Tagset";
    model_file = _modelpath + "Model";

    int i;
    for (i = 1; (i<argc) && (argv[i])[0]=='-'; i++) {
        switch ((argv[i])[1]) {
            case 'b':
                is_bin = atoi(argv[++i])?true:false;
                break;
            case 'd':
                dictfile.clear();
                dictfile = string(argv[++i]);
                break;
            case 'f':
                featfile.clear();
                featfile = string(argv[++i]);
                break;
            case 'h':
                print_help();
                exit(0);
            case 'p':
                probfile.clear();
                probfile = string(argv[++i]);
                break;
            case 't':
                tagsetfile.clear();
                tagsetfile = string(argv[++i]);
                break;
            case 'm':
                model_file.clear();
                model_file = string(argv[++i]);
                break;
            default:
                cout << "Unrecognized option: " << argv[i] << "!" << endl;
                print_help();
                exit(0);
        }
    }
    
    if ((i+1)>=argc) {
        cout << "Not enough parameters!" << endl;
        print_help();
        exit(0);
    }
    // model_file = string(argv[i]);
    corpus_file = string(argv[i]);
    reced_file = string(argv[i+1]);
}

int nerp_recog(int argc, char *argv[])
{
    string corpus_file;
    string model_file;
    string reced_file;
    bool is_bin; 
    string dictfile;
    string featfile;
    string probfile;
    string tagsetfile;
    read_parameters(argc, argv, model_file, corpus_file, reced_file, is_bin, dictfile, featfile, probfile, tagsetfile);

    cout << nerp::g_copyright<<endl;
    nerp::Recognizer t;
    if (!t.Initialize(is_bin, dictfile, featfile, probfile, tagsetfile, model_file))
    {
        exit(0);
    }

    t.RecogFile(corpus_file.c_str(), reced_file.c_str());
    return 1;
}

int main(int argc, char *argv[])
{
    return nerp_recog(argc, argv);
}