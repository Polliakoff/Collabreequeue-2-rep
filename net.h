#pragma once

#include "neuron.h"
#include <cassert>
#include <windows.h>


class net{
private:
    std::vector<neuron> l0; //входные данные
    std::vector<neuron> l1; //нечто
    std::vector<neuron> l2; //выходные данные


public:
    matrix_weight w01; //веса 01
    matrix_weight w12; //веса 12
    std::vector<double> b1; //базис сдвига активации L1
    std::vector<double> b2; //базис сдвига активации L2

    net();
    net(int _l0, int _l1, int _l2); //кол-во входных, кол-во средних, кол-во выходных
    ~net();

    void count_w01();
    void count_w12();
    void make_new_weight01();
    void make_new_weight12();

    const std::vector<neuron>& get_out(); //вывод вектора нейронов
    void get_in(const std::vector<double>& parametrs); //ввод информации

    std::vector<neuron> get_in_out(const std::vector<double>& parametrs); //получает значения и выводит свои мысли на этот счет
};


