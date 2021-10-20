//#ifndef SHIP_PHYSICS_H
//#define SHIP_PHYSICS_H
#pragma once

#include "ship.h"
#include <cmath>
#include "brain.h"

class ship_physics : public ship
{
    brain net;
public:
    ship_physics();
    ship_physics(const double& pos_x,const double& pos_y); // задавать корабль исключительно этим конструктором с установленной позицией
    double velocity_x = 0, velocity_y = 0, abs_velocity;
    double angular_velocity = 0;
    void engine(const int &mode);
    void helm(const int &mode2);
    void apply_brain_command();
    void brainstorm();
    void friction();
};

//#endif // SHIP_PHYSICS_H
