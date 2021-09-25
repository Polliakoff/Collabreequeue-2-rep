#pragma once

#include <vector>
#include <ctime>
#include <cmath>
#include <windows.h>
#include <QRandomGenerator>

class neuron
{
private:
    double value;

public:
    neuron();
    neuron(double _value);
    ~neuron();

    double get_value();
    void set_value(double x);
    void calculate_sigmoid(double x);
    neuron& operator= (const neuron &str);
    friend neuron operator+(neuron me, const double &a);

};



struct matrix_weight{//w00*a0 w01*a1
private:
        double gen_temp;
public:
    std::vector<std::vector<double>> weight;

    matrix_weight(); //не использовать
    matrix_weight(int n, int m);
    ~matrix_weight();

    std::vector<double> count(std::vector<neuron> a);
    std::vector<double> count(std::vector<neuron> a, std::vector<double> b);
};


