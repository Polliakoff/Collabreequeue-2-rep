#ifndef PATHWAY_H
#define PATHWAY_H

#include "polygon.h"

class pathway : public polygon
{
public:
    pair<double, double> final_point;
    pair<double, double> start_point;
    pathway();
    ~pathway();
    void make_my_way();
    void generator();
};

#endif // PATHWAY_H
