//#ifndef SHIP_PHYSICS_H
//#define SHIP_PHYSICS_H
#pragma once

#include "ship.h"
#include <cmath>
#include "brain.h"

class ship_physics : public ship
{
protected:
    pair<double, double> final_destination;
    brain net;
public:
    ship_physics();
    ship_physics(const double& pos_x,const double& pos_y,const double& dest_x, const double& dest_y); // задавать корабль исключительно этим конструктором с установленной позицией
    double velocity_x = 0, velocity_y = 0, abs_velocity;
    double angular_velocity = 0;
    double avg_velocity = 0;
    void engine(const int &mode);
    void helm(const int &mode2);
    void brainstorm();
    void friction();
    void change_destination(const double& dest_x,const double& dest_y);
    void modify_path();
    double to_turn_to;
    double velocity_projection;
    pair<double, double> path;
    double fuel = 1000;
    double actual_angle;
    double velocity_sum = 0;
    double ship_and_velocity_angle;
    double friction_value;

public slots:
    virtual void update(polygon &map);
    void think_n_do();
    void dumb_n_do(double neuron1, double neuron2, double neuron3, double neuron4);
};

//#endif // SHIP_PHYSICS_H
