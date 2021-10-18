#pragma once

#include <cmath>
#include <Eigen/Dense>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <QDataStream>
//https://www.qt.io/blog/2018/05/31/serialization-in-and-with-qt
//https://github.com/mauricek/qt_iot_blog_samples/blob/master/part2/serialization/sensorinformation.cpp#L188

class brain
{
private:
    double sigmoid(const double&);
    int first=13;   //количество входных слоёв
    int last=4;     //количество выходных
    void inheritWeights(brain &a, brain &b, double dmnc);
public:
    int S;                              //внутренних слоёв
    std::vector<int> l;                 //вершин внутренних слоёв, задаёт размеры следующим матрицам
    std::vector<Eigen::RowVectorXd> A;  //S+2 всего слоёв (размер 1хl[n])
    std::vector<Eigen::MatrixXd> W;     //S+1 матриц весов (размер l[n]хl[n+1])



    brain();
    brain(brain &a, brain &b, double dominance);                //новая нейросеть на основе старых,
                                                                //параметр dmnc отвечает за превосходство первого над вторым
                                                                //и должна принимать значение от 0.00(копирование второй сети)
                                                                //до 1.00(копирование первой сети)
    bool viable();      //проверяет сеть на вменяемость
    void think();       //пересчитывает значения
    void mutate();      //мутирует с некоторой вероятностью
};


QDataStream &operator<<(QDataStream &, const brain &);
QDataStream &operator>>(QDataStream &, brain &);
