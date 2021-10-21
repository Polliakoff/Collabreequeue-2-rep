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
}

void ship_physics::helm(const int &mode2)
{
    double agility = 0.001;  //Базовый параметр поворотливости, от которого зависят остальные. Дёргай для изменения ускорения.

    if (mode2 == 1 || mode2 == 4)
    {
        if (angular_velocity > 0) angular_velocity -= agility;
        else if (angular_velocity < 0) angular_velocity += agility;
        if (angular_velocity <= 0.1 && angular_velocity >= -0.1) angular_velocity = 0;
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
    //--------------------------------------Деревянное трение. Убрать после прописывания friction()

    abs_velocity = sqrt(velocity_x*velocity_x + velocity_y*velocity_y);
    velocity_x -= velocity_x/50;
    velocity_y -= velocity_y/50;

    //--------------------------------------

    //    if(velocity_x != 0 and velocity_y != 0)
    //    {
    //        double actual_angle = acos((velocity_y)/(sqrt(velocity_x*velocity_x+velocity_y*velocity_y)));

    //        double ship_and_velocity_angle = abs(actual_angle - acos((velocity_y)/(sqrt(velocity_x*velocity_x+velocity_y*velocity_y))));

    //        double friction_value = -0.3242*ship_and_velocity_angle*ship_and_velocity_angle + 1.2223*ship_and_velocity_angle + 0.32;
    //        //double friction_value = 0.1156*ship_and_velocity_angle*ship_and_velocity_angle - 0.842*ship_and_velocity_angle + 1.6717;
    //        //double friction_value = 0.052*ship_and_velocity_angle*ship_and_velocity_angle - 0.3717*ship_and_velocity_angle + 0.8709;
    //        velocity_x -= sin(angle)*friction_value*velocity_x*0.3;
    //        velocity_y -= cos(angle)*friction_value*velocity_y*0.3;
    //    }
    // double right_side_angle = angle - M_PI/2, left_side_angle = angle + M_PI/2, back_angle = angle - M_PI;
    //double abs_velocity = sqrt(velocity_x*velocity_x + velocity_y*velocity_y);

    //левая сторона
    //правая сторона
    //перед
    //зад

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

    velocity_projection = skalar_multipl(path.first,path.second,velocity_x,velocity_y)/vector_module(path.first,path.second);
    to_turn_to = acos(skalar_multipl(path.first,path.second,eyes[2].direction[0],eyes[2].direction[1])
            /(vector_module(path.first,path.second)*vector_module(eyes[2].direction[0],eyes[2].direction[1])));

    if(position.first + velocity_x > 0){
        to_turn_to *= -1;
    }
}


void ship_physics::update(polygon &map)
{
    ship::update(map);
    modify_path();
}
