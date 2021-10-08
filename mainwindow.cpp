#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = std::make_unique<QGraphicsScene>();
    korablik = std::make_unique<ship>(200,200);
    timer = std::make_unique<QTimer>();

    map = std::make_unique<polygon>();
    map->add_point(250,400);
    map->add_point(250,0);
    map->add_point(150,0);
    map->add_point(150,400);
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
    korablik->rotate_by(-3*M_PI/4);

    connect(timer.get(), &QTimer::timeout,  [=](){korablik->update(*map);});
    connect(timer.get(), SIGNAL(timeout()), this, SLOT(painter()));

    timer->start(0);

}

void MainWindow::painter()
{
    scene->clear();
    qdraw_polygon(korablik->body,scene.get());
    qdraw_polygon(*map,scene.get());

    scene->addLine(korablik->point_seen[0].first,korablik->point_seen[0].second,
            korablik->point_seen[1].first,korablik->point_seen[1].second);
    scene->addLine(korablik->point_seen[2].first,korablik->point_seen[2].second,
            korablik->point_seen[3].first,korablik->point_seen[3].second);
    scene->addLine(korablik->point_seen[4].first,korablik->point_seen[4].second,
            korablik->point_seen[5].first,korablik->point_seen[5].second);

    scene->addEllipse(korablik->point_seen[0].first-10,korablik->point_seen[0].second-10,20,20);
    scene->addEllipse(korablik->point_seen[1].first-10,korablik->point_seen[1].second-10,20,20);
    scene->addEllipse(korablik->point_seen[2].first-10,korablik->point_seen[2].second-10,20,20);
    scene->addEllipse(korablik->point_seen[3].first-10,korablik->point_seen[3].second-10,20,20);
    scene->addEllipse(korablik->point_seen[4].first-10,korablik->point_seen[4].second-10,20,20);
    scene->addEllipse(korablik->point_seen[5].first-10,korablik->point_seen[5].second-10,20,20);
}


