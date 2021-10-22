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
    test_ship = std::make_unique<ship_physics>(575,650,0,0);
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

    int t = korablik.size();
    for(int i = 0; i < t; i++){
        update_connections.emplace_back(connect(timer.get(), &QTimer::timeout,  [=](){korablik[i]->update(*map);}));
        think_n_do_connections.emplace_back(connect(timer.get(), &QTimer::timeout,  [=](){korablik[i]->think_n_do();}));
    }
    ///===========тестовый
    test_update_connection = connect(timer.get(), &QTimer::timeout,  [=](){test_ship->update(*map);});
    test_think_n_do_connection = connect(timer.get(), &QTimer::timeout,  [=](){test_ship->dumb_n_do(neuron1, neuron2, neuron3, neuron4);});
    ///===========тестовый
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(painter()));
    //disconnect(timer.get(), &QTimer::timeout,)

    timer->start(0);

}

void MainWindow::painter()
{
    scene->clear();
    int ship_number = 0;
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


        if(shp->get()->collided) {
            korablik.erase(shp);
            disconnect(update_connections[ship_number]);
            update_connections.erase(update_connections.begin() + ship_number);
            disconnect(think_n_do_connections[ship_number]);
            think_n_do_connections.erase(think_n_do_connections.begin() + ship_number);
            //korablik.shrink_to_fit();
        }
        ++shp;
        ++ship_number;
    }

    ///===========тестовый
    qdraw_polygon(test_ship->body,scene.get());
    qdraw_polygon(*map,scene.get());

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

    ui->lineEdit->setText(QString::number(test_ship->path.first));
    ui->lineEdit_2->setText(QString::number(test_ship->path.second));
    ui->lineEdit_3->setText(QString::number(test_ship->abs_velocity));
    ui->lineEdit_4->setText(QString::number(test_ship->to_turn_to*180/M_PI));
    ui->lineEdit_5->setText(QString::number(test_ship->velocity_projection));
    ui->lineEdit_6->setText(QString::number(test_ship->velocity_x));
    ui->lineEdit_7->setText(QString::number(test_ship->velocity_y));
    ui->lineEdit_8->setText(QString::number(test_ship->eyes[2].direction[0]));
    ui->lineEdit_9->setText(QString::number(test_ship->actual_angle));
    ui->lineEdit_10->setText(QString::number(test_ship->fuel));
    scene->addEllipse(test_ship->get_position().first-10,test_ship->get_position().second-10,20,20, QPen(Qt::red));
    scene->addLine(test_ship->get_position().first,test_ship->get_position().second,
                   test_ship->get_position().first+test_ship->path.first,test_ship->get_position().second+test_ship->path.second, QPen(Qt::red));
    scene->addLine(test_ship->get_position().first,test_ship->get_position().second,
                   test_ship->get_position().first+test_ship->velocity_x*50,test_ship->get_position().second+test_ship->velocity_y*50, QPen(Qt::blue));
    scene->addLine(test_ship->get_position().first,test_ship->get_position().second,
                   test_ship->get_position().first+(test_ship->path.first*test_ship->velocity_projection/vector_module(test_ship->path.first,test_ship->path.second))*50,
                   test_ship->get_position().second+(test_ship->path.second*test_ship->velocity_projection/vector_module(test_ship->path.first,test_ship->path.second))*50,
                   QPen(Qt::green));

    if(test_ship->collided) {
        test_ship = std::make_unique<ship_physics>(700,700,0,0);
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
    if (tmblr_time == true) timer->stop();
    else timer->start(0);

}
///===========тестовый
