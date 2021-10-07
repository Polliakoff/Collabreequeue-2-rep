#include "straight_line.h"

straight_line::straight_line(){}

straight_line::straight_line(const double &x1, const double &y1, const double &x2, const double &y2)
{
    direction(0) = x2-x1;
    direction(1) = y2-y1;
    direction(2) = 1;

    point(0) = x1;
    point(1) = y1;
    point(2) = 1;

    rotation_point = point;
}



straight_line::~straight_line()
{

}

void straight_line::rotate(const double &angle)
{
    Eigen::Matrix3d rotation;
    double cs = cos(angle);
    double sn = sin(angle);

    rotation(0,0) = cs;
    rotation(0,1) = sn;
    rotation(0,2) = 0;
    rotation(1,0) = -sn;
    rotation(1,1) = cs;
    rotation(1,2) = 0;
    rotation(2,0) = -rotation_point(0)*(cs-1)+rotation_point(1)*sn;
    rotation(2,1) = -rotation_point(1)*(cs-1)-rotation_point(0)*sn;
    rotation(2,2) = 1;

    direction = rotation*direction;
}

void straight_line::move_by(const double &x, const double &y)
{
    point(0)+=x;
    point(1)+=y;
}

double straight_line::get_x(const double &y)
{
    if(round_to(direction(1),5)!=0){
        return ((y-point(1))/direction(1)*direction(0)+point(0));
    }
    else{
        return std::numeric_limits<double>::infinity();
    }
}

double straight_line::get_y(const double &x)
{
    if(round_to(direction(0),5)!=0){
        return ((x-point(0))/direction(0)*direction(1)+point(1));
    }
    else{
        return std::numeric_limits<double>::infinity();
    }
}




double intersect(straight_line &line_1, straight_line &line_2)
{
    if(round_to(line_1.direction(0),5)==0){
        if(round_to(line_2.direction(0),5)==0){
            return std::numeric_limits<double>::infinity();
        }
        else{
            return line_1.get_x(0);
        }
    }
    else if(round_to(line_2.direction(0),5)==0){
        if(round_to(line_1.direction(0),5)==0){
            return std::numeric_limits<double>::infinity();
        }
        else{
            return line_2.get_x(0);
        }
    }
    else{
        double q1p1 = line_1.direction(0)/line_1.direction(1);
        double q2p2 = line_2.direction(0)/line_2.direction(1);
        return((line_2.point(1)-line_1.point(1)-line_2.point(0)*q2p2+line_1.point(0)*q1p1)/(q1p1-q2p2));
    }
}

std::pair<double, double> perp_vect(std::pair<double, double> input_vector, const double& desired_length)
{
    std::pair<double,double> result;
    result.second = 1;
    result.first = -input_vector.second/input_vector.second;

    double coeff = desired_length/sqrt(pow(result.second,2)+1);

    result.first*=coeff;
    result.second*=coeff;

    return result;
}
