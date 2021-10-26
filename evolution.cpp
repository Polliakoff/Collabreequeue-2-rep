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
    generation = generation_size;
}

void evolution::evolve()
{
    dscnnct();


    int num = 0;
    for(auto &i: population){
        //if(i.get()->operational){
        QObject::disconnect(update_connections[num]);
        QObject::disconnect(think_n_do_connections[num]);
        //}
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
       best.emplace(population[imp].get()->distance_to_finish, imp);
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
    think_n_do_connections.clear();
    update_connections.clear();
    double dmnc = 0.7;
    population.reserve(generation);
    if (newGenParents.size()>0)
        for (auto temp = newGenParents.begin(); temp+1!=newGenParents.end(); ++temp){
            for (auto inner_temp = temp+1; inner_temp!=newGenParents.end(); ++inner_temp){
                population.emplace_back(std::make_unique<ship_physics>(*temp->get(), *inner_temp->get(), dmnc));
                population.emplace_back(std::make_unique<ship_physics>(*temp->get(), *inner_temp->get(), 1-dmnc));
            }
        }
    //проверка на антихриста
    for(auto temp = population.begin(); temp!=population.end(); ){
        if(!temp->get()->viable()){
            population.erase(temp);
        } else ++temp;
    }
    for(auto &par: newGenParents){
        //population.push_back(std::move(par));
    }
    for (int i = population.size(); i < generation; ++i)
        population.emplace_back(std::make_unique<ship_physics>(575,650,0,0));

    clock = 0;
    ++tst;
    if(tst>40)
        cnnct();
    //cnnct();
}

void evolution::evolution_stat()
{
    ++clock;

    if(clock==100){
        for(auto &i: population){
            if(i->velocity_sum<=10){
                i->operational = false;
                i->can_be_parrent = false;
            }
        }
    }
    if(clock==500){
        for(auto &i: population){
            if(i->velocity_sum<=70){
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


void evolution::cnnct(std::shared_ptr<QTimer> &timer, std::shared_ptr<pathway> &map)
{
    this->timer=timer;
    this->map=map;
    int t = population.size();
    for(int i = 0; i < t; i++){
        update_connections.emplace_back(QObject::connect(timer.get(), &QTimer::timeout,
                                                                      [=](){population[i]->update(*map.get());}));

        think_n_do_connections.emplace_back(QObject::connect(timer.get(), &QTimer::timeout,
                                                                          [=](){population[i]->think_n_do();}));
    }
}

void evolution::cnnct()
{
    int t = population.size();
    for(int i = 0; i < t; i++){
        update_connections.emplace_back(QObject::connect(timer.get(), &QTimer::timeout,
                                                                      [=](){population[i]->update(*map.get());}));

        think_n_do_connections.emplace_back(QObject::connect(timer.get(), &QTimer::timeout,
                                                                          [=](){population[i]->think_n_do();}));
    }
}

void evolution::dscnnct()
{
    int num=0;
    for(auto i = population.begin(); i!=population.end() ; ){
        if(!i->get()->operational){
            QObject::disconnect(update_connections[num]);
            QObject::disconnect(think_n_do_connections[num]);
            //population.erase(i);
            //--num;
        }
        ++i;
        ++num;
    }
}
