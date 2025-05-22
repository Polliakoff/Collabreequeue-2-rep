#pragma once

#include <cmath>
#include <Eigen/Dense>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <QDataStream>
#include <memory>
#include <algorithm>
#include <vector>
#include <random>
#include <tuple>
//https://www.qt.io/blog/2018/05/31/serialization-in-and-with-qt
//https://github.com/mauricek/qt_iot_blog_samples/blob/master/part2/serialization/sensorinformation.cpp#L188

class brain
{
protected:
    double sigmoid(const double&);
    void inheritWeights(brain &a, brain &b, double dmnc);
    double round_to_one(const double& subject);
public:
    int S;                              //всего слоёв
    std::vector<int> l;                 //вершины слоёв, задаёт размеры следующим матрицам
    std::vector<Eigen::RowVectorXd> A;  //Значения нейронов в слоях
    std::vector<Eigen::MatrixXd> W;     //S-1 матриц весов - связей нейронов между слоями
    /*  ArrayXX<char> в Eigen не алиасирован, объявляем явно */
    using Mask = Eigen::Array<char,Eigen::Dynamic,Eigen::Dynamic>;
    std::vector<Mask> E;                    // маски того же размера, что и W


    int first=7;    //количество входных
    int last=2;     //количество выходных

    brain();
    brain(brain &a, brain &b, double dominance);                //новая нейросеть на основе старых,
    ~brain(){}                                                  //параметр dmnc отвечает за превосходство первого над вторым
                                                                //и должна принимать значение от 0.00(копирование второй сети)
                                                                //до 1.00(копирование первой сети)
    void think();       //пересчитывает значения
    void noiseWeights();//шумит веса
    /* --- структурные мутации «в стиле NEAT» --- */
    void toggleConnection(std::size_t layer,
                          std::size_t src, std::size_t dst, bool enable);

    void addRandomConnection();             // mutation: add-connection
    void splitConnection(std::size_t layer, // mutation: add-node
                         std::size_t src, std::size_t dst);

    /* --- новые операторы --- */
    void disableRandomConnection();               // mutation: del-connection
    void pruneIsolatedNeurons();                  // «чистка» мёртвых нейронов
    void insertHiddenLayer(std::size_t pos,
                           int neurons = 4);      // mutation: add-layer
    void removeHiddenLayer(std::size_t pos);      // mutation: del-layer

    /* вероятность вызова этих мутаций решит ваш GA */

    static int ID;
};

QDataStream &operator<<(QDataStream &, const brain &);
QDataStream &operator>>(QDataStream &, brain &);
