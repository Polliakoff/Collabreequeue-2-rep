#include "usy.h"

usy::usy(QObject *parent):
    QObject(parent), QGraphicsItemGroup()
{

    this->addToGroup(palka1);
    this->addToGroup(palka2);
    this->addToGroup(palka3);
    this->addToGroup(palka4);
}

usy::~usy()
{

}

void usy::plkcheck(int &p1, int &p2, int &p3, int &p4, QVector<QGraphicsLineItem *> &zelen){


    // не обновляем если ни с чем не пересекается
    if(((palka1->collidingItems().size())==5)&&
            ((palka2->collidingItems().size())==5)&&
            ((palka3->collidingItems().size())==5)&&
            ((palka4->collidingItems().size())==5)&&
            (p1 == 200) && (p2==200) && (p3==200) && (p4==200) ) return;

    p1=1;
    p2=1;
    p3=1;
    p4=1;
    palka1->setLine(0,0,p1,0);
    palka2->setLine(0,0,20,-p2);
    palka3->setLine(0,0,-p3,0);
    palka4->setLine(0,0,-20,-p4);

    for(int i = 0; i<zelen.size();i++){

        ////---------------------------------------------------------------удлиняем каждую палку на каждом тике


        while(true)
        {
            temp = 0;
            if((palka1->collidingItems().size()==5 ||
                (palka1->collidesWithItem(zelen[i]) && palka1->collidingItems().size()==6)) && (p1<200))
            {
                p1+=10;
                palka1->setLine(0,0,p1,0);
            }else temp++;

            if((palka2->collidingItems().size()==5 ||
                (palka2->collidesWithItem(zelen[i]) && palka2->collidingItems().size()==6)) && (p2<200))
            {
                p2+=10;
                palka2->setLine(0,0,20,-p2);
            }else temp++;

            if((palka3->collidingItems().size()==5 ||
                (palka3->collidesWithItem(zelen[i]) && palka3->collidingItems().size()==6)) && (p3<200))
            {
                p3+=10;
                palka3->setLine(0,0,-p3,0);
            }else temp++;

            if((palka4->collidingItems().size()==5 ||
                (palka4->collidesWithItem(zelen[i]) && palka4->collidingItems().size()==6)) && (p4<200))
            {
                p4+=10;
                palka4->setLine(0,0,-20,-p4);
            }else temp++;

            if(temp == 4)
            {
                break;
            }

        }



    }

}
