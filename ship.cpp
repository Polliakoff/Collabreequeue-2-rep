#include "ship.h"

ship::ship(QObject *parent): QObject{parent}
{

}

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

bool ship::segmentIntersect(const pair<double, double> &a, const pair<double, double> &b, const pair<double, double> &c, const pair<double, double> &d, pair<double, double> &out)
{
    const double EPS = 1e-9;

    double A1 = b.second - a.second;
    double B1 = a.first  - b.first;
    double C1 = A1*a.first + B1*a.second;

    double A2 = d.second - c.second;
    double B2 = c.first  - d.first;
    double C2 = A2*c.first + B2*c.second;

    double det = A1*B2 - A2*B1;
    if (std::fabs(det) < EPS) {                     // параллели / коллинеар
        auto on = [&](const pair<double,double>& p,
                      const pair<double,double>& p1,
                      const pair<double,double>& p2){
            return p.first  >= std::min(p1.first ,p2.first )-EPS &&
                   p.first  <= std::max(p1.first ,p2.first )+EPS &&
                   p.second >= std::min(p1.second,p2.second)-EPS &&
                   p.second <= std::max(p1.second,p2.second)+EPS;
        };
        if (std::fabs((A1*c.first + B1*c.second) - C1) < EPS) {
            if (on(c,a,b)) { out = c; return true; }
            if (on(d,a,b)) { out = d; return true; }
        }
        return false;
    }

    out.first  = (B2*C1 - B1*C2) / det;
    out.second = (A1*C2 - A2*C1) / det;

    auto inside = [EPS](double v, double v1, double v2)   // <── захват EPS
    {
        return v >= std::min(v1,v2) - EPS &&
               v <= std::max(v1,v2) + EPS;
    };

    return inside(out.first ,a.first ,b.first ) &&
           inside(out.second,a.second,b.second) &&
           inside(out.first ,c.first ,d.first ) &&
           inside(out.second,c.second,d.second);

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
    std::fill(distances.begin(),  distances.end(),  -1);
    std::fill(point_seen.begin(), point_seen.end(),
              std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                             std::numeric_limits<double>::quiet_NaN()));

    for (size_t iEye = 0; iEye < eyes.size(); ++iEye)
    {
        Eigen::Vector2d dir(eyes[iEye].direction(0), eyes[iEye].direction(1));
        dir.normalize();

        std::vector<std::pair<double,double>> hits;      // все t
        std::vector<double>                   ts;

        for (size_t j = 0; j < pol.vertexes.size(); ++j)
        {
            size_t k = (j+1)%pol.vertexes.size();
            pair<double,double> p;
            if (!segmentIntersect(pol.vertexes[j], pol.vertexes[k],
                                  {eyes[iEye].point(0)-1e6*dir.x(),  // «бесконечный» отрезок
                                   eyes[iEye].point(1)-1e6*dir.y()},
                                  {eyes[iEye].point(0)+1e6*dir.x(),
                                   eyes[iEye].point(1)+1e6*dir.y()}, p))
                continue;

            hits.push_back(p);
            ts.push_back( (p.first  - position.first) * dir.x() +
                         (p.second - position.second)* dir.y() );
        }

        double bestFwd  = 1e100, bestBack = 1e100;
        pair<double,double> hitFwd, hitBack;

        for (size_t n = 0; n < ts.size(); ++n) {
            double t = ts[n];
            if (t >= 1e-9) {                       // вперед
                if (t < bestFwd) { bestFwd = t; hitFwd = hits[n]; }
            } else if (t <= -1e-9) {               // назад
                if (-t < bestBack) { bestBack = -t; hitBack = hits[n]; }
            }
        }

        const size_t base = iEye*2;
        distances [base]     = bestBack;
        point_seen[base]     = hitBack;
        distances [base + 1] = bestFwd;
        point_seen[base + 1] = hitFwd;
    }
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

