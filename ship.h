#ifndef SHIP_H
#define SHIP_H

#include "func.h"
#include "straight_line.h"
#include "polygon.h"
#include "brain.h"

class ship
{
private:
    double angle;
    pair<double, double> position;
public:
    ship();//не использовать
    ship(const double& pos_x,const double& pos_y); // задавать корабль исключительно этим конструктором с установленной позицией
    ~ship();

    polygon body;
    vector<straight_line> eyes;

    pair<double, double> convert_to_ship(const pair<double, double>& point);
    pair<double, double> get_position();
    void move_by(const double& new_pos_x,const double& new_pos_y);
    double get_angle();
    void rotate_by(const double& new_angle);
};

pair<double, double> point_rotation(const pair<double, double>& point, const pair<double, double>& axis, const double &delta_angle);

#endif // SHIP_H
