#ifndef EVOLUTIONH_H
#define EVOLUTIONH_H

//#include <QObject>
#include "ship_physics.h"
#include <memory>
#include <algorithm>
#include <map>
#include <QTimer>
#include "pathway.h"


class evolution
{

private:
    void sanity_check(vector<std::unique_ptr<ship_physics>> population, vector<int> index,
                      std::multimap<double, std::pair<std::string,int>> best,
                      vector<std::pair<std::unique_ptr<ship_physics>,std::string>> newGenParents);
protected:

    std::ofstream fout;
public:
    evolution()=default;//РЅРµ С‚СЂРѕРіР°С‚СЊ
    std::string genName="0x";
    evolution(const int& generation_size, const double& start_x,const double& start_y,const double& finish_x,const double& finish_y);
    virtual ~evolution();

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
    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> timer;
public slots:
    void evolution_stat();

};

#endif // EVOLUTIONH_H
