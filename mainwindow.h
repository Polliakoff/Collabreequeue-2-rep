#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ship.h"
#include <QGraphicsScene>
#include <QTimer>

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

private:
    Ui::MainWindow *ui;
    void qdraw_polygon(const polygon& pol, QGraphicsScene* scene);
    QGraphicsScene *scene;
    ship *korablik;
    polygon *map;
};
#endif // MAINWINDOW_H
