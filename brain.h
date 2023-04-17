#pragma once

#include <cmath>
#include <Eigen/Dense>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <QDataStream>
#include "pattern.h"
#include <memory>
//https://www.qt.io/blog/2018/05/31/serialization-in-and-with-qt
//https://github.com/mauricek/qt_iot_blog_samples/blob/master/part2/serialization/sensorinformation.cpp#L188

class brain
{
protected:
    double sigmoid(const double&);
    double diff_sigmoid(const double&);
//    double sigmoid_distance(const double&);
    void inheritWeights(brain &a, brain &b, double dmnc);
    Eigen::RowVectorXd false_prophet(std::shared_ptr<Eigen::RowVectorXd>);
    double round_to_one(const double& subject);
    std::vector<std::shared_ptr<Eigen::RowVectorXd>> memory;
public:
    pattern behavior;
    int S;                              //всего слоёв
    std::vector<int> l;                 //вершин внутренних слоёв, задаёт размеры следующим матрицам
    std::vector<Eigen::RowVectorXd> A;  //S+2 всего слоёв (размер 1хl[n])
    std::vector<Eigen::RowVectorXd> learning_A;
    std::vector<Eigen::MatrixXd> W;     //S+1 матриц весов (размер l[n]хl[n+1])

    ///Обратная связь
//    int first=14;   //количество входных слоёв

    int first=7;

//    int last=4;     //количество выходных когда чатыре

    int last=2;     //количество выходных

    brain();
    brain(brain &a, brain &b, double dominance);                //новая нейросеть на основе старых,
    ~brain(){}                                                  //параметр dmnc отвечает за превосходство первого над вторым
                                                                //и должна принимать значение от 0.00(копирование второй сети)
                                                                //до 1.00(копирование первой сети)
    bool viable();      //проверяет сеть на вменяемость
    void think();       //пересчитывает значения
    void think(std::shared_ptr<Eigen::RowVectorXd>);
    void mutate();      //мутирует с некоторой вероятностью
    void noiseWeights();//шумит веса
    void learn();
    static int ID;
};


QDataStream &operator<<(QDataStream &, const brain &);
QDataStream &operator>>(QDataStream &, brain &);
