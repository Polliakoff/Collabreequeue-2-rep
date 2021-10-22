#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = std::make_unique<QGraphicsScene>();


    for(int i = 0; i < 60; i++)
    {
        korablik.emplace_back(std::make_unique<ship_physics>(575,650,0,0));
    }
///===========тестовый
    korablik.emplace_back(std::make_unique<ship_physics>(575,650,0,0));
///===========тестовый
    timer = std::make_unique<QTimer>();

    map = std::make_unique<pathway>();
    //    map->add_point(250,250);
    //    map->add_point(250,150);
    //    map->add_point(150,150);
    //    map->add_point(150,250);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::qdraw_polygon(const polygon &pol,QGraphicsScene* scene)
{
    size_t i = 1;
    size_t i_dop = i-1;
    int j = 0;
    while(j!=2){
        scene->addLine(pol.vertexes[i_dop].first,
                       pol.vertexes[i_dop].second,
                       pol.vertexes[i].first,
                       pol.vertexes[i].second);
        i++;
        i_dop = i-1;
        if(j==1) j = 2;
        if(i==pol.vertexes.size()){i=0;j=1;}
    }
}


void MainWindow::on_pushButton_clicked()
{
    ui->graphicsView->setScene(scene.get());

    int t = 60;
    for(int i = 0; i < t; i++){      
        update_connections.emplace_back(connect(timer.get(), &QTimer::timeout,  [=](){korablik[i]->update(*map);}));
        think_n_do_connections.emplace_back(connect(timer.get(), &QTimer::timeout,  [=](){korablik[i]->think_n_do();}));
    }
///===========тестовый
    update_connections.emplace_back(connect(timer.get(), &QTimer::timeout,  [=](){korablik[t]->update(*map);}));
    think_n_do_connections.emplace_back(connect(timer.get(), &QTimer::timeout,  [=](){korablik[t]->dumb_n_do(neuron1, neuron2, neuron3, neuron4);}));
///===========тестовый
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(painter()));
    //disconnect(timer.get(), &QTimer::timeout,)

    timer->start(0);

}

void MainWindow::painter()
{
    scene->clear();
    for(auto shp = korablik.begin(); shp!=korablik.end(); ){

        qdraw_polygon(shp->get()->body,scene.get());
        qdraw_polygon(*map,scene.get());

        scene->addLine(shp->get()->point_seen[0].first,shp->get()->point_seen[0].second,
                shp->get()->point_seen[1].first,shp->get()->point_seen[1].second, QPen(Qt::lightGray));
        scene->addLine(shp->get()->point_seen[2].first,shp->get()->point_seen[2].second,
                shp->get()->point_seen[3].first,shp->get()->point_seen[3].second, QPen(Qt::lightGray));
        scene->addLine(shp->get()->point_seen[4].first,shp->get()->point_seen[4].second,
                shp->get()->point_seen[5].first,shp->get()->point_seen[5].second, QPen(Qt::lightGray));

        scene->addEllipse(shp->get()->point_seen[0].first-10,shp->get()->point_seen[0].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(shp->get()->point_seen[1].first-10,shp->get()->point_seen[1].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(shp->get()->point_seen[2].first-10,shp->get()->point_seen[2].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(shp->get()->point_seen[3].first-10,shp->get()->point_seen[3].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(shp->get()->point_seen[4].first-10,shp->get()->point_seen[4].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(shp->get()->point_seen[5].first-10,shp->get()->point_seen[5].second-10,20,20, QPen(Qt::lightGray));


//        if(shp->get()->collided) {
//            korablik.erase(shp);
//            //korablik.shrink_to_fit();
//        }
        ++shp;
    }

///===========тестовый
    ui->lineEdit->setText(QString::number(korablik[60]->path.first));
    ui->lineEdit_2->setText(QString::number(korablik[60]->path.second));
    ui->lineEdit_3->setText(QString::number(korablik[60]->abs_velocity));
    ui->lineEdit_4->setText(QString::number(korablik[60]->to_turn_to*180/M_PI));
    ui->lineEdit_5->setText(QString::number(korablik[60]->velocity_projection));
    ui->lineEdit_6->setText(QString::number(korablik[60]->velocity_x));
    ui->lineEdit_7->setText(QString::number(korablik[60]->velocity_y));
    ui->lineEdit_8->setText(QString::number(korablik[60]->eyes[2].direction[0]));
    ui->lineEdit_9->setText(QString::number(korablik[60]->eyes[2].direction[1]));
    ui->lineEdit_10->setText(QString::number(korablik[60]->fuel));
    scene->addEllipse(korablik[60]->get_position().first-10,korablik[60]->get_position().second-10,20,20, QPen(Qt::red));
    scene->addLine(korablik[60]->get_position().first,korablik[60]->get_position().second,
            korablik[60]->get_position().first+korablik[60]->path.first,korablik[60]->get_position().second+korablik[60]->path.second, QPen(Qt::red));
    scene->addLine(korablik[60]->get_position().first,korablik[60]->get_position().second,
            korablik[60]->get_position().first+korablik[60]->velocity_x*50,korablik[60]->get_position().second+korablik[60]->velocity_y*50, QPen(Qt::blue));
    scene->addLine(korablik[60]->get_position().first,korablik[60]->get_position().second,
            korablik[60]->get_position().first+(korablik[60]->path.first*korablik[60]->velocity_projection/vector_module(korablik[60]->path.first,korablik[60]->path.second))*50,
            korablik[60]->get_position().second+(korablik[60]->path.second*korablik[60]->velocity_projection/vector_module(korablik[60]->path.first,korablik[60]->path.second))*50,
            QPen(Qt::green));
///===========тестовый
}


///===========тестовый
void MainWindow::on_pushButton_2_clicked()
{
    tmblr_1 =! tmblr_1;
    neuron1 = int(tmblr_1);


}


void MainWindow::on_pushButton_3_clicked()
{
    tmblr_2 =! tmblr_2;
    neuron2 = int(tmblr_2);

}


void MainWindow::on_pushButton_5_clicked()
{
    tmblr_4 =! tmblr_4;
    neuron4 = int(tmblr_4);
}


void MainWindow::on_pushButton_4_clicked()
{ 
    tmblr_3 =! tmblr_3;
    neuron3 = int(tmblr_3);
}


void MainWindow::on_pushButton_6_clicked()
{
    //    korablik->move_by_distance(20);
    //    korablik->update(*map);
    this->painter();
}


void MainWindow::on_pushButton_7_clicked()
{
    for(auto &shp: korablik){
        shp->update(*map);
    }
}
///===========тестовый
