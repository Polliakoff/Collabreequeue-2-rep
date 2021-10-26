//#ifndef SHIP_H
//#define SHIP_H
#pragma once

#include <vector>
#include "straight_line.h"
#include "polygon.h"
#include "brain.h"


class ship
{
protected:
    std::pair<double, double> position;
public:
    ship();//не использовать
    ship(const double& pos_x,const double& pos_y); // задавать корабль исключительно этим конструктором с установленной позицией
    ~ship();

    bool collided = false;

    double angle;
    polygon body;
    std::vector<straight_line> eyes;
    std::vector<double> distances;
    std::vector<std::pair<double,double>> point_seen;

    std::pair<double, double> convert_to_ship(const std::pair<double, double>& point);
    std::pair<double, double> get_position();
    void move_by_coords(const double& new_pos_x,const double& new_pos_y);
    void move_by_distance(const double& distance);
    double get_angle();
    void rotate_by(const double& new_angle);
    bool collision(polygon &pol);
    void eyesight(polygon &pol);
public slots:
    virtual void update(polygon &map);

};

std::pair<double, double> point_rotation(const std::pair<double, double>& point, const std::pair<double, double>& axis,
                                         const double &delta_angle);
bool point_to_poly(const std::pair<double,double>& point, polygon &pol);
//#endif // SHIP_H
