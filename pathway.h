#ifndef PATHWAY_H
#define PATHWAY_H

#include "polygon.h"

class pathway : public polygon
{
public:
    pathway();
    ~pathway();
    void make_my_way();
    void generator();
};

#endif // PATHWAY_H
