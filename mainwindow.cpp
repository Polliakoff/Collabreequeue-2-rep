#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scene = new QGraphicsScene;
    korablik = new ship(200,200);
    polygon map;
    map.add_point(250,250);
    map.add_point(250,100);
    map.add_point(100,100);
    map.add_point(100,150);
    map.add_point(150,150);
    map.add_point(150,250);
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
//    ship test_ship(200,200);

//    polygon test_poly;
//    test_poly.add_point(250,250);
//    test_poly.add_point(250,100);
//    test_poly.add_point(100,100);
//    test_poly.add_point(100,150);
//    test_poly.add_point(150,150);
//    test_poly.add_point(150,250);

//    QGraphicsScene* scene = new QGraphicsScene;
//    ui->graphicsView->setScene(scene);

//    //scene->addLine(10,100,300,100);

//    test_ship.rotate_by(-M_PI/4);
//    //test_ship.move_by_coords(-100,-100);

//    //auto test_convert = test_ship.convert_to_ship(test_ship.body.vertexes[2]);

//    //test_ship.move_by_distance(100);

//    qdraw_polygon(test_ship.body,scene);
    //    qdraw_polygon(test_poly,scene);
}

void MainWindow::painter()
{
    scene->clear();
    qdraw_polygon(korablik->body,scene);
    qdraw_polygon(*map,scene);
}

