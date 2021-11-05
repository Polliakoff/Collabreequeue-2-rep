#include "ship.h"

ship::ship(){}

ship::ship(const double& pos_x,const double& pos_y)
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
    eyes.emplace_back(position.first, position.second, position.first-10, position.second-10);
    eyes.emplace_back(position.first, position.second, position.first, position.second-10);

    for(int i=0;i<6;i++){
        distances.emplace_back(-1);
    }
    point_seen.resize(6);

    rotate_by(-180+rand()%360);
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
    moving_vector.first *= abs(distance)/sqrt(pow(old_x,2)+pow(old_y,2));
    moving_vector.second *= abs(distance)/sqrt(pow(old_x,2)+pow(old_y,2));

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
    if(angle<=-2*M_PI) angle+=2*M_PI;

    for(auto &i: body.vertexes){
        i = point_rotation(i,position,delta_angle);
    }

    for(auto &j: eyes){
        j.rotate(delta_angle);
    }
}

bool ship::collision(polygon &pol)
{
    for(auto i:body.vertexes){
        if(point_to_poly(i,pol) == false) return true;
    }
    return false;
}

void ship::eyesight(polygon &pol)
{
    vector<double> temp_distances;
    for(int i=0;i<6;i++){
        temp_distances.emplace_back(-1);
    }
    vector<pair<double,double>> temp_point_seen;
    temp_point_seen.resize(6);

    for(size_t i = 0; i<eyes.size(); i++){
        for(size_t j = 0; j<pol.faces.size();j++){
            pair<double,double> checking_point;
            int range_1;
            int range_2;
            if(j!=pol.faces.size()-1){
                range_1 = j;
                range_2 = j+1;
            }
            else{
                range_1 = j;
                range_2 = 0;
            }
            checking_point = intersect(eyes[i],pol.faces[j],pol.vertexes[range_1],pol.vertexes[range_2]);
            if(checking_point.first != std::numeric_limits<double>::infinity()){

                auto cheching_poit_positon = convert_to_ship(checking_point);

                double pos_in_mas;

                if(i!=2){
                    if(cheching_poit_positon.first < 0){
                        pos_in_mas = (i+1)*2-2;

                    }
                    else{
                        pos_in_mas = (i+1)*2-1;
                    }
                }
                else{
                    if(cheching_poit_positon.second > 0){
                        pos_in_mas = (i+1)*2-2;

                    }
                    else{
                        pos_in_mas = (i+1)*2-1;
                    }
                }

                if(temp_distances[pos_in_mas] == -1){
                    temp_distances[pos_in_mas] = sqrt(pow(checking_point.first-position.first,2)+pow(checking_point.second-position.second,2));
                    temp_point_seen[pos_in_mas].first = checking_point.first;
                    temp_point_seen[pos_in_mas].second = checking_point.second;
                }

                else if(temp_distances[pos_in_mas]>=sqrt(pow(checking_point.first-position.first,2)+pow(checking_point.second-position.second,2))){
                    temp_distances[pos_in_mas] = sqrt(pow(checking_point.first-position.first,2)+pow(checking_point.second-position.second,2));
                    temp_point_seen[pos_in_mas].first = checking_point.first;
                    temp_point_seen[pos_in_mas].second = checking_point.second;
                }
            }
        }
    }

    distances = temp_distances;
    point_seen = temp_point_seen;
}

void ship::update(polygon &map)
{
    eyesight(map);
    collided = collision(map);
}

bool point_to_poly(const pair<double,double>& point, polygon &pol)
{
    int intersections_l = 0;
    int intersections_r = 0;
    auto input_vertexes = pol.vertexes;
    straight_line rays(point.first, point.second, point.first+1,point.second+1);
    for(size_t j = 0; j<pol.faces.size();j++){
        int range_1;
        int range_2;
        if(j!=pol.faces.size()-1){
            range_1 = j;
            range_2 = j+1;
        }
        else{
            range_1 = j;
            range_2 = 0;
        }
        auto checkig_point = intersect(pol.faces[j],rays,pol.vertexes[range_1],pol.vertexes[range_2]);
        if(checkig_point.first != std::numeric_limits<double>::infinity()){
            auto found{std::find(input_vertexes.begin(), input_vertexes.end(),
                                 checkig_point)};
            if(found != input_vertexes.end()){
                pair<double,double> fixing_vector = perp_vect(std::make_pair(rays.direction(0),rays.direction(1)),0.1);
                found->first+=fixing_vector.first;
                found->second+=fixing_vector.second;
                polygon new_poly;
                for(auto i:input_vertexes){
                    new_poly.add_point(i.first,i.second);
                }
                return point_to_poly(point,new_poly);
            }
            else{
                if(checkig_point.first>point.first) intersections_r++;
                else intersections_l++;

            }
        }
    }
    bool result;
    if(intersections_l % 2 != 0 && intersections_r % 2 != 0) result = true;
    else result = false;
    return result;
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

