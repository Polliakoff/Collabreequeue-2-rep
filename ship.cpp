#include "ship.h"

ship::ship(){}

ship::ship(const double &pos_x, const double &pos_y)
{
    position.first = pos_x;
    position.second = pos_y;
    angle = 0;

    body.add_point(position.first+10, position.second+10);
    body.add_point(position.first+10, position.second-10);
    body.add_point(position.first, position.second-20);
    body.add_point(position.first-10, position.second-10);
    body.add_point(position.first-10, position.second+10);

    eyes.emplace_back(position.first, position.second, position.first+10, position.second-10);
    eyes.emplace_back(position.first, position.second, position.first+10, position.second-10);
}

ship::~ship()
{

}

pair<double, double> ship::convert_to_ship(const pair<double, double>& point)
{
    pair<double, double> temp_point;
    temp_point.first = point.first - position.first;
    temp_point.second = point.second - position.second;

    temp_point.first = temp_point.first*cos(angle) + temp_point.second*sin(angle);
    temp_point.second = -temp_point.first*sin(angle) + temp_point.second*cos(angle);

    return temp_point;

}


pair<double, double> ship::get_position()
{
    return position;
}

void ship::move_by(const double &delta_x, const double &delta_y)
{
    for(auto i: body.vertexes){
        i.first+=delta_x;
        i.second+=delta_y;
    }

    for(auto j: eyes){
        j.move_by(delta_x,delta_y);
    }
}

double ship::get_angle()
{
    return angle;
}

void ship::rotate_by(const double &delta_angle)
{
    for(auto i: body.vertexes){
        i.first = i.first*cos(delta_angle) + i.second*sin(delta_angle);
        i.second = -i.first*sin(delta_angle) + i.second*cos(delta_angle);
    }

    for(auto j: eyes){
        j.rotate(delta_angle);
    }
}
