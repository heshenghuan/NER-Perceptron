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
#include "NERDict.h"
#include "Tagset.h"
#include "Vocabulary.h"
#include "Probability.h"
#include "Pretreatment.h"
#include "MultiPerceptron.h"
#include "Config.h"
#include <string>
using namespace std;

#define LINE_LEN_MAX 256

void print_help() {
    cout << nerp::g_copyright << nerp::g_version
        << "\n\nNAME\n"
        << "    nerp_train -- Training a NER-Perceptron model using the given corpus file.\n"
        << "USAGE:\n"
        << "    nerp_train [-abr 0|1] [-dt file] [-n INT] [-ml FLOAT] corpus model\n"
        << "OPTIONS:\n"
        << "     -h    Print help infomation\n\n"
        << "     Following command can be used to specify resources files.\n"
        << "     -b [0,1]    0: CharType resource file is a text file\n"
        << "                 1: CharType resource file is a binary file (default)\n\n"
        << "     -d <dict>   Specify the dictionary file (default: model\\Dict)\n\n"
        << "     -t <tagset> Specify the tagset file (default: model\\Tagset)\n\n"
        // << "        -f <feat>     -> Feature file (default: model\\Feat.bin)\n"
        // << "        -p <prob>     -> Probability file (default: model\\Prob.bin)\n"
        << "     Following command can be used to specify the training params.\n"
        << "     -n int       Set the maximum iteration loops (default 200)\n\n"
        << "     -m double    Set minimum loss value decrease (default 1e-03)\n\n"
        << "     -r [0,1]     Choosing optimization method\n"
        << "                  0: Stochastic gradient descent optimization(default)\n"
        << "                  1: Gradient Descent optimization\n\n"     
        << "     -l float     Set the learning rate (default 1.0)\n\n"
        << "     -a [0,1]     Set weight matrix output way\n"
        << "                  0: final weight (default)\n"
        << "                  1: average weights of all iteration loops\n\n"
        // << "        -u [0,1]      -> 0: initial training model (default)\n"
        // << "                      -> 1: updating model (pre_model_file is needed)\n" 
        << endl;
}

void read_parameters(int argc, char *argv[], char *corpus_file, char *model_file, 
                        size_t *max_loop, double *loss_thrd, float *learn_rate, int *optim, 
                        int *avg, bool &is_bin, string &dictfile, string &tagsetfile) {
    // set default options
#ifdef WIN32
    string _modelpath = "model\\";
#else
    string _modelpath = "model/";
#endif
    is_bin = true;
    dictfile = _modelpath + "Dict";
    tagsetfile = _modelpath + "Tagset";

    *max_loop = 200;
    *loss_thrd = 1e-3;
    *learn_rate = 1.0;
    *optim = 0;
    *avg = 0;
    // *update = 0;
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
            case 't':
                tagsetfile.clear();
                tagsetfile = string(argv[++i]);
                break;
            case 'h':
                print_help();
                exit(0);
            case 'n':
                *max_loop = size_t(atoi(argv[++i]));
                break;
            case 'm':
                *loss_thrd = atof(argv[++i]);
                break;
            case 'l':
                *learn_rate = (float)atof(argv[++i]);
                break;
            case 'r':
                *optim = atoi(argv[++i]);
                break;
            case 'a':
                *avg = atoi(argv[++i]);
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
    strcpy (corpus_file, argv[i]);
    strcpy (model_file, argv[i+1]);
}

int nerp_train(int argc, char *argv[])
{
    char corpus_file[LINE_LEN_MAX];
    char model_file[LINE_LEN_MAX];
    size_t max_loop;
    double loss_thrd;
    float learn_rate;
    int optim;
    int avg;
    bool is_bin; 
    string dictfile;
    string tagsetfile;
    // string trainfile;
    read_parameters(argc, argv, corpus_file, model_file, &max_loop, &loss_thrd, &learn_rate, &optim, &avg, is_bin, dictfile, tagsetfile);

    string corpus = string(corpus_file);
    string model = string(model_file);
    cout << nerp::g_copyright<<endl;
    nerp::Pretreatment t;

    if (t.LoadCharFile(is_bin))
    {
        cout << "Load Character type file finished."<<endl;
    }
    else
    {
        cout << "ERROR! Trouble with resource file, please check if the file\n";
        cout << "is exist or contact to the Author.\n";
        exit(0);
    }

    if (t.LoadDictFile(dictfile.c_str()))
    {
        cout << "Load Dictionary finished." << endl;
    }
    else
    {
        cout << "ERROR! Trouble with resource file, please check if the file\n";
        cout << "is exist or contact to the Author.\n";
        exit(0);
    }

    if (t.LoadTagetFile(tagsetfile.c_str()))
    {
        cout << "Load Tagset finished." << endl;
    }
    else
    {
        cout << "ERROR! Trouble with resource file, please check if the file\n";
        cout << "is exist or contact to the Author.\n";
        exit(0);
    }
    t.Sampling(true);

    cout << "\nReading corpus file..."<<endl;
    t.TrainNERFile(corpus.c_str()); // TrainSegFile will save Prob & Feat.
    t.PrintInfo();

    vector<nerp::feature> feats_vec;
    vector<int> class_vec;

    cout<<"\nMaking train data..."<<endl;
    // t.MakeTrainData(corpus.c_str(), trainfile.c_str());
    t.MakeTrainData(corpus.c_str(), feats_vec, class_vec);
    cout<<"Pretreatment Finished."<<endl;

    cout<<"\n********************Train********************\n"<<endl;
    nerp::MultiPerceptron mp;
    // if (!mp.load_training_file(trainfile))
    if (!mp.load_training_data(feats_vec, class_vec))
    {
        cout << "\nERROR! Trouble with train file, please check if the file\n";
        cout << "is exist or contact to the Author.\n";
        exit(0);
    }
    mp.init_omega();
    if (optim) {
        mp.train_batch(max_loop, loss_thrd, learn_rate, avg);       
    }
    else {
        mp.train_SGD(max_loop, loss_thrd, learn_rate, avg);
    }
    if(!mp.save_model(model)) //mp.save_bin_model(model)
    {
        exit(0);
    }
    return 1;
}

int main(int argc, char *argv[])
{
    return nerp_train(argc, argv);
}