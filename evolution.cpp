#include "evolution.h"

evolution::evolution(const int& generation_size, std::shared_ptr<pathway> &pthw)
{
    fout.open("evolution_obj.log");
    map = pthw;
    for(int i = 0; i < generation_size; i++)
    {
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second));
        names.emplace_back(genName + population.back().get()->name);
    }
    generation = generation_size;
    fout << "";
    fout.close();
}

void evolution::evolve()
{
    //dscnnct();
    fout.open("evolution_obj.log", std::ios::app);

    if(genName[3]=='z') {
        if (genName[2]=='9'){
            genName[2] = '0';
            if (genName[1]=='9'){
                genName[1] = '0';
                genName[0] = int(genName[0])+1;
            } else genName[1] = int(genName[1])+1; //меняет цифру
        } else genName[2] = int(genName[2])+1; //меняет цифру
        genName[3] = 'a';
    } else genName[3]=char(genName[3])+1; //меняет букву

    int num = 0;
    for(auto &shp: population){
        QObject::disconnect(update_connections[num]);
        QObject::disconnect(think_n_do_connections[num]);
        ++num;
    } //стоп машина

    vector<int> index;
    vector<int> learning_index;
    int i = 0;
    for (auto &shp: population){
        if(shp.get()->can_be_parrent){
            index.push_back(i);
        }
        else if(shp.get()->autist == false){
            shp.get()->time_to_learn();
            learning_index.push_back(i);
        }
        ++i;
    } //выбрали норм корабли //доработать

    //отбор родителей
    std::multimap<double, std::pair<std::string,int>> best;

    for(auto &imp: index){
        best.emplace(population[imp].get()->distance_to_finish, std::make_pair(names[imp],imp));
    }

    i=0;
    vector<std::pair<std::unique_ptr<ship_physics>,std::string>> newGenParents;
    if(index.size()>0){
        for (auto &m: best){
            //auto par = population[i];
            newGenParents.push_back(std::make_pair(std::move(population[m.second.second]),m.second.first));
            ////не убирать, для проверки скрещивания
            //            auto pal = std::make_unique<ship_physics>(575,650,0,0);
            //            newGenParents.push_back(std::make_pair(std::move(pal), genName + pal.get()->name));
            ++i;
            if (i == 10)
                break;
        } //отобрали пять лучших
    }

    //отбор учащихся
    std::multimap<double, std::pair<std::string,int>> learning_best;

    for(auto &imp: learning_index){
        learning_best.emplace(population[imp].get()->distance_to_finish, std::make_pair(names[imp],imp));
    }

    i=0;
    vector<std::pair<std::unique_ptr<ship_physics>,std::string>> newGenLearners;
    if(learning_index.size()>0){
        for (auto &m: learning_best){
            newGenLearners.push_back(std::make_pair(std::move(population[m.second.second]),m.second.first));
            ++i;
            if (i == 200)
                break;
        }
    }

    names.clear();
    names.reserve(generation);

    population.clear();
    think_n_do_connections.clear();
    update_connections.clear();

    double dmnc = 0.6;
    population.reserve(generation);
    fout << "!!!NEW GENERATION!!!\t" << genName << "\twe have " << newGenParents.size() << " parents" << "\n\n";
    if(newGenParents.size()==1){
        fout << "single parrent:\t" << newGenParents[0].second << "\n";
    }
    for(int t = 2; t>0; --t){
        if (newGenParents.size()>0){
            for (auto temp = newGenParents.begin(); temp+1!=newGenParents.end(); ++temp){
                for (auto inner_temp = temp+1; inner_temp!=newGenParents.end(); ++inner_temp){
                    fout << "merging:\t" << temp->second << "\n\t\t\t" << inner_temp->second << "\n";
                    population.emplace_back(std::make_unique<ship_physics>(*temp->first.get(), *inner_temp->first.get(),
                                                                           dmnc,map->start_point.first,map->start_point.second));
                    fout << "first:\t\t" << genName + population.back().get()->name << "\n";
                    population.emplace_back(std::make_unique<ship_physics>(*temp->first.get(), *inner_temp->first.get(),
                                                                           1-dmnc,map->start_point.first,map->start_point.second));
                    fout << "second:\t\t" << genName + population.back().get()->name << "\n\n";
                }
            }
        }
        dmnc+=0.2;
    }

    //проверка на антихриста
    for(auto temp = population.begin(); temp!=population.end(); ){
        if(!temp->get()->viable()){
            population.erase(temp);
        } else ++temp;
    }
    //время менять имена
    for(auto &shp: population){
        names.emplace_back(genName + shp->name);
    }
    //переезд родителей
    for(auto &par: newGenParents){
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second,par.first.get()->getBrain()));
        population[population.size()-1]->set_id(par.first->id);
        names.emplace_back(par.second);
    }
    //переезд учащихся
    fout<<"Found learners:\n";
    for(auto &ler: newGenLearners){
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second,ler.first.get()->getBrain()));
        population[population.size()-1]->set_id(ler.first->id);
        names.emplace_back(ler.second);
        fout << genName + ler.second << "\n";
    }
    //завоз рандомов
    for (int i = population.size(); i < generation; ++i){
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second));
        names.emplace_back(genName + population.back().get()->name);
    }
    //избавляемся от остаточных галлюцинаций
    for(auto &ship:population){
        ship->initial_fix();
    }

    fout.close();
    clock = 0;
    cnnct();
}

evolution::~evolution(){

}

void evolution::evolution_stat()
{
    ++clock;

    if(clock==5){
        for(auto &i: population){
            if(i->fuel == 2200){
                i->operational = false;
                i->can_be_parrent = false;
                i->autist = true;
            }
        }
    }

    if(clock==25){
        for(auto &i: population){
            if(i->velocity_sum<=min_speed-98){
                i->operational = false;
                i->can_be_parrent = false;
                i->autist = true;
            }
        }
    }

    if(clock==700){
        for(auto &i: population){
            if(i->velocity_sum<=min_speed-40){
                i->operational = false;
                i->can_be_parrent = false;
                i->autist = true;
            }
        }
    }

    dscnnct();

    bool ready_to_evolve = true;
    for(auto &i: population){
        if(i->operational){
            ready_to_evolve = false;
        }
    }

    if(ready_to_evolve || clock==2000){
        for(auto &i: population){
            if(i->get_position().second>300){
                i->can_be_parrent = false;
            } /*else if(i->distance_to_finish <= 450){
                i->can_be_parrent = true;
            }*/
        }
        evolve();
    }
}


void evolution::cnnct(std::shared_ptr<QTimer> &timer)
{
    this->timer=timer;
    int t = population.size();
    for(int i = 0; i < t; i++){
        update_connections.emplace_back(QObject::connect(timer.get(), &QTimer::timeout,
                                                         [=](){population[i]->update(*map.get());}));

        think_n_do_connections.emplace_back(QObject::connect(timer.get(), &QTimer::timeout,
                                                             [=](){population[i]->think_n_do();}));
    }
}

void evolution::cnnct(std::shared_ptr<QTimer> &timer, std::shared_ptr<pathway> &pthw)
{
    map=pthw;
    this->timer=timer;
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
