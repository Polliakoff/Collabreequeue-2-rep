#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "ship_physics.h"
//#include "func.h"
//#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <QObject>
#include <QTimer>
#include "pathway.h"

class evolution
{
public:
    evolution();//не трогать
    evolution(const int& generation_size, const double& start_x,const double& start_y,const double& finish_x,const double& finish_y);

    int generation;
    int clock;
    int min_speed = 100;
    vector <std::unique_ptr<ship_physics>> population;
    vector<QMetaObject::Connection> update_connections;
    vector<QMetaObject::Connection> think_n_do_connections;

    void cnnct(QTimer *, polygon *);
    void dscnnct();
    void evolve();
    //void disconnect(std::vector<QMetaObject::Connection> a, std::vector<QMetaObject::Connection> b);
public slots:
    void evolution_stat();

};

#endif // EVOLUTION_H
