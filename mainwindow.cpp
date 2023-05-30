#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene = std::make_unique<QGraphicsScene>();

    update_timer = std::make_unique<QTimer>();
    painter_timer = std::make_unique<QTimer>();
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
    ui->pushButton->setEnabled(false);
    ui->checkBox_2->setEnabled(true);
    ui->pushButton_9->setEnabled(false);
    only_test = false;

    if(first_boot){
        ui->checkBox->setEnabled(false);
        ui->pushButton_8->setEnabled(false);
        ship_evolution = std::make_unique<evolution>(150, map);

        ///===========тестовый
        if(!pre_init){
            test_ship = std::make_unique<ship_physics>(map->start_point.first, map->start_point.second, map->final_point.first, map->final_point.second);
        }
        ///===========тестовый

        first_boot = false;
    }

    connect(update_timer.get(), &QTimer::timeout,  [=](){ship_evolution->evolution_stat();});

    ship_evolution->cnnct(update_timer);

    if(!pre_init){
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton_4->setEnabled(true);
        ui->pushButton_5->setEnabled(true);
        ui->pushButton_6->setEnabled(true);
        ui->pushButton_7->setEnabled(true);
        test_ship->fuel+=10000;
        test_update_connection = connect(update_timer.get(), &QTimer::timeout,  [=](){test_ship->update(*map);});
        test_think_n_do_connection = connect(update_timer.get(), &QTimer::timeout,  [=](){test_ship->dumb_n_do(neuron1, neuron2, neuron3, neuron4);});

        connect(painter_timer.get(), SIGNAL(timeout()), this, SLOT(painter()));
        connect(update_timer.get(), SIGNAL(timeout()), this, SLOT(gauges()));

        update_timer->start(1);
        painter_timer->start(100);
    }
}

void MainWindow::painter()
{
    scene->clear();
    qdraw_polygon(*map,scene.get());

    if (!only_test){
        for(auto shp = ship_evolution->population.begin(); shp!=ship_evolution->population.end(); ){
            if(shp->get()->operational){
                qdraw_polygon(shp->get()->body,scene.get());
                if (tmblr_eyes){
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
            }
            ++shp;
        }
    }

    ///===========тестовый
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
    if(!only_test){
            ui->lineEdit_11->setText(QString::fromStdString(ship_evolution->genName));
    }

    ///===========тестовый
    ui->lineEdit->setText(QString::number(test_ship->path.first));
    ui->lineEdit_2->setText(QString::number(test_ship->path.second));
    ui->lineEdit_3->setText(QString::number(test_ship->abs_velocity));

    if(!only_test){
        ui->lineEdit_4->setText(QString::number(ship_evolution->clock));
    }

    ui->lineEdit_5->setText(QString::number(test_ship->fuel_consumption));
    ui->lineEdit_6->setText(QString::number(test_ship->velocity_x));
    ui->lineEdit_7->setText(QString::number(test_ship->velocity_y));
    ui->lineEdit_8->setText(QString::number(test_ship->velocity_sum));
    ui->lineEdit_9->setText(QString::number(test_ship->distance_to_finish));
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
    tmblr_slow_time =! tmblr_slow_time;
    if (tmblr_slow_time == true) {
        ui->pushButton_6->setText("Ускорить");
        update_timer->stop();
        update_timer->start(40);
    }
    else {
        ui->pushButton_6->setText("Замедлить");
        update_timer->stop();
        update_timer->start(1);
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    tmblr_time =! tmblr_time;
    if (tmblr_time == true) {
        ui->pushButton_7->setText("Запсук");
        update_timer->stop();
        painter_timer->stop();
    }
    else {
        ui->pushButton_7->setText("Пауза");
        if(tmblr_slow_time == true) update_timer->start(40);
        else update_timer->start(1);
        painter_timer->start(100);
    }
}

void MainWindow::genNameSet(std::string name)
{
    ui->lineEdit_11->setText(QString::fromStdString(name));
}
///===========тестовый

void MainWindow::on_checkBox_stateChanged()
{
    tmblr_generator =! tmblr_generator;
}

void MainWindow::on_pushButton_8_clicked()
{
    if(first_map){
        ui->pushButton_9->setEnabled(true);
        ui->pushButton->setEnabled(true);
    }

    scene->clear();
    map = std::make_shared<pathway>();
    map->switcher(tmblr_generator);

    qdraw_polygon(*map,scene.get());

    if(first_map){
        ui->graphicsView->setScene(scene.get());
        first_map = false;
    }
}

void MainWindow::on_pushButton_9_clicked()
{
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton_5->setEnabled(true);
    ui->pushButton_6->setEnabled(true);
    ui->pushButton_7->setEnabled(true);
    ui->pushButton_9->setEnabled(false);

    only_test = true;
    pre_init = true;

    test_ship = std::make_unique<ship_physics>(map->start_point.first, map->start_point.second, map->final_point.first, map->final_point.second);

    test_ship->fuel+=10000;
    test_update_connection = connect(update_timer.get(), &QTimer::timeout,  [=](){test_ship->update(*map);});
    test_think_n_do_connection = connect(update_timer.get(), &QTimer::timeout,  [=](){test_ship->dumb_n_do(neuron1, neuron2, neuron3, neuron4);});

    connect(painter_timer.get(), SIGNAL(timeout()), this, SLOT(painter()));
    connect(update_timer.get(), SIGNAL(timeout()), this, SLOT(gauges()));

    update_timer->start(1);
    painter_timer->start(100);
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    tmblr_eyes = !tmblr_eyes;
}
