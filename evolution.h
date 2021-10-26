#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "ship_physics.h"
//#include "func.h"
//#include <vector>
#include <memory>
#include <algorithm>
#include <map>
#include <unordered_map>
#include <QObject>
#include <QTimer>
#include "pathway.h"
#include <fstream>

class evolution
{
public:
    evolution();//не трогать
    evolution(const int& generation_size, const double& start_x,const double& start_y,const double& finish_x,const double& finish_y);

    ~evolution();

    std::string genName="0x";
    std::ofstream fout;
    char shpId = '0';
    int generation;
    int clock=0;
    int min_speed = 100;
    int tst=0;
    std::unordered_map<std::string, std::unique_ptr<ship_physics>> population;
    std::unordered_map<std::string, QMetaObject::Connection> update_connections;
    std::unordered_map<std::string, QMetaObject::Connection> think_n_do_connections;

    void cnnct(std::shared_ptr<QTimer> &timer, std::shared_ptr<pathway> &map);
    void cnnct();
    void dscnnct();
    void evolve();

    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> timer;
public slots:
    void evolution_stat();

};

#endif // EVOLUTION_H
