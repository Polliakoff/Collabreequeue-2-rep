#ifndef EVOLUTION_H
#define EVOLUTION_H

#include "ship_physics.h"
#include "func.h"
#include <vector>
#include <memory>

class evolution
{
public:
    evolution();//не трогать
    evolution(const int& generation_size, const double& start_x,const double& start_y,const double& finish_x,const double& finish_y);

    vector <std::unique_ptr<ship_physics>> population;
    int generation;
    int clock;

    void evolve();
public slots:
    void evolution_stat();

};

#endif // EVOLUTION_H
