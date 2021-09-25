#ifndef USY_H
#define USY_H

#include<QObject>
#include<QGraphicsItemGroup>

class usy: public QObject, public QGraphicsItemGroup
{

Q_OBJECT
public:
    explicit usy(QObject *parent = 0);
    ~usy();


signals:
    public slots:
    void plkcheck(int &p1, int &p2, int &p3, int &p4, QVector<QGraphicsLineItem *> &zelen);

private:
    QGraphicsLineItem *palka1 = new QGraphicsLineItem(0,0,1,0);
    QGraphicsLineItem *palka2 = new QGraphicsLineItem(0,0,20,1);
    QGraphicsLineItem *palka3 = new QGraphicsLineItem(0,0,1,0);
    QGraphicsLineItem *palka4 = new QGraphicsLineItem(0,0,-20,1);
    int temp;


};

#endif // USY_H
