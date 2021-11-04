#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "ship_physics.h"
#include <memory>
#include <algorithm>
#include <map>
#include <QObject>
#include <QTimer>
#include "pathway.h"

class evolution
{
protected:
    evolution();//не трогать
public:
    std::ofstream fout;
    std::string genName="0x";
    evolution(const int& generation_size, const double& start_x,const double& start_y,const double& finish_x,const double& finish_y);

    int generation;
    int clock=0;
    int min_speed = 100;
    int tst=0;
    vector<std::unique_ptr<ship_physics>> population;
    vector<std::string> names;
    vector<QMetaObject::Connection> update_connections;
    vector<QMetaObject::Connection> think_n_do_connections;

    void cnnct(std::shared_ptr<QTimer> &timer, std::shared_ptr<pathway> &map);
    void cnnct();
    void dscnnct();
    void evolve();
    //void disconnect(std::vector<QMetaObject::Connection> a, std::vector<QMetaObject::Connection> b);
    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> timer;
public slots:
    void evolution_stat();

};

#endif // EVOLUTION_H
