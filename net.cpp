#include "net.h"
#include <ctime>

net::net(){
    for(int i = 0; i<6; i++){ //6 нейронов на входе по умолчанию
        l0.emplace_back(neuron());
    }
    for(int i = 0; i<6; i++){ //6 промежуточных нейрона
        l1.emplace_back(neuron());
    }
    for(int i = 0; i<6; i++){ //6 выходных нейрона
        l2.emplace_back(neuron());
    }
    w01 = matrix_weight(6,6);
    w12 = matrix_weight(6,6);

    std::vector<double> a(6,0);
    b1 = a; //сдвиг равен нулю по умолчанию
    std::vector<double> b(6,0);
    b2 = b;

    count_w01();
    count_w12();
}

net::net(int _l0, int _l1, int _l2){ //кол-во входных, кол-во средних, кол-во выходных
    for(int i = 0; i<_l0; i++){
        l0.emplace_back(neuron());
    }
    for(int i = 0; i<_l1; i++){
        l1.emplace_back(neuron());
    }
    for(int i = 0; i<_l2; i++){
        l2.emplace_back(neuron());
    }

    w01 = matrix_weight(_l1,_l0);

    w12 = matrix_weight(_l2,_l1);

    std::vector<double> a(l1.size(),0);
    b1 = a; //сдвиг равен нулю по умолчанию
    std::vector<double> b(l2.size(),0);
    b2 = a;

    count_w01();
    count_w12();

    //l2 = w12.count(l1,b2);
}

void net::count_w01(){ //подсчет матрицы весов на нейроны и передача сигмоиде
    std::vector<double> tmp = w01.count(l0,b1);
    for (size_t i=0; i<l1.size(); i++){
        l1[i].calculate_sigmoid(tmp[i]);
    }
}

void net::count_w12(){//подсчет матрицы весов на нейроны и передача сигмоиде
    std::vector<double> tmp = w12.count(l1,b2);
    for (size_t i=0; i<l2.size(); i++){
        l2[i].calculate_sigmoid(tmp[i]);
    }
}

void net::make_new_weight01(){ //выбираем случайный вес
    size_t n = rand()%l1.size();
    size_t m = rand()%l0.size();
    w01.weight[n][m]=w01.weight[n][m]+(rand()%50-rand()%50)/100.0;
}

void net::make_new_weight12(){ //выбираем случайный вес
    size_t n = rand()%l2.size();
    size_t m = rand()%l1.size();
    w12.weight[n][m]=w12.weight[n][m]+(rand()%50-rand()%50)/100.0;
}

const std::vector<neuron>& net::get_out(){
    return l2;
}

void net::get_in(const std::vector<double>& parametrs){//может быть долго, хз
    assert(parametrs.size()==l0.size());
    for(size_t i = 0; i < l0.size(); i++){
        l0[i].calculate_sigmoid(parametrs[i]);
    }
    return;
}


std::vector<neuron> net::get_in_out(const std::vector<double>& parametrs){ //получает значения и выводит свои мысли на этот счет
    get_in(parametrs);
    count_w01();
    count_w12();
    return get_out();
}

net::~net(){
    return;
}


