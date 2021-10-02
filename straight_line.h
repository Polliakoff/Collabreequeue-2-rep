#ifndef STRAIGHT_LINE_H
#define STRAIGHT_LINE_H

#include <cmath>
#include <Eigen/Dense>

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

#endif // STRAIGHT_LINE_H
