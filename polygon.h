#ifndef POLYGON_H
#define POLYGON_H

#include "func.h"
#include "straight_line.h"
#include <vector>



class polygon
{
public:
    polygon();
    ~polygon();

    std::vector <std::pair<double, double>> vertexes;
    std::vector <straight_line> faces;

    void add_point(double x, double y);
};

#endif // POLYGON_H
