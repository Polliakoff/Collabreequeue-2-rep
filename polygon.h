#ifndef POLYGON_H
#define POLYGON_H

#include "func.h"
#include "straight_line.h"
#include <vector>

using std::vector;
using std::pair;

class polygon
{
public:
    polygon();
    ~polygon();

    vector <pair<double, double>> vertexes;
    vector <straight_line> faces;

    void add_point(double x, double y);
};

#endif // POLYGON_H
