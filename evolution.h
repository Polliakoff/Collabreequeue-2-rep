#ifndef EVOLUTIONH_H
#define EVOLUTIONH_H

//#include <QObject>
#include "ship_physics.h"
#include <memory>
#include <algorithm>
#include <map>
#include <QTimer>
#include "pathway.h"
#include <QObject>
#include <QThread>

class evolution
{
protected:
    std::ofstream fout;
public:
    evolution()=default;//РЅРµ С‚СЂРѕРіР°С‚СЊ
    std::string genName="000x";
    evolution(const int& generation_size, std::shared_ptr<pathway> &pthw);
    virtual ~evolution();

    int generation;
    int clock=0;
    int min_speed = 100;
    int tst=0;

    vector<std::unique_ptr<QThread>> population_threads;
    vector<std::unique_ptr<ship_physics>> population;
    vector<std::string> names;
    vector<QMetaObject::Connection> update_connections;
    vector<QMetaObject::Connection> think_n_do_connections;

    void cnnct(std::shared_ptr<QTimer> &timer, std::shared_ptr<pathway> &pthw);
    void cnnct(std::shared_ptr<QTimer> &timer);
    void cnnct();
    void dscnnct();
    void evolve();
    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> timer;

public slots:
    void evolution_stat();

};

#endif // EVOLUTIONH_H
