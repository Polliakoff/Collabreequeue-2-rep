#include "evolution.h"

evolution::evolution()
{

}

evolution::evolution(const int& generation_size, const double &start_x, const double &start_y, const double &finish_x, const double &finish_y)
{
    for(int i = 0; i < generation_size; i++)
    {
        population.emplace_back(std::make_unique<ship_physics>(start_x,start_y,finish_x,finish_y));
    }
    generation = 1;
}

void evolution::evolve()
{

}

void evolution::evolution_stat()
{
    clock++;
    if(clock>=3000){
        for(auto &i:population){
            if(i->velocity_sum<=100){
                i->operational = false;
                i->can_be_parrent = false;
            }
        }
    }
}
