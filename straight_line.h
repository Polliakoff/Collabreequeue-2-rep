#ifndef STRAIGHT_LINE_H
#define STRAIGHT_LINE_H

#include <cmath>

class straight_line
{
public:
    straight_line();
    straight_line(const double& k_input, const double& b_input);
    straight_line(const double& x1, const double& y1, const double& x2, const double& y2);


    ~straight_line();

    double k = 0;
    double b = 0;
    void rotate(const double& angle);
};

#endif // STRAIGHT_LINE_H
