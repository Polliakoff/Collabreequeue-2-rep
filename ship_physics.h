//#ifndef SHIP_PHYSICS_H
//#define SHIP_PHYSICS_H
#pragma once

#include "ship.h"
#include <cmath>

class ship_physics : public ship
{
public:
    ship_physics();
    ship_physics(const double& pos_x,const double& pos_y); // задавать корабль исключительно этим конструктором с установленной позицией
    double velocity_x = 0, velocity_y = 0, abs_velocity;
    double angular_velocity = 0;
    void engine(int mode);
    void helm(int mode2);
    void apply_brain_command(double neuron_1, double neuron_2, double neuron_3, double neuron_4);
    void friction();
};

//#endif // SHIP_PHYSICS_H
