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
public:
    brain();

    void think();
    int S;                              //внутренних слоёв
    std::vector<int> l;                 //вершин внутренних слоёв, задаёт размеры следующим матрицам
    std::vector<Eigen::RowVectorXd> A;  //S+2 всего слоёв (размер 1хl[n])
    std::vector<Eigen::MatrixXd> W;     //S+1 матриц весов (размер l[n]хl[n+1])
};

std::ofstream &operator<<(std::ofstream& fout, brain& b);
QDataStream &operator<<(QDataStream &, const brain &);
QDataStream &operator>>(QDataStream &, brain &);
