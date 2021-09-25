#ifndef SHIP_H
#define SHIP_H


#include <QObject>
#include <QGraphicsItemGroup>
#include <QRandomGenerator>
#include <QPainter>
#include <QtMath>
#include <vector>
#include "net.h"
#include <ctime>
#include <usy.h>
#include <QTimer>

using std::vector;

class ship: public QObject, public QGraphicsItemGroup
{
    Q_OBJECT
public:
    explicit ship(QObject *parent = 0);
    ~ship();


signals:
public slots:

    void gtime(int &a, int &b, double &c, double &d,
               const QVector<QGraphicsLineItem *> zelen, int &zelchet,
               int &pl1, int &pl2, int &pl3, int &pl4, int &count, int &gen);

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
    QPainterPath shape() const override;
private:
    int skip = 0;
    qreal angle;
    double acs1,acs2;
    int zpalka = 0;
    QGraphicsLineItem *zp1;
    QGraphicsLineItem *zp2;


    vector<neuron> temp_v;
    net the_hero;
    double heroes_score;
    double best_temp;
    int timt = 0;            // Счетчик тиков до вылета корабля от неактивности из-за того, что он не отпускает тормоз.
    int gen = 0;            // Счетчик генерации
    int da_best_ship(double &sum, vector <double> &score); //Функция вычленения лучших в зависимости от счета
    double sum;
    int i, j, k, temp2;
    double start_temp0, start_temp1, start_temp2, start_temp3, start_temp4, start_temp5;
    double lazy_score = 0;
    int retry_count = 0;



public:
    usy *palki;
    bool e=false;
    int fuel = 1500;
    vector<net> izvilina;
    vector<net> old_izvilina;
    vector<net> candidate_izv;
    vector<double> score;   // Вектор счета для каждого.
    int lazy_count = 0;
};

#endif // SHIP_H
