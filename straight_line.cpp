#include "straight_line.h"

straight_line::straight_line(){}

straight_line::straight_line(const double &k_input, const double &b_input)
{
    k = k_input;
    b = b_input;
}

straight_line::straight_line(const double &x1, const double &y1, const double &x2, const double &y2)
{
  k = (y1-y2)/(x1-x2);
  b = y2-k*x2;
}

straight_line::~straight_line()
{

}

void straight_line::rotate(const double& angle)
{
    double x1 = 1;
    double x2 = 2;
    double y1 = k*x1+b;
    double y2 = k*x2+b;

    double x1_1 = x1*cos(angle) + y1*sin(angle);
    double y1_1 = x1*(-1)*sin(angle) + y1*cos(angle);

    double x2_1 = x2*cos(angle) + y2*sin(angle);
    double y2_1 = x2*(-1)*sin(angle) + y2*cos(angle);

    k = (y1_1-y2_1)/(x1_1-x2_1);
    b = y2_1-k*x2_1;
}

