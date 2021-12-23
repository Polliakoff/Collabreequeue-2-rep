#ifndef STRAIGHT_LINE_H
#define STRAIGHT_LINE_H

#include "func.h"
#include <Eigen/Dense>
#include <cmath>
#include <QtCore/qglobal.h>


class straight_line
{
public:
    straight_line();
    straight_line(const double& x1, const double& y1, const double& x2, const double& y2);

    ~straight_line();

    Eigen::Vector3d direction;
    Eigen::Vector3d point;
    Eigen::Vector3d rotation_point;
    void rotate(const double& angle);
    void move_by(const double& x, const double& y);
    double get_x(const double& y);
    double get_y(const double& x);
};

std::pair<double, double> intersect(straight_line& line_1,straight_line& line_2);
std::pair<double, double> intersect(straight_line &line_1, straight_line &line_2, std::pair<double,double> range_1, std::pair<double,double> range_2);
std::pair<double, double> perp_vect(std::pair<double, double> input_vector, const double& desired_length);

#endif // STRAIGHT_LINE_H
