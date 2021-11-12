#ifndef PATHWAY_H
#define PATHWAY_H

#include "polygon.h"
#include <QRandomGenerator>

class pathway : public polygon
{
public:
    pair<double, double> final_point;
    pair<double, double> start_point;
    vector <double> glacier_x;
    vector <double> glacier_y;
    pathway();
    ~pathway();
    int  l_count = 0, r_count = 0, main_count = 0;
    bool generated = false;
    void make_my_way();
    void generator();
};

#endif // PATHWAY_H
