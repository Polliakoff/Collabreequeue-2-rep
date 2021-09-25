#include "neuron.h"
#include <ctime>


using namespace std;

neuron::neuron(){ //создание случайного нейрона
    this->value = (rand()%100)/100.0; //первая случайное задание значения нейрона
}

neuron::neuron(double _value): value(_value) { //создание заданного нейрона
    return;
}

void neuron::calculate_sigmoid(double x){ //задаем значение сигмоидой, меняем нейрон
    value=1/(1+exp(-x));
}

double neuron::get_value() { //получить значение нейрона
    return value;
}

void neuron::set_value(double x){
    value=x;
}

neuron::~neuron() {
}

//переопределяем копирование
neuron& neuron::operator= (const neuron &drob){
    // Выполняем копирование значений
    this->value = drob.value;

    // Возвращаем текущий объект
    return *this;
}

using namespace std;

matrix_weight::matrix_weight(){
    vector<double> n = {1, 0, 0};
    weight.emplace_back(n);
    n = {0, 1, 0};
    weight.emplace_back(n);
    n = {0, 0, 1};
    weight.emplace_back(n);
}

matrix_weight::matrix_weight(int n, int m){
    srand(time(0));
    for (int i=0; i<n; i++){
        vector<double> temp; //создаем вектор столбец
        for (int j=0; j<m; j++){

            //            temp.emplace_back((rand()%100-50)/50.0); //наполянем его

            gen_temp = ((QRandomGenerator::global()->bounded(100)) - 50.0)/50.0;
            temp.emplace_back(gen_temp);

        }
        this->weight.emplace_back(temp); //вставляем его
    }
    return;
}


matrix_weight::~matrix_weight(){
    return;
}

std::vector<double> matrix_weight::count(std::vector<neuron> a){
    std::vector<double> tmp(this->weight.size(), 0.0);
    for (size_t i = 0; i < this->weight.size(); i++)
        for (size_t j = 0; j < this->weight[0].size(); j++){
            tmp[i] = tmp[i] + this->weight[i][j]*a[j].get_value();
        }
    return tmp;
}

std::vector<double> matrix_weight::count(std::vector<neuron> a, std::vector<double> b){
    std::vector<double> tmp(this->weight.size(), 0.0);
    for (size_t i = 0; i < this->weight.size(); i++)
        for (size_t j = 0; j < this->weight[0].size(); j++){
            tmp[i] = tmp[i] + this->weight[i][j]*a[j].get_value() + b[i];
        }
    return tmp;
}


//переопределяем сложение
neuron operator+(neuron me, const double &a){
    return me.value+a;
}


