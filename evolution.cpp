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

void evolution::cnnct(QTimer *timer, polygon *map)
{

    int t = population.size();
    for(int i = 0; i < t; i++){
        update_connections.emplace_back(QObject::connect(timer, &QTimer::timeout,
                                                                      [=](){this->population[i]->update(*map);}));

        think_n_do_connections.emplace_back(QObject::connect(timer, &QTimer::timeout,
                                                                          [=](){population[i]->think_n_do();}));
    }
}

void evolution::dscnnct()
{
    int num=0;
    for(auto &i: population){
        if(!i.get()->operational){
            QObject::disconnect(update_connections[num]);
            QObject::disconnect(think_n_do_connections[num]);
        }
        ++num;
    }
}

void evolution::evolve()
{
    int num = 0;
    for(auto &i: population){
        if(i.get()->operational){
            QObject::disconnect(update_connections[num]);
            QObject::disconnect(think_n_do_connections[num]);
        }
        ++num;
    } //стоп машина


    vector<int> index;
    int i = 0;
    for (auto &shp: population){
        if(shp.get()->can_be_parrent){
            index.push_back(i);
        }
        ++i;
    } //выбрали норм корабли

    std::map<double, int> best;
    for(auto &imp: index){
       best.emplace(population[i].get()->distance_to_finish, imp);
    }

    i=0;
    vector <std::unique_ptr<ship_physics>> newGenParents;
    if(index.size()>0)
        for (auto &m: best){
            //auto par = population[i];
            newGenParents.push_back(std::move(population[m.second]));
            ++i;
            if (i==5) break;
        } //отобрали пять лучших

    population.clear();
    double dmnc = 0.7;
    for (auto temp = newGenParents.begin(); temp+1!=newGenParents.end(); ++temp){
        for (auto inner_temp = temp+1; inner_temp!=newGenParents.end(); ++inner_temp){
            population.emplace_back(std::make_unique<ship_physics>(*temp->get(), *inner_temp->get(), dmnc));
            population.emplace_back(std::make_unique<ship_physics>(*temp->get(), *inner_temp->get(), 1-dmnc));
        }
    }

}

void evolution::evolution_stat()
{
    ++clock;

    if(clock>=1000){

        for(auto &i: population){
            if(i->velocity_sum<=min_speed){
                i->operational = false;
                i->can_be_parrent = false;
            }

        }
    }
    //возможно поменять
    dscnnct();

    if(clock==1200){
        evolve();
    }
}
