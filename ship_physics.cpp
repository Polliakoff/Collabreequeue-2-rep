#include "ship_physics.h"

ship_physics::ship_physics()
{

}

ship_physics::ship_physics(const double &pos_x, const double &pos_y):ship(pos_x, pos_y)
{

}

void ship_physics::apply_brain_command(double neuron_1, double neuron_2, double neuron_3, double neuron_4)
{
    double rounded_neuron_1, rounded_neuron_2, rounded_neuron_3, rounded_neuron_4;
    if (neuron_1 > 0.8) rounded_neuron_1 = 1;
    else rounded_neuron_1 = 0;
    if (neuron_2 > 0.8) rounded_neuron_2 = 1;
    else rounded_neuron_2 = 0;
    if (neuron_3 > 0.8) rounded_neuron_3 = 1;
    else rounded_neuron_3 = 0;
    if (neuron_4 > 0.8) rounded_neuron_4 = 1;
    else rounded_neuron_4 = 0;

    if (rounded_neuron_1 == 1 && rounded_neuron_2 == 1) engine(1);
    if (rounded_neuron_1 == 1 && rounded_neuron_2 == 0) engine(2);
    if (rounded_neuron_1 == 0 && rounded_neuron_2 == 1) engine(3);
    if (rounded_neuron_1 == 0 && rounded_neuron_2 == 0) engine(4);

    if (rounded_neuron_3 == 1 && rounded_neuron_4 == 1) helm(1);
    if (rounded_neuron_3 == 1 && rounded_neuron_4 == 0) helm(2);
    if (rounded_neuron_3 == 0 && rounded_neuron_4 == 1) helm(3);
    if (rounded_neuron_3 == 0 && rounded_neuron_4 == 0) helm(4);

    move_by_coords(velocity_x, velocity_y);
    rotate_by(angular_velocity);
}

void ship_physics::engine (int mode)
{
    double thrust = 0.01;  //Базовый параметр тяги, от которого зависят остальные. Дёргай для изменения ускорения.

    if (mode == 1)
    {
        velocity_x -= sin(angle)*thrust/2;
        velocity_y -= cos(angle)*thrust/2;
    }
    if (mode == 2)
    {
        velocity_x -= sin(angle)*thrust;
        velocity_y -= cos(angle)*thrust;
    }
    if (mode == 3)
    {
        velocity_x += sin(angle)*thrust;
        velocity_y += cos(angle)*thrust;
    }
    if (mode == 4)
    {
        //velocity_x += sin(angle)*thrust/5; //to do: движение без ускорения, то есть сравнять с лобовым трением
        //velocity_y -= cos(angle)*thrust/5;
    }

    //--------------------------------------Захардкоженный лимит скорости. Можно будет убрать после добавления трения

    //тут ничего нет
    //--------------------------------------
}

void ship_physics::helm (int mode2)
{
    double agility = 0.001;  //Базовый параметр поворотливости, от которого зависят остальные. Дёргай для изменения ускорения.

    if (mode2 == 1 || mode2 == 4)
    {
        if (angular_velocity > 0) angular_velocity -= agility;
        else if (angular_velocity < 0) angular_velocity += agility;
    }
    if (mode2 == 2) angular_velocity += agility;
    if (mode2 == 3) angular_velocity -= agility;

    if (angular_velocity > 0.015)
    {
        angular_velocity = 0.015;
    }
    if (angular_velocity < -0.015)
    {
        angular_velocity = -0.015;
    }

}
