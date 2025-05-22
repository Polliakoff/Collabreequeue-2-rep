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
#include <random>               // <-- единый ГСЧ
#include <vector>       //  Нужен, т.к. ниже есть std::vector
#include <fstream>      //  std::ofstream в составе класса
#include <tuple>

class evolution
{
protected:
    std::ofstream fout;
    /* --- параметры GA (можно подправить эмпирически) --- */
    static constexpr double P_NOISE      = 0.20;
    static constexpr double P_ADD_CONN   = 0.05;
    static constexpr double P_DEL_CONN   = 0.05;
    static constexpr double P_SPLIT      = 0.05;
    static constexpr double P_ADD_LAYER  = 0.02;
    static constexpr double P_DEL_LAYER  = 0.02;
    static int chooseParentCount(int G);

public:
    evolution()=default;
    std::string genName="000x";
    evolution(const int& generation_size, std::shared_ptr<pathway> &pthw);
    virtual ~evolution();

    int generation;
    int clock=0;
    int min_speed = 100;
    int tst=0;

    std::vector<std::unique_ptr<ship_physics>> population;
    std::vector<std::string> names;
    std::vector<QMetaObject::Connection> update_connections;
    std::vector<QMetaObject::Connection> think_n_do_connections;

    void cnnct(std::shared_ptr<QTimer> &timer, std::shared_ptr<pathway> &pthw);
    void cnnct(std::shared_ptr<QTimer> &timer);
    void mutate(ship_physics &);
    void cnnct();
    void dscnnct();
    void evolve();
    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> timer;

public slots:
    void evolution_stat();

};

#endif // EVOLUTIONH_H
