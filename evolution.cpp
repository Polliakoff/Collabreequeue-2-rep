#include "evolution.h"

evolution::evolution(QObject *parent) : QObject(parent)
{

}

evolution::evolution(const int& generation_size, const double &start_x, const double &start_y, const double &finish_x, const double &finish_y)
{
    fout.open("evolution_obj.log");
    for(int i = 0; i < generation_size; i++)
    {
        population.emplace_back(std::make_unique<ship_physics>(start_x,start_y,finish_x,finish_y));
        names.emplace_back(genName + population.back().get()->name);
    }
    generation = generation_size;
}

void evolution::evolve()
{
    //dscnnct();

    if(genName[1]=='z') {
            genName[0] = int(genName[0])+1;
            genName[1] = 'a';
        } else
            genName[1]=char(genName[1])+1;

    int num = 0;
    for(auto &shp: population){
        QObject::disconnect(update_connections[num]);
        QObject::disconnect(think_n_do_connections[num]);
        ++num;
    } //стоп машина


    vector<int> index;
    int i = 0;
    for (auto &shp: population){
        if(shp.get()->can_be_parrent){
            index.push_back(i);
        }
        ++i;
    } //выбрали норм корабли //доработать

    std::map<double, std::pair<std::string,int>> best;
    for(auto &imp: index){
       best.emplace(population[imp].get()->distance_to_finish, std::make_pair(names[imp],imp));
    }

    i=0;
    vector<std::pair<std::unique_ptr<ship_physics>,std::string>> newGenParents;

    names.clear();
    names.reserve(generation);

    if(index.size()>0){
        for (auto &m: best){
            //auto par = population[i];
            newGenParents.push_back(std::make_pair(std::move(population[m.second.second]),m.second.first));
            ++i;
            if (i==5) break;
        } //отобрали пять лучших
    }


    population.clear();
    think_n_do_connections.clear();
    update_connections.clear();

    double dmnc = 0.7;
    population.reserve(generation);
    fout << "!!!NEW GENERATION!!!\t" << genName << "\twe have " << newGenParents.size() << " parents" << "\n\n";
    if (newGenParents.size()>0){
        for (auto temp = newGenParents.begin(); temp+1!=newGenParents.end(); ++temp){
            for (auto inner_temp = temp+1; inner_temp!=newGenParents.end(); ++inner_temp){
                fout << "merging:\t" << temp->second << "\n\t\t\t" << inner_temp->second << "\n";
                population.emplace_back(std::make_unique<ship_physics>(*temp->first.get(), *inner_temp->first.get(), dmnc));
                fout << "first:\t\t" << genName + population.back().get()->name << "\n";
                population.emplace_back(std::make_unique<ship_physics>(*temp->first.get(), *inner_temp->first.get(), 1-dmnc));
                fout << "second:\t\t" << genName + population.back().get()->name << "\n\n";
            }
        }
    }

    //проверка на антихриста
    for(auto temp = population.begin(); temp!=population.end(); ){
        if(!temp->get()->viable()){
            population.erase(temp);
        } else ++temp;
    }
    for(auto &shp: population){
        names.emplace_back(genName + shp->name);
    }
    for(auto &par: newGenParents){
        population.emplace_back(std::make_unique<ship_physics>(575,650,0,0,par.first.get()->getBrain()));
        names.emplace_back(par.second);
    }
    for (int i = population.size(); i < generation; ++i){
        population.emplace_back(std::make_unique<ship_physics>(575,650,0,0));
        names.emplace_back(genName + population.back().get()->name);
    }

    clock = 0;
    emit evolution::valueChanged(genName);
    cnnct();
}

evolution::~evolution(){

}

void evolution::evolution_stat()
{
    ++clock;

    if(clock>=700){

        for(auto &i: population){
            if(i->velocity_sum<=min_speed-40){
                i->operational = false;
                i->can_be_parrent = false;
            }

        }
    }
    //возможно поменять
    dscnnct();

    if(clock==80){
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
