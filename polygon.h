#ifndef POLYGON_H
#define POLYGON_H

#include <utility>
#include <vector>

using std::vector;
using std::pair;

class polygon
{
public:
    polygon();
    ~polygon();

    vector <pair<double, double>> point;

    void add_point(double x, double y);
    bool search_for_point(double x, double y);
};

#endif // POLYGON_H
