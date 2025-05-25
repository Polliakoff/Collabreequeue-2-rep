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
#include <QStringList>
#include <iomanip>

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
    static constexpr double DIST_EPS = 100.0;
    QString endSaveFileName = "";
    bool running = false;
    bool evolving = true;
    std::ofstream nmeaLog;

public:
    evolution()=default;
    std::string genName="000x";
    evolution(const int& generation_size, std::shared_ptr<pathway> &pthw);
    evolution(const std::shared_ptr<pathway> &pthw, const QString &brainsFile);
    evolution(int generation_size, const QStringList &geoMaps);
    virtual ~evolution();

    int generation;
    int clock=0;
    int min_speed = 100;
    int tst=0;

    /* --- переключение карт --- */
    QStringList geo_files;         // список путей
    int cur_map = -1;
    bool advance_map();           // загружает следующую карту

    /* --- детектор стагнации --- */
    int stagnate_cnt = 0;
    int gen_cnt = 0;
    double best_prev = 1e9;
    static constexpr int N_STAG = 3;   // поколений без прироста
    static constexpr int N_GEN = 10;   // поколений без прироста
    static constexpr double EPS_STAG = 5.0;  // м (разница «почти не изм.»)

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
    void saveBestBrains(const QString &fileName);
    void openNMEALog(const QString &fileName);
    bool isItRunning();
    std::shared_ptr<pathway> map;
    std::shared_ptr<QTimer> timer;

public slots:
    void evolution_stat();

};

#endif // EVOLUTIONH_H
