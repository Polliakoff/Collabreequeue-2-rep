#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      ship_evolution(600,575,550,0,0)
{
    ui->setupUi(this);

    scene = std::make_unique<QGraphicsScene>();

    ///===========тестовый
    test_ship = std::make_unique<ship_physics>(575,550,0,0);
    ///===========тестовый
    update_timer = std::make_unique<QTimer>();
    painter_timer = std::make_unique<QTimer>();

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

    connect(update_timer.get(), &QTimer::timeout,  [=](){ship_evolution.evolution_stat();});

    //connect(&ship_evolution, SIGNAL(&evolution::valueChanged), this, SLOT(genNameSet(ship_evolution.genName)));

    ship_evolution.cnnct(update_timer, map);
    ///===========тестовый
    test_update_connection = connect(update_timer.get(), &QTimer::timeout,  [=](){test_ship->update(*map);});
    test_think_n_do_connection = connect(update_timer.get(), &QTimer::timeout,  [=](){test_ship->dumb_n_do(neuron1, neuron2, neuron3, neuron4);});
    ///===========тестовый
    connect(painter_timer.get(), SIGNAL(timeout()), this, SLOT(painter()));
    connect(update_timer.get(), SIGNAL(timeout()), this, SLOT(gauges()));
    //disconnect(timer.get(), &QTimer::timeout,)

    update_timer->start(1);
    painter_timer->start(100);

}

void MainWindow::painter()
{
    scene->clear();
    qdraw_polygon(*map,scene.get());
    for(auto shp = ship_evolution.population.begin(); shp!=ship_evolution.population.end(); ){
        if(shp->get()->operational){
            qdraw_polygon(shp->get()->body,scene.get());

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
        }
        ++shp;
    }

    ///===========тестовый
    if(test_ship->operational){
        qdraw_polygon(test_ship->body,scene.get());

        scene->addLine(test_ship->point_seen[0].first,test_ship->point_seen[0].second,
                test_ship->point_seen[1].first,test_ship->point_seen[1].second, QPen(Qt::lightGray));
        scene->addLine(test_ship->point_seen[2].first,test_ship->point_seen[2].second,
                test_ship->point_seen[3].first,test_ship->point_seen[3].second, QPen(Qt::lightGray));
        scene->addLine(test_ship->point_seen[4].first,test_ship->point_seen[4].second,
                test_ship->point_seen[5].first,test_ship->point_seen[5].second, QPen(Qt::lightGray));

        scene->addEllipse(test_ship->point_seen[0].first-10,test_ship->point_seen[0].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(test_ship->point_seen[1].first-10,test_ship->point_seen[1].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(test_ship->point_seen[2].first-10,test_ship->point_seen[2].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(test_ship->point_seen[3].first-10,test_ship->point_seen[3].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(test_ship->point_seen[4].first-10,test_ship->point_seen[4].second-10,20,20, QPen(Qt::lightGray));
        scene->addEllipse(test_ship->point_seen[5].first-10,test_ship->point_seen[5].second-10,20,20, QPen(Qt::lightGray));
    }

    scene->addEllipse(test_ship->get_position().first-10,test_ship->get_position().second-10,20,20, QPen(Qt::red));
    scene->addLine(test_ship->get_position().first,test_ship->get_position().second,
                   test_ship->get_position().first+test_ship->path.first,test_ship->get_position().second+test_ship->path.second, QPen(Qt::red));
    scene->addLine(test_ship->get_position().first,test_ship->get_position().second,
                   test_ship->get_position().first+test_ship->velocity_x*50,test_ship->get_position().second+test_ship->velocity_y*50, QPen(Qt::blue));
    scene->addLine(test_ship->get_position().first,test_ship->get_position().second,
                   test_ship->get_position().first+(test_ship->path.first*test_ship->velocity_projection/vector_module(test_ship->path.first,test_ship->path.second))*50,
                   test_ship->get_position().second+(test_ship->path.second*test_ship->velocity_projection/vector_module(test_ship->path.first,test_ship->path.second))*50,
                   QPen(Qt::green));
    ///===========тестовый
}

void MainWindow::gauges()
{
    ui->lineEdit_11->setText(QString::fromStdString(ship_evolution.genName));

    ///===========тестовый
    ui->lineEdit->setText(QString::number(test_ship->path.first));
    ui->lineEdit_2->setText(QString::number(test_ship->path.second));
    ui->lineEdit_3->setText(QString::number(test_ship->abs_velocity));
    ui->lineEdit_4->setText(QString::number(ship_evolution.clock));
    ui->lineEdit_5->setText(QString::number(test_ship->fuel_consumption));
    ui->lineEdit_6->setText(QString::number(test_ship->velocity_x));
    ui->lineEdit_7->setText(QString::number(test_ship->velocity_y));
    ui->lineEdit_8->setText(QString::number(test_ship->velocity_sum));
    ui->lineEdit_9->setText(QString::number(test_ship->ship_and_velocity_angle));
    ui->lineEdit_10->setText(QString::number(test_ship->fuel));

    if(!test_ship->operational) {
        disconnect(test_update_connection);
        disconnect(test_think_n_do_connection);
    }
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
    tmblr_time =! tmblr_time;
    if (tmblr_time == true) {
        update_timer->stop();
        painter_timer->stop();
    }
    else {
        update_timer->start(1);
        painter_timer->start(200);
    }

}

void MainWindow::genNameSet(std::string name)
{
    ui->lineEdit_11->setText(QString::fromStdString(name));
}
///===========тестовый
