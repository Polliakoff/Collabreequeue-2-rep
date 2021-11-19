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

    ///отлов бага==========================
    int deep_search_counter = 0;
    bool in_search;
    bool in_deep_search;
    int subject_id;
    std::unique_ptr<ship_physics> subject;
    pair<double, double> death_position;
    double final_fuel;
    ///отлов бага==========================
public slots:
    void evolution_stat();

};

#endif // EVOLUTIONH_H
