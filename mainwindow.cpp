#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = std::make_unique<QGraphicsScene>();

    for(int i = 0; i < 50; i++)
    {
        korablik.emplace_back();
        neuron_1.emplace_back();
        neuron_2.emplace_back();
        neuron_3.emplace_back();
        neuron_4.emplace_back();
        korablik[i] = std::make_unique<ship_physics>(2000,2000);
    }

    timer = std::make_unique<QTimer>();

    map = std::make_unique<polygon>();
    map->add_point(2600,2600);
    map->add_point(2600,1600);
    map->add_point(1600,1600);
    map->add_point(1600,2600);

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
        scene->addLine(pol.vertexes[i_dop].first,pol.vertexes[i_dop].second,
                       pol.vertexes[i].first,pol.vertexes[i].second);
        i++;
        i_dop = i-1;
        if(i>=pol.vertexes.size()){i=0;j++;}
    }
}


void MainWindow::on_pushButton_clicked()
{
    ui->graphicsView->setScene(scene.get());
    for(int i = 0; i < 50; i++){
        neuron_1[i] = (rand() % 100) / 60.0;
        neuron_2[i] = (rand() % 100) / 60.0;
        neuron_3[i] = (rand() % 100) / 60.0;
        neuron_4[i] = (rand() % 100) / 60.0;
    }

    for(int i = 0; i < 50; i++){
        connect(timer.get(), &QTimer::timeout,  [=](){korablik[i]->update(*map);});
        connect(timer.get(), &QTimer::timeout,  [=](){korablik[i]->apply_brain_command(neuron_1[i], neuron_2[i], neuron_3[i], neuron_4[i]);});
    }

    connect(timer.get(), SIGNAL(timeout()), this, SLOT(painter()));

    timer->start(0);

}

void MainWindow::painter()
{
    scene->clear();
    for(int i = 0; i < 50; i++){
        qdraw_polygon(korablik[i]->body,scene.get());
        qdraw_polygon(*map,scene.get());

        scene->addLine(korablik[i]->point_seen[0].first,korablik[i]->point_seen[0].second,
                korablik[i]->point_seen[1].first,korablik[i]->point_seen[1].second);
        scene->addLine(korablik[i]->point_seen[2].first,korablik[i]->point_seen[2].second,
                korablik[i]->point_seen[3].first,korablik[i]->point_seen[3].second);
        scene->addLine(korablik[i]->point_seen[4].first,korablik[i]->point_seen[4].second,
                korablik[i]->point_seen[5].first,korablik[i]->point_seen[5].second);

        scene->addEllipse(korablik[i]->point_seen[0].first-10,korablik[i]->point_seen[0].second-10,20,20);
        scene->addEllipse(korablik[i]->point_seen[1].first-10,korablik[i]->point_seen[1].second-10,20,20);
        scene->addEllipse(korablik[i]->point_seen[2].first-10,korablik[i]->point_seen[2].second-10,20,20);
        scene->addEllipse(korablik[i]->point_seen[3].first-10,korablik[i]->point_seen[3].second-10,20,20);
        scene->addEllipse(korablik[i]->point_seen[4].first-10,korablik[i]->point_seen[4].second-10,20,20);
        scene->addEllipse(korablik[i]->point_seen[5].first-10,korablik[i]->point_seen[5].second-10,20,20);

        ui->lineEdit->setText(QString::number(korablik[i]->abs_velocity));
        ui->lineEdit_2->setText(QString::number(korablik[i]->get_position().first));
        ui->lineEdit_3->setText(QString::number(korablik[i]->get_position().second));
        ui->lineEdit_4->setText(QString::number(korablik[i]->collided));
    }
}



void MainWindow::on_pushButton_2_clicked()
{
    //korablik->move_by_distance(10);
    tmblr_1 =! tmblr_1;
    neuron_1[0] = int(tmblr_1);
}


void MainWindow::on_pushButton_3_clicked()
{
    //korablik->move_by_distance(-10);
    tmblr_2 =! tmblr_2;
    neuron_2[0] = int(tmblr_2);
}


void MainWindow::on_pushButton_5_clicked()
{
    //korablik->rotate_by(-M_PI/12);
    tmblr_4 =! tmblr_4;
    neuron_4[0] = int(tmblr_4);
}


void MainWindow::on_pushButton_4_clicked()
{
    //korablik->rotate_by(M_PI/12);
    tmblr_3 =! tmblr_3;
    neuron_3[0] = int(tmblr_3);
}


void MainWindow::on_pushButton_6_clicked()
{
    //    korablik->move_by_distance(20);
    //    korablik->update(*map);
    this->painter();
}


void MainWindow::on_pushButton_7_clicked()
{
    for(int i = 0; i < 50; i++){
        korablik[i]->update(*map);
    }
}

