#include "evolution.h"

using namespace std;

evolution::evolution()
{

}

evolution::evolution(const int& generation_size, const double &start_x, const double &start_y, const double &finish_x, const double &finish_y)
{
    for(int i = 0; i < generation_size; i++)
    {
        auto ptr = make_unique<ship_physics>(start_x,start_y,finish_x,finish_y);
        population.emplace(genName + ptr.get()->name,std::move(ptr));
    }
    generation = generation_size;

    fout.open("evolution.log");
}

evolution::~evolution()
{
    fout.close();
}

void evolution::evolve()
{
    dscnnct();

    if(genName[1]=='z') {
        genName[0] = int(genName[0])+1;
        genName[1] = 'a';
    } else
        genName[1]=char(genName[1])+1;

    for(auto &i: population){
        QObject::disconnect(update_connections[i.first]);
        QObject::disconnect(think_n_do_connections[i.first]);
    } //стоп машина


    vector<string> index;
    for (auto &shp: population){
        if(shp.second.get()->can_be_parent){
            index.emplace_back(shp.first);
        }
    } //выбрали норм корабли

    std::map<double, string> best;
    for(auto &imp: index){
       best.emplace(population[imp].get()->distance_to_finish, imp);
    }

    int i = 0;
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

    if (newGenParents.size()>1)
        for (auto temp = newGenParents.begin(); temp+1!=newGenParents.end(); ++temp){
            for (auto inner_temp = temp+1; inner_temp!=newGenParents.end(); ++inner_temp){
                fout << endl << temp->get()->name << "\n+\n" << inner_temp->get()->name << endl;

                auto ptr = make_unique<ship_physics>(*temp->get(), *inner_temp->get(), dmnc);
                population.emplace(genName + ptr.get()->name,std::move(ptr));

                ptr = make_unique<ship_physics>(*temp->get(), *inner_temp->get(), 1-dmnc);
                population.emplace(genName + ptr.get()->name,std::move(ptr));
            }
        }
    //возвращаем родителей
    for (auto &shp: newGenParents){
        population.emplace(shp.get()->name,std::move(shp));
    }

    //проверка на антихриста
    for(auto temp = population.begin(); temp!=population.end(); ){
        if(!temp->second->viable()){
            population.erase(temp);
        } else ++temp;
    }
    for(auto &par: newGenParents){
        //population.push_back(std::move(par));
    }
    for (int i = population.size(); i < generation; ++i){
        auto ptr = make_unique<ship_physics>(575,650,0,0);
        population.emplace(genName + ptr.get()->name,std::move(ptr));
    }
    clock = 0;
    ++tst;
    if(tst>40)
        cnnct();
    //cnnct();
}

void evolution::evolution_stat()
{
    ++clock;

    if(clock==10){
        for(auto &i: population){
            if(i.second->velocity_sum<=10){
                i.second->operational = false;
                i.second->can_be_parent = false;
            }
        }
    }
    if(clock==500){
        for(auto &i: population){
            if(i.second->velocity_sum<=70){
                i.second->operational = false;
                i.second->can_be_parent = false;
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
    //int t = population.size();
    for(auto &i: population){
        auto temp = i.first;
        update_connections.emplace(temp, QObject::connect(timer.get(), &QTimer::timeout,
                                                                      [=](){population[temp]->update(*map.get());}));

        think_n_do_connections.emplace(temp, QObject::connect(timer.get(), &QTimer::timeout,
                                                                          [=](){population[temp]->think_n_do();}));
    }
}

void evolution::cnnct()
{
    for(auto &i: population){

        auto temp = i.first;
        update_connections.emplace(temp, QObject::connect(timer.get(), &QTimer::timeout,
                                                                      [=](){population[temp]->update(*map.get());}));

        think_n_do_connections.emplace(temp, QObject::connect(timer.get(), &QTimer::timeout,
                                                                          [=](){population[temp]->think_n_do();}));
    }
}

void evolution::dscnnct()
{
    for(auto i = population.begin(); i!=population.end() ; ){
        if(!i->second->operational){
            QObject::disconnect(update_connections[i->first]);
            QObject::disconnect(think_n_do_connections[i->first]);
            //population.erase(i);
            //--num;
        }
        ++i;
    }
}
