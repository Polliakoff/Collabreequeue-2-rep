#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <ship.h>
#include <QTimer>
#include <QVector>
#include <generator.h>
#include <net.h>
#include <neuron.h>
#include <usy.h>
#include <QStandardItemModel>

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

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void gauges_controls_algos();




    void on_action_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_pushButton_11_clicked();

    void on_action_4_triggered();

private:
    Ui::MainWindow *ui;
    QGraphicsScene* scene;
    ship* collabrik;
    int *a;
    int *b;
    int *pl1, *pl2, *pl3,*pl4;
    double *c, *d;
    QTimer  *timer;
    QGraphicsLineItem *temp;
    QVector<int> *xl, *xr, *yl, *yr, *xo, *yo, *xp, *yp;
    QVector<QGraphicsLineItem *> zelen;
    int timesoftimer;
    int zelchet, check_zelchet;
    int count=0;
    int gen=0;
    QStandardItemModel *model1;
    QStandardItemModel *model2;
    QStandardItemModel *model3;
    QModelIndex ind1;
    QModelIndex ind2;
    QModelIndex ind3;
    double ves_temp1;
    double ves_temp2;
    double score_temp;

};
#endif // MAINWINDOW_H
