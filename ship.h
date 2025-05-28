#ifndef SHIP_H
#define SHIP_H

#include <vector>
#include "straight_line.h"
#include "polygon.h"
#include <QDataStream>
#include <QObject>
#include <algorithm>  // для std::sort
#include <cmath>

class ship: public QObject
{
    Q_OBJECT
protected:
    pair<double, double> position;
public:
    explicit ship(QObject *parent = nullptr);//не использовать
    ship(const double& pos_x,const double& pos_y); // задавать корабль исключительно этим конструктором с установленной позицией
    ~ship();

    bool collided = false;

    double angle;
    polygon body;
    vector<straight_line> eyes;
    vector<double> distances;
    vector<pair<double,double>> point_seen;

    pair<double, double> convert_to_ship(const pair<double, double>& point);
    pair<double, double> get_position(); 
    bool segmentIntersect(const pair<double,double>& a,
                          const pair<double,double>& b,
                          const pair<double,double>& c,
                          const pair<double,double>& d,
                          pair<double,double>& out);
    // fallback binary search если hits<2
    double findBoundary(const Eigen::Vector2d& dir,
                        polygon &pol,
                        double t_lo,
                        double t_hi);
    void move_by_coords(const double& new_pos_x,const double& new_pos_y);
    void move_by_distance(const double& distance);
    double get_angle();
    void rotate_by(const double& new_angle);
    bool collision(polygon &pol);
    void eyesight(polygon &pol);
public slots:
    virtual void update(polygon &map);

};

pair<double, double> point_rotation(const pair<double, double>& point, const pair<double, double>& axis, const double &delta_angle);
bool point_to_poly(const pair<double,double>& point, polygon &pol);

#endif // SHIP_H
