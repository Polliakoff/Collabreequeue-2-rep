#include "ship_physics.h"

ship_physics::ship_physics()
{

}

ship_physics::ship_physics(const double& pos_x,const double& pos_y,const double& dest_x, const double& dest_y):ship(pos_x, pos_y)
{
    change_destination(dest_x,dest_y);
}

void ship_physics::think_n_do()
{
    brainstorm();

    if (net.A.back()(0) > 0.8 && net.A.back()(1) > 0.8) engine(1);   //добавить газу
    else if (net.A.back()(0) > 0.8) engine(2);                       //макс скорость
    else if (net.A.back()(1) > 0.8) engine(3);                       //обратный ход
    else engine(4);                                                  //глушим двигатель

    if (net.A.back()(2) > 0.8 && net.A.back()(3) <= 0.8) helm(2);    //поворот туда
    if (net.A.back()(3) > 0.8 && net.A.back()(2) <= 0.8) helm(3);    //поворот НЕ ТУДА
    if(!(net.A.back()(2) > 0.8 || net.A.back()(3) > 0.8)) helm(1);   //убить угловую скорость


    //    //добавить газу
    //    if (net.A.back()(0) > 0.8)
    //        engine(2); //просто вдавить вперед
    //    //стоп машина
    //    if (net.A.back()(1) > 0.8)
    //        engine(3); //должно быть вдавить назад

    //    11 медленнее ехать(больше топлива?) 00 убивать скорость об трение
    //    //поворот туда
    //    if (net.A.back()(2) > 0.8)
    //        helm(2); //просто налево
    //    //поворот НЕ ТУДА
    //    if (net.A.back()(3) > 0.8)
    //        helm(3); //просто направо
    //    пусть сам ищет путь 11 - без изменений(но больше топлива?) 00 без изменений


    friction();
    move_by_coords(velocity_x, velocity_y);
    rotate_by(angular_velocity);
}

void ship_physics::engine(const int &mode)
{
    double thrust = 0.02;  //Базовый параметр тяги, от которого зависят остальные. Дёргай для изменения ускорения.

    if (mode == 1)
    {
        velocity_x -= sin(angle)*thrust/2;
        velocity_y -= cos(angle)*thrust/2;
        fuel -= 0.5;
    }
    if (mode == 2)
    {
        velocity_x -= sin(angle)*thrust;
        velocity_y -= cos(angle)*thrust;
        fuel -= 1;
    }
    if (mode == 3)
    {
        velocity_x += sin(angle)*thrust;
        velocity_y += cos(angle)*thrust;
        fuel -= 1;
    }
    if (mode == 4)
    {
        if (velocity_x != 0 || velocity_y != 0) //по какой-то неясной причине friction value не умножается при скорости равной нулю и убивает корабль как объект. Too bad.
        {
            velocity_x -= sin(angle)*abs_velocity*friction_value*0.1;
            velocity_y -= cos(angle)*abs_velocity*friction_value*0.1;
            fuel -= abs_velocity*friction_value*0.1/thrust;
        }
    }
}

void ship_physics::helm(const int &mode2)
{
    double agility = 0.0003;  //Базовый параметр поворотливости, от которого зависят остальные. Дёргай для изменения ускорения.
    double max_maneuver = 0.010;

    if (mode2 == 1 || mode2 == 4)
    {
        if (angular_velocity > 0) angular_velocity -= agility;
        else if (angular_velocity < 0) angular_velocity += agility;
        if (angular_velocity <= agility+0.0001 && angular_velocity >= -(agility+0.0001)) angular_velocity = 0;
    }
    if (mode2 == 2) angular_velocity += agility;
    if (mode2 == 3) angular_velocity -= agility;

    if (angular_velocity > max_maneuver)
    {
        angular_velocity = max_maneuver;
    }
    if (angular_velocity < -max_maneuver)
    {
        angular_velocity = -max_maneuver;
    }

}

void ship_physics::brainstorm()
{
    net.A[0](0)=distances[0];
    net.A[0](1)=distances[1];
    net.A[0](2)=distances[2];
    net.A[0](3)=distances[3];
    net.A[0](4)=distances[4];
    net.A[0](5)=distances[5];
    net.A[0](6)=angle;
    net.A[0](7)=abs_velocity;
    net.A[0](8)=velocity_x; //должно быть в проекции на вектор правильного направления
    net.think();
}

void ship_physics::friction()
{

    abs_velocity = sqrt(velocity_x*velocity_x + velocity_y*velocity_y);
    if (velocity_x < 0.0001 && velocity_x > -0.0001 && velocity_y < 0.0001 && velocity_y > -0.0001)
    {
        velocity_x = 0;
        velocity_y = 0;
    }

    actual_angle = acos((velocity_y)/(sqrt(velocity_x*velocity_x+velocity_y*velocity_y)));

    ship_and_velocity_angle = vectors_angle(velocity_x,velocity_y,eyes[2].direction[0],eyes[2].direction[1]);

    friction_value = -0.139*ship_and_velocity_angle*ship_and_velocity_angle + 0.5639*ship_and_velocity_angle + 0.1886;

    if (velocity_x != 0 || velocity_y != 0) //по какой-то неясной причине friction value не умножается при скорости равной нулю и убивает корабль как объект. Too bad.
    {
        velocity_x -= friction_value*velocity_x*0.1;
        velocity_y -= friction_value*velocity_y*0.1;
    }
}

void ship_physics::change_destination(const double &dest_x, const double &dest_y)
{
    final_destination.first = dest_x;
    final_destination.second = dest_y;
    modify_path();
}

void ship_physics::modify_path()
{
    path.first = final_destination.first - position.first;
    path.second = final_destination.second - position.second;

    velocity_projection = vectors_projection(path.first,velocity_x,path.second,velocity_y);

    to_turn_to = vectors_angle(path.first,path.second,eyes[2].direction[0],eyes[2].direction[1]);
    if(convert_to_ship(final_destination).first>0){
        to_turn_to *= -1;
    }
}


void ship_physics::update(polygon &map)
{
    ship::update(map);
    modify_path();
    velocity_sum+=abs_velocity;
    if(collided){
        can_be_parrent = false;
    }
    if(fuel <= 0 || collided){
        operational = false;
    }
}

///-----------------------------Функция для дебага, управляющая 61-ым кораблём--------------------------------------
void ship_physics::dumb_n_do(double neuron1, double neuron2, double neuron3, double neuron4)
{
    if (neuron1 > 0.8 && neuron2 > 0.8) engine(1);   //добавить газу
    else if (neuron1 > 0.8) engine(2);                       //макс скорость
    else if (neuron2 > 0.8) engine(3);                       //обратный ход
    else engine(4);                                                  //глушим двигатель

    if (neuron3 > 0.8 && neuron4 <= 0.8) helm(2);    //поворот туда
    if (neuron4 > 0.8 && neuron3 <= 0.8) helm(3);    //поворот НЕ ТУДА
    if(!(neuron3 > 0.8 || neuron4 > 0.8)) helm(1);   //убить угловую скорость

    friction();
    move_by_coords(velocity_x, velocity_y);
    rotate_by(angular_velocity);
}
///--------------------------------------------------------------------------------------------------------------------
