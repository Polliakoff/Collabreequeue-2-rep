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

    temp_point = point_rotation(temp_point, std::make_pair(0,0),-angle);

    return temp_point;
}


pair<double, double> ship::get_position()
{
    return position;
}

void ship::move_by_coords(const double &delta_x, const double &delta_y)
{
    position.first+=delta_x;
    position.second+=delta_y;

    for(auto &i: body.vertexes){
        i.first+=delta_x;
        i.second+=delta_y;
    }

    for(auto &j: eyes){
        j.move_by(delta_x,delta_y);
    }
}

void ship::move_by_distance(const double &distance)
{
    pair<double, double> moving_vector;
    moving_vector.first = body.vertexes[2].first - position.first;
    moving_vector.second = body.vertexes[2].second - position.second;
    if(distance < 0){
        moving_vector.first*=-1;
        moving_vector.second*=-1;
    }

    double old_x = moving_vector.first;
    double old_y = moving_vector.second;
    moving_vector.first *= distance/sqrt(pow(old_x,2)+pow(old_y,2));
    moving_vector.second *= distance/sqrt(pow(old_x,2)+pow(old_y,2));

    move_by_coords(moving_vector.first,moving_vector.second);
}

double ship::get_angle()
{
    return angle;
}

void ship::rotate_by(const double &delta_angle)
{
    angle+=delta_angle;
    if(angle>=2*M_PI) angle-=2*M_PI;

    for(auto &i: body.vertexes){
        i = point_rotation(i,position,delta_angle);
    }

    for(auto &j: eyes){
        j.rotate(delta_angle);
    }
}

pair<double, double> point_rotation(const pair<double, double> &point, const pair<double, double> &axis, const double &delta_angle)
{
    pair<double, double> result = point;

    double cs = cos(delta_angle);
    double sn = sin(delta_angle);

        result.first -= axis.first;
        result.second -= axis.second;

        double old_x = result.first;
        double old_y = result.second;

        result.first = old_x*cs + old_y*sn;
        result.second = (-1)*old_x*sn + old_y*cs;

        result.first += axis.first;
        result.second += axis.second;

        return result;

}
