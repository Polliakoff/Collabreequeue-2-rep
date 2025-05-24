#include "ship_physics.h"

int ship_physics::sId = 0;

ship_physics::ship_physics()
{

}

ship_physics::ship_physics(const double& pos_x,const double& pos_y,const double& dest_x, const double& dest_y):
    ship(pos_x, pos_y), net()
{
    initial_position = std::make_pair(pos_x, pos_y);
    id = ++sId;
    name = '_'+std::to_string(net.S);
    name.push_back('s');
    for (auto &i: net.l){
        name += std::to_string(i);
        name.push_back('.');
    }

    change_destination(dest_x,dest_y);
}

ship_physics::ship_physics(ship_physics &a, ship_physics &b, const double &dmnc,const double& pos_x,const double& pos_y):
    ship(pos_x,pos_y),
    net(a.net, b.net, dmnc)
{
    initial_position = std::make_pair(pos_x, pos_y);
    id = ++sId;
    name = '_'+std::to_string(net.S);
    name.push_back('s');
    for (auto &i: net.l){
        name += std::to_string(i);
        name.push_back('.');
    }

    change_destination(a.final_destination.first,a.final_destination.second);
}

ship_physics::ship_physics(const double& pos_x,const double& pos_y,
                           const double& dest_x, const double& dest_y, brain& newBrain, bool noise):
    ship(pos_x, pos_y)
{
    initial_position = std::make_pair(pos_x, pos_y);
    id = ++sId;
    net = newBrain;
    if(noise){
        net.noiseWeights();
    }
    name = '_'+std::to_string(net.S);
    name.push_back('s');
    for (auto &i: net.l){
        name += std::to_string(i);
        name.push_back('.');
    }

    change_destination(dest_x,dest_y);
}

void ship_physics::think_n_do()
{
    brainstorm();

    engine(1);
    helm(1);

    friction();
    move_by_coords(velocity_x, velocity_y);
    rotate_by(angular_velocity);
}

void ship_physics::engine(const int &mode)
{
    double thrust = 0.02;  //Базовый параметр тяги, от которого зависят остальные. Дёргай для изменения ускорения.

    velocity_x -= sin(angle)*thrust*((net.A.back()(0)-0.5)*2);
    velocity_y -= cos(angle)*thrust*((net.A.back()(0)-0.5)*2);
    fuel_consumption = 2*abs((net.A.back()(0)-0.5)*2);
    fuel += fuel_consumption;
}

void ship_physics::helm(const int &mode2)
{
    double agility = 0.0006;  //Базовый параметр поворотливости, от которого зависят остальные. Дёргай для изменения ускорения.
    double max_maneuver = 0.020;

    angular_velocity += agility*((net.A.back()(1)-0.5)*2);
    fuel_consumption += 1*abs((net.A.back()(1)-0.5)*2);
    fuel += fuel_consumption;
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
    net.A[0](6)=10*abs_velocity; //должно быть в проекции на вектор правильного направления

    for(auto &i:net.A[0]){
        if(qIsNaN(i)){
            this->can_be_parrent = false;
            this->operational = false;
        }
    }
    net.think();
}

void ship_physics::friction()
{

    abs_velocity = sqrt(velocity_x*velocity_x + velocity_y*velocity_y);
    if (velocity_x < 0.0001 && velocity_x > -0.0001 && velocity_y < 0.0001 && velocity_y > -0.0001)
    {
        velocity_x = 0.000001;
        velocity_y = 0.000001;
    }

    actual_angle = acos((velocity_y)/(sqrt(velocity_x*velocity_x+velocity_y*velocity_y)));

    ship_and_velocity_angle = vectors_angle(velocity_x,velocity_y,eyes[2].direction[0],eyes[2].direction[1]);

    friction_value = -0.139*ship_and_velocity_angle*ship_and_velocity_angle + 0.5639*ship_and_velocity_angle + 0.1886;

    if (velocity_x != 0 || velocity_y != 0)
    {
        velocity_x -= friction_value*velocity_x*0.05;
        velocity_y -= friction_value*velocity_y*0.05;
    }

}

void ship_physics::change_destination(const double &dest_x, const double &dest_y)
{    
    final_destination.first = dest_x;
    final_destination.second = dest_y;
    modify_path();
    distance_to_finish = vector_module(path.first,path.second);
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
    distance_to_finish = vector_module(path.first,path.second);
}

void ship_physics::get_distance()
{
    distance_to_start = vector_module(initial_position.first - position.first,initial_position.second - position.second);
}

brain &ship_physics::getBrain()
{
    return net;
}

void ship_physics::noise_Brain()
{
    net.noiseWeights();
}

void ship_physics::set_id(const int& new_id)
{
    id = new_id;
}

void ship_physics::update(polygon &map)
{
    ship::update(map);
    modify_path();
    get_distance();
    velocity_sum+=abs_velocity;
    if(collided){
        operational = false;
    }
    distance_to_finish = vector_module(path.first,path.second);
}

///-----------------------------Функция для дебага, управляющая тестовым кораблём--------------------------------------
void ship_physics::dumb_n_do(double neuron1, double neuron2, double neuron3, double neuron4)
{
    test_engine(neuron1, neuron2);
    test_helm(neuron3, neuron4);
    friction();
    move_by_coords(velocity_x, velocity_y);
    rotate_by(angular_velocity);
}

void ship_physics::test_engine(double neuron1, double neuron2)
{
    double thrust = 0.02;  //Базовый параметр тяги, от которого зависят остальные. Дёргай для изменения ускорения.
    velocity_x -= sin(angle)*thrust*(neuron1-neuron2);
    velocity_y -= cos(angle)*thrust*(neuron1-neuron2);
    fuel_consumption = 2*abs(neuron1-neuron2);
    fuel += fuel_consumption;
}

void ship_physics::test_helm(double neuron3, double neuron4)
{
    double agility = 0.0006;  //Базовый параметр поворотливости, от которого зависят остальные. Дёргай для изменения ускорения.
    double max_maneuver = 0.020;
    angular_velocity += agility*(neuron3-neuron4);
    fuel_consumption += 1*abs(neuron3-neuron4);
    fuel += fuel_consumption;
    if (angular_velocity > max_maneuver)
    {
        angular_velocity = max_maneuver;
    }
    if (angular_velocity < -max_maneuver)
    {
        angular_velocity = -max_maneuver;
    }
}
///--------------------------------------------------------------------------------------------------------------------
