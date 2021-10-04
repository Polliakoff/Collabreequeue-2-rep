#pragma once

#include "func.h"

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


