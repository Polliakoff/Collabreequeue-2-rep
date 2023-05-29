#ifndef SHIP_H
#define SHIP_H

#include <vector>
#include "straight_line.h"
#include "polygon.h"
#include <QDataStream>
#include <QObject>

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
