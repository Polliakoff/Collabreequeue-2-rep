#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ship_physics.h"
#include <QGraphicsScene>
#include <QTimer>
#include <vector>
#include <cstdlib>

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
    //QGraphicsScene *scene;
    vector <std::unique_ptr<ship_physics>> korablik;
    std::unique_ptr<polygon> map;
    std::unique_ptr<QTimer> timer;
    vector <double> neuron_1, neuron_2, neuron_3, neuron_4;
    bool tmblr_1 = false, tmblr_2 = false, tmblr_3 = false, tmblr_4 = false;
};
#endif // MAINWINDOW_H
