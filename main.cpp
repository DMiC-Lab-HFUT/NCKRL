#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include "Config.h"
#include "DataModel.h"
#include "Train.h"
#include "Test.h"

using namespace std;

void usage_message() {
    cout << "command line options:";
    cout << "\n-e: training epochs as: -e number, default is set to 1000";
    cout << "\n-d: dimension of entities and relations as: -d number, default is set to 50";
    cout << "\n-r: margin as: -r number, default is set to 1";
    cout << "\n-l: learning rate as: -l decimal, default is set to 0.001";
    cout << "\n-m: method as: -m [bern|unif], default is set to bern";
    cout << "\n-s: data set as: -s [FB15K|WN18], default is set to FB15K";
    cout << "\n-n: noise rate as: -n [10|20|40], default is set to 10%";
    cout << "\n-c: number of negative candidates as: -c number, default is set to 20\n\n";
}

int arg_pos(char *str, int argc, char **argv) {
    int i;
    for (i = 1; i < argc; i++)
        if (!strcmp(str, argv[i])) {
            if (i == argc - 1) {
                cerr << "argument missing for " << str << endl;
                exit(1);
            }
            return i;
        }
    return -1;
}

int main(int argc, char **argv) {
    srand((unsigned) time(nullptr));

    //default parameters
    unsigned epochs = 1000;
    unsigned dimension = 50;
    double margin = 1;
    double l_rate = 0.001;
    string method = "bern";
    string data_set = "FB15K";
    string noise_rate = "10";
    int ng_num = 20;
    bool l1_flg = true;

    // 命令行界面可选参数
    if ( argc > 1) {
        int i;
        if (argc == 2 && !strcmp("-h",argv[1])) {
            usage_message();
            exit(1);
        }
        if ((i = arg_pos((char *)"-e", argc, argv)) > 0)
            epochs = (unsigned)strtol(argv[i + 1], nullptr, 10);
        if ((i = arg_pos((char *)"-d", argc, argv)) > 0)
            dimension = (unsigned)strtol(argv[i + 1], nullptr, 10);
        if ((i = arg_pos((char *)"-r", argc, argv)) > 0)
            margin = strtod(argv[i + 1], nullptr);
        if ((i = arg_pos((char *)"-l", argc, argv)) > 0)
            l_rate = strtod(argv[i + 1], nullptr);
        if ((i = arg_pos((char *)"-m", argc, argv)) > 0)
            method = argv[i + 1];
        if ((i = arg_pos((char *)"-s", argc, argv)) > 0)
            data_set = argv[i + 1];
        if ((i = arg_pos((char *)"-n", argc, argv)) > 0)
            noise_rate = argv[i + 1];
        if ((i = arg_pos((char *)"-c", argc, argv)) > 0)
            ng_num = (unsigned)strtol(argv[i + 1], nullptr, 10);
    }

    // 数据集和参数配置
    Data data("./data/"+data_set+"/",
              "./results/"+data_set+"/","train.txt",
              "train_n"+noise_rate+".txt","valid.txt",
              "test.txt","entity2id.txt","relation2id.txt");
    Parameter params(epochs,dimension,l_rate,margin,
            method,noise_rate,ng_num,l1_flg,true);


    //output
    ofstream of;
    of.open((data.report_dir+params.report).c_str());
    of << "data set: " << data_set << endl;
    of << "noise rate: " << noise_rate << "%" << endl;
    of << "epochs: " << epochs << endl;
    of << "dimension: " << dimension << endl;
    of << "learning rate: " << l_rate << endl;
    of << "candidates number: " << ng_num << endl;
    of << "margin: " << margin << endl;
    of << "method: " << method << endl;
    of.close();

    cout << "preparing..." << endl;
    // 读入数据
    DataModel data_model(data, params);
    data_model.prepare();
    cout << "preparing accomplished. \n\nstart training..." << endl;

    // 训练开始
    Train training(data,params,data_model.relation_num
            ,data_model.entity_num,data_model.data_train
            ,data_model.train_flg
            ,data_model.left_num,data_model.right_num);
    training.run();
    cout << "training finished. \n\nstart testing..." << endl;

    // 测试开始
    Test testing(data,params,data_model.relation_num
            ,data_model.entity_num,data_model.data_test
            ,data_model.all_flg);
    testing.link_prediction();
    cout << "testing finished." << endl;

    return 0;
}
