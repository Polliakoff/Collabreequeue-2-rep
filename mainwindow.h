#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ship_physics.h"
#include "pathway.h"
#include <QGraphicsScene>
#include <QTimer>
//#include <vector>
#include <cstdlib>
#include "evolution.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void painter();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

private:
    Ui::MainWindow *ui;
    void qdraw_polygon(const polygon& pol, QGraphicsScene* scene);
    std::unique_ptr<QGraphicsScene> scene;
    std::unique_ptr<evolution> ship_evolution;
    //vector <std::unique_ptr<ship_physics>> korablik;
///тестовый
    std::unique_ptr<ship_physics> test_ship;
    QMetaObject::Connection test_update_connection;
    QMetaObject::Connection test_think_n_do_connection;
///тестовый
    std::unique_ptr<pathway> map;
    std::unique_ptr<QTimer> timer;
    double neuron1 = 0, neuron2 = 0, neuron3 = 0, neuron4 = 0;
    bool tmblr_1 = false, tmblr_2 = false, tmblr_3 = false, tmblr_4 = false, tmblr_time = false;
    vector<QMetaObject::Connection> update_connections;
    vector<QMetaObject::Connection> think_n_do_connections;
};
#endif // MAINWINDOW_H
