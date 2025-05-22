#include "evolution.h"

using namespace std;

static thread_local std::mt19937 RNG_EVO{ std::random_device{}() };
static thread_local std::uniform_real_distribution<double> U01(0.0, 1.0);

/*  Подбираем P так, чтобы rand ≈ 0.25·G  (минимум 2 родителя).
    rand(G,P) = G − ( 2·P² + 2·P )
-------------------------------------------------------------------- */
int evolution::chooseParentCount(int G)
{
    if (G < 8) return 2;            // тривиальный случай

    auto randCnt = [G](int P){ return G - (2*P*P + 2*P); };
    double g   = static_cast<double>(G);
    double root= (-1.0 + std::sqrt(1.0 + 1.5 * g)) * 0.5;   // решение P²+P=3G/8

    int p0 = std::max(2, static_cast<int>(std::floor(root)));
    int p1 = std::min(G/2, p0 + 1);     // ближайшие целые вокруг корня

    double err0 = std::abs(randCnt(p0) - 0.25*g);
    double err1 = std::abs(randCnt(p1) - 0.25*g);

    return (err1 < err0 ? p1 : p0);
}


evolution::evolution(const int& generation_size, std::shared_ptr<pathway> &pthw)
{
    fout.open("evolution_obj.log");
    map = pthw;
    for(int i = 0; i < generation_size; i++)
    {
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second));
        population.back()->rotate_by( map->get_spawn_heading() );
        names.emplace_back(genName + population.back().get()->name);
    }
    generation = generation_size;
    fout << "";
    fout.close();
}

void evolution::mutate(ship_physics &shp)
{
    brain &br = shp.getBrain();

    double r = U01(RNG_EVO);
    if      (r < P_ADD_CONN)   br.addRandomConnection();
    else if (r < P_ADD_CONN + P_DEL_CONN) br.disableRandomConnection();
    else if (r < P_ADD_CONN + P_DEL_CONN + P_SPLIT) {
        /*  выбираем случайную активную связь, если она есть,
        и «раскалываем» её                                  */
        const auto &E = br.E;
        std::vector<std::tuple<std::size_t,std::size_t,std::size_t>> pool;
        for (std::size_t l = 0; l < E.size(); ++l)
            for (std::size_t i = 0; i < (std::size_t)E[l].rows(); ++i)
                for (std::size_t j = 0; j < (std::size_t)E[l].cols(); ++j)
                    if (E[l](i,j)) pool.emplace_back(l,i,j);
        if (!pool.empty()) {
            auto [li,si,di] = pool[ std::uniform_int_distribution<std::size_t>(0,pool.size()-1)(RNG_EVO) ];
            br.splitConnection(li,si,di);
        }
    }
    else if (r < P_ADD_CONN + P_DEL_CONN + P_SPLIT + P_ADD_LAYER) {
        if (br.S > 1) {
            std::uniform_int_distribution<std::size_t> pos(0, br.S-2);
            br.insertHiddenLayer(pos(RNG_EVO));
        }
    }
    else if (r < P_ADD_CONN + P_DEL_CONN + P_SPLIT + P_ADD_LAYER + P_DEL_LAYER) {
        if (br.S > 3) {                         // хотя бы один скрытый слой останется
            std::uniform_int_distribution<std::size_t> pos(1, br.S-2);
            br.removeHiddenLayer(pos(RNG_EVO));
        }
    }
    else if (r < P_ADD_CONN + P_DEL_CONN + P_SPLIT +
                     P_ADD_LAYER + P_DEL_LAYER + P_NOISE) {
        br.noiseWeights();
    }

    br.pruneIsolatedNeurons();                 // финальная «чистка»
}


void evolution::evolve()
{
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
    const int P_target = chooseParentCount(generation);
    vector<int> index;
    int i = 0;
    for (auto &shp: population){
        if(shp.get()->can_be_parrent){
            index.push_back(i);
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
            newGenParents.push_back(std::make_pair(std::move(population[m.second.second]),m.second.first));
            ++i;
            if (i == P_target)
                break;
        } //отобрали семь лучших
    }

    i=0;

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
                    population.back()->rotate_by( map->get_spawn_heading() );
                    mutate(*population.back());  // <-- МУТАЦИЯ
                    fout << "first:\t\t" << genName + population.back().get()->name << "\n";
                    population.emplace_back(std::make_unique<ship_physics>(*temp->first.get(), *inner_temp->first.get(),
                                                                           1-dmnc,map->start_point.first,map->start_point.second));
                    population.back()->rotate_by( map->get_spawn_heading() );
                    mutate(*population.back());  // <-- МУТАЦИЯ

                    fout << "second:\t\t" << genName + population.back().get()->name << "\n\n";
                }
            }
        }
        dmnc+=0.2;
    }

    //время менять имена
    for(auto &shp: population){
        names.emplace_back(genName + shp->name);
    }
    //переезд родителей (дважды)
    for(int t = 2; t>0; --t){
        for(auto &par: newGenParents){
            population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                                   map->final_point.first,map->final_point.second,par.first.get()->getBrain()));
            population.back()->rotate_by( map->get_spawn_heading() );
            population[population.size()-1]->set_id(par.first->id);
            names.emplace_back(par.second);
        }
    }
    //создание псевдородителей
    for(auto &par: newGenParents){
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second,par.first.get()->getBrain(), true));
        population.back()->rotate_by( map->get_spawn_heading() );
        population[population.size()-1]->set_id(par.first->id);
        names.emplace_back(par.second);
    }

    //создание псевдо-псевдо-родителей
    for(auto &par: newGenParents){
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second,par.first.get()->getBrain()));
        population.back()->rotate_by( map->get_spawn_heading() );
        mutate(*population.back());
        population[population.size()-1]->set_id(par.first->id);
        names.emplace_back(par.second);
    }

    //завоз рандомов
    for (int i = population.size(); i < generation; ++i){
        population.emplace_back(std::make_unique<ship_physics>(map->start_point.first,map->start_point.second,
                                                               map->final_point.first,map->final_point.second));
        population.back()->rotate_by( map->get_spawn_heading() );
        mutate(*population.back());
        names.emplace_back(genName + population.back().get()->name);
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
            if(i->fuel == 4400){
                i->operational = false;
                i->can_be_parrent = false;
            }
        }
    }

    if(clock==25){
        for(auto &i: population){
            if(i->velocity_sum<=min_speed-98){
                i->operational = false;
                i->can_be_parrent = false;
            }
        }
    }

    if(clock==700){
        for(auto &i: population){
            if(i->velocity_sum<=min_speed-40){
                i->operational = false;
                i->can_be_parrent = false;
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
            }
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
        }
        ++i;
        ++num;
    }
}
