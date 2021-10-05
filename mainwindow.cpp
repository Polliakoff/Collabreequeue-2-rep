#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::qdraw_polygon(const polygon &pol,QGraphicsScene* scene)
//{
//    size_t i = 1;
//    size_t i_dop = i-1;
//    int j = 0;
//    while(j!=2){
//        scene->addLine(test_ship.body.vertexes[i_dop].first,test_ship.body.vertexes[i_dop].second,
//                test_ship.body.vertexes[i].first,test_ship.body.vertexes[i].second);
//        i++;
//        i_dop = i-1;
//        if(i>=test_ship.body.vertexes.size()){i=0;j++;}
//    }
//}


void MainWindow::on_pushButton_clicked()
{
    ship test_ship(200,200);

    polygon test_poly;
    test_poly.add_point(250,250);
    test_poly.add_point(250,150);
    test_poly.add_point(150,150);
    test_poly.add_point(150,250);

    QGraphicsScene* scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);

    //scene->addLine(10,100,300,100);

    test_ship.rotate_by(-M_PI/4);
    //test_ship.move_by_coords(-100,-100);

    //auto test_convert = test_ship.convert_to_ship(test_ship.body.vertexes[2]);

    test_ship.move_by_distance(100);

    size_t i = 1;
    size_t i_dop = i-1;
    int j = 0;
    while(j!=2){
        scene->addLine(test_ship.body.vertexes[i_dop].first,test_ship.body.vertexes[i_dop].second,
                test_ship.body.vertexes[i].first,test_ship.body.vertexes[i].second);
        i++;
        i_dop = i-1;
        if(i>=test_ship.body.vertexes.size()){i=0;j++;}
    }

    i = 1;
    i_dop = i-1;
    j = 0;
    while(j!=2){
        scene->addLine(test_poly.vertexes[i_dop].first,test_poly.vertexes[i_dop].second,
                test_poly.vertexes[i].first,test_poly.vertexes[i].second);
        i++;
        i_dop = i-1;
        if(i>=test_poly.vertexes.size()){i=0;j++;}
    }



}

