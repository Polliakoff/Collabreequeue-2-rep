#include "straight_line.h"

straight_line::straight_line(){}

straight_line::straight_line(const double &x1, const double &y1, const double &x2, const double &y2)
{
    direction(0) = x2-x1;
    direction(1) = y2-y1;
    direction(2) = 1;

    point(0) = x1;
    point(1) = y1;
    point(2) = 1;

    rotation_point = point;
}



straight_line::~straight_line()
{

}

void straight_line::rotate(const double &angle)
{
    Eigen::Matrix3d rotation;
    double cs = cos(angle);
    double sn = sin(angle);

    rotation(0,0) = cs;
    rotation(0,1) = sn;
    rotation(0,2) = 0;
    rotation(1,0) = -sn;
    rotation(1,1) = cs;
    rotation(1,2) = 0;
    rotation(2,0) = -rotation_point(0)*(cs-1)+rotation_point(1)*sn;
    rotation(2,1) = -rotation_point(1)*(cs-1)-rotation_point(0)*sn;
    rotation(2,2) = 1;

    direction = rotation*direction;
}

void straight_line::move_by(const double &x, const double &y)
{
    point(0)+=x;
    point(1)+=y;
}

double straight_line::get_x(const double &y)
{
    return ((y-point(1))/direction(1)*direction(0)+point(0));
}

double straight_line::get_y(const double &x)
{
    return ((x-point(0))/direction(0)*direction(1)+point(1));
}



