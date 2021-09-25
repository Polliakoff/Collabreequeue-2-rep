#include "ship.h"
#include <ctime>
#include <vector>

using std::vector;





ship::ship(QObject *parent):
    QObject(parent), QGraphicsItemGroup()
{
    acs1 = 0;
    acs2 = 0;
    angle = 0;
    setRotation(angle);
    izvilina.emplace_back(6,6,6);
    score.reserve(30);
    for(i = 0; i < 30; i++)
    {
        score[i] = -1;
    }
    izvilina.reserve(30);
    old_izvilina.reserve(30);
    score.emplace_back(1);
    palki=new usy;
    this->addToGroup(palki);
}

ship::~ship()
{
    
}

QRectF ship::boundingRect() const
{
    return QRectF(-40,-40,80,80);
}

void ship::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    
    QPolygon polygon;   // Используем класс полигона, чтобы отрисовать треугольник
    // Помещаем координаты точек в полигональную модель
    polygon << QPoint(0,-20) << QPoint(9,20) << QPoint(-9,20);
    painter->setBrush(Qt::red);     // Устанавливаем кисть, которой будем отрисовывать объект
    painter->drawPolygon(polygon);  // Рисуем треугольник по полигональной модели
    
    Q_UNUSED(option);
    Q_UNUSED(widget)
    
}

QPainterPath ship::shape() const
{
    QPainterPath path;
    QPolygon polygon;
    polygon << QPoint(0,-20) << QPoint(9,20) << QPoint(-9,20);
    path.addPolygon(polygon);
    return path;
    
}




void ship::gtime(int &a, int &b, double &c, double &d,
                 const QVector<QGraphicsLineItem *> zelen,
                 int &zelchet, int &pl1, int &pl2, int &pl3, int &pl4, int &count, int &gen)
{
    
    if(skip == 3)
    {
        
        vector<double> v = {fuel/300.0, angle / 180.0, (double(pl1) - 100) / 50.0, (double(pl2) - 100) / 50.0,  (double(pl3) - 100) / 50.0, (double(pl4) - 100) / 50.0};
        

        a = 2;
        b = 2;

        if(count == 0 && gen != 0){
            temp_v = the_hero.get_in_out(v);
        }
        else{
            temp_v = izvilina[count].get_in_out(v);
        }





        if (temp_v[0].get_value() >= 0.5)
        {
            a = 3;
        }
        
        if (temp_v[1].get_value() >= 0.5)
        {
            a = 1;
        }
        
        if (temp_v[2].get_value() >= 0.5)
        {
            b = 1;
        }
        
        if (temp_v[3].get_value() >= 0.5)
        {
            b = 3;
        }
        
        if (temp_v[4].get_value() >= 0.5)
        {
            b = 2;
        }
        
        if (temp_v[5].get_value() >= 0.5)
        {
            a = 2;
        }

        //----------копирует стартовые значения, чтобы следующие корабли не тормозили на старте----------------------
        if(gen == 0 && count == 0 && fuel == 1500)
        {
            start_temp0 = fuel/300.0;
            start_temp1 = angle / 180.0;
            start_temp2 = (double(pl1) - 100) / 50.0;
            start_temp3 = (double(pl2) - 100) / 50.0;
            start_temp4 = (double(pl3) - 100) / 50.0;
            start_temp5 = (double(pl4) - 100) / 50.0;
        }


        
        skip = 0;
        temp_v.clear();

        if(gen != 0 && count == 1 && score[0]<heroes_score*0.7 && retry_count<5)
        {

            retry_count++;
            count--;
            if( QRandomGenerator::global()->bounded(15) == 5) a = 2;

        }

        else if(retry_count>=5){
            retry_count = 0;
        }



    }






    else{
        skip++;
    }
    
    fuel -= 1;
    
    switch(a){
    case 1: {
        

        acs1+=0.01;
        setPos(mapToParent(0,acs1));
        c = acs1;
        
        break;
    }
        
    case 2:{
        

        
        if(fabs(acs1)<=0.01)acs1=0;
        
        setPos(mapToParent(0,acs1));
        c = acs1;
        break;
    }
        
        
    case 3: {
        acs1-=0.02;
        setPos(mapToParent(0,acs1));
        c = acs1;
        break;
    }
        
    default:{
        break;
    }
    }
    ///--------------------------------------------------------------------------------------
    switch(b){
    case 1: {
        if(acs2>-0.4)
        {
            acs2-=0.02;
        }
        angle+=acs2 * (sin(abs(acs1 * 1.4)) + 0.1);
        setRotation(angle);
        d=acs2 * (sin(abs(acs1 * 1.4)) + 0.1);
        break;
    }
        
    case 2:{
        
        if(acs2!=0){
            
            if(acs2<0) acs2+=0.02;
            else acs2-=0.02;
            
        }
        
        if(fabs(acs2)<=0.01)acs2=0;
        
        angle+=acs2;
        setRotation(angle);
        d=acs2;
        
        
        break;
    }
        
    case 3: {
        if(acs2<0.4)
        {
            acs2+=0.02;
        }
        angle+=acs2 * (sin(abs(acs1 * 1.4)) + 0.1);
        setRotation(angle);
        d=acs2 * (sin(abs(acs1 * 1.4)) + 0.1);
        break;
    }
        
    default:{
        break;
    }
    }
    
    //Подсчет сколько тиков корабль стоит
    if(acs1 == 0)
    {
        timt++;
    }
    
    
    
    ///------------------------------------------------------------------------------------------------------------

    //Тут теперь живет действие при пересечении И вылете из-за таймаута
    if(e == true || (timt >=250 && acs2 != 0) || (timt >= 50 && acs2 == 0) || fuel == 0 || this->y()<=0)
    {



        if((1770-this->y()) > 0)
        {

            score[count] = (1770-this->y())*(1770-this->y()) / 100.0;

            //вторая фаза эволюции (соревнование по времени)
            if(this->y()<=0){
                score[count]*=(1+fuel/1500);
            }

        }
        else
        {
            score[count] = 0.5;
        }
        
        score[count] += 1 + zelchet * zelchet * 50;
        
        

        //        printf("%lf, #%d\n", score[count], count);
        
        zelchet = 0;
        zp1 = 0;
        zp2 = 0;
        zpalka = 0;
        
        fuel = 1500;
        acs1 = 0;
        acs2 = 0;
        angle = 0;
        setRotation(0);
        
        setPos(600 * 1.5 ,1180 * 1.5);
        
        ///--------------------------создание первых весов для кораблей, выборка чувствительных к воздействиям----------
        if(gen == 0){
            izvilina.emplace_back(6,6,6);
            score.emplace_back(1);
            for(int l = 0; l<100000; l++)
            {
                candidate_izv.emplace_back(6,6,6);

                vector<double> v = {rand()%100 / 100.0 * 5, rand()%100 / 100.0 * 5, rand()%100 / 100.0 * 5,
                                    rand()%100 / 100.0 * 5, rand()%100 / 100.0 * 5, rand()%100 / 100.0 * 5};
                temp_v = candidate_izv[0].get_in_out(v);

                if((temp_v[0].get_value() > 0.49 && temp_v[0].get_value() < 0.51)
                        || (temp_v[1].get_value() > 0.49 && temp_v[1].get_value() < 0.51)
                        || (temp_v[2].get_value() > 0.49 && temp_v[2].get_value() < 0.51)
                        || (temp_v[3].get_value() > 0.49 && temp_v[3].get_value() < 0.51)
                        || (temp_v[4].get_value() > 0.49 && temp_v[4].get_value() < 0.51)
                        || (temp_v[5].get_value() > 0.49 && temp_v[5].get_value() < 0.51))
                {

                    // а тут выбираем из них корабли, которые не будут стоять на месте и плыть назад

                    vector<double> v = {start_temp0, start_temp1, start_temp2, start_temp3, start_temp4, start_temp5};
                    temp_v = candidate_izv[0].get_in_out(v);

                    if(temp_v[0].get_value() > 0.50 && temp_v[1].get_value() < 0.50 && temp_v[5].get_value() < 0.50)
                    {
                        for(j = 0; j < 6; j++)
                        {
                            for(k = 0; k < 6; k++)
                            {
                                izvilina[count + 1].w01.weight[j][k] = candidate_izv[0].w01.weight[j][k];
                                izvilina[count + 1].w12.weight[j][k] = candidate_izv[0].w12.weight[j][k];
                            }
                        }
                        candidate_izv.clear();
                        break;
                    }

                }
                candidate_izv.clear();
            }
        }
        ///-----------------------------------------------------------------------------------------------------------
        
        timt = 0;

        count++;
        
        
        ///-----------------------------------------------------------------------------------------------------------

        
        if(count == 30)
        {


            
            sum = 0;
            count = 0;
            
            for(i = 0; i < 30; i++)
            {
                sum += score[i];
                
                old_izvilina.emplace_back(izvilina[i]);

                //-----------------------------------------копирование весов
                for(j = 0; j < 6; j++)
                {
                    for(k = 0; k < 6; k++)
                    {
                        old_izvilina[i].w01.weight[j][k] = izvilina[i].w01.weight[j][k];
                        old_izvilina[i].w12.weight[j][k] = izvilina[i].w12.weight[j][k];
                    }
                }
                //----------------------------------------
                
            }
            izvilina.clear();
            
            
            ///------------------------------------------------------------------------------------------------------------
            best_temp = 0;           // Обнаружение лучшего корабля и отправка его напрямую в
            // следующую генерацию для противодействия вырождению
            for(i = 0; i < 30; i++)
            {
                if(score[i] > best_temp)
                {
                    best_temp = score[i];
                    temp2 = i;
                    
                }
            }
            //            printf("top - #%d\n", temp2);

            if(score[temp2]>lazy_score*1.2)
            {
                lazy_score = score[temp2];

            }

            else if(score[temp2]*0.7<lazy_score)
            {
                lazy_count++;
            }

            //            if((score[temp2]>0.7*heroes_score || score[temp2]<1.2*heroes_score) && gen>=3){
            //                lazy_count++;
            //            }

            if (gen == 0){
                the_hero = old_izvilina[temp2];
                heroes_score = score[temp2];
            }
            else if(score[temp2]> heroes_score){

                the_hero = old_izvilina[temp2];
                heroes_score = score[temp2];
                if(lazy_count !=0){
                    lazy_count--;
                }


            }




            izvilina.emplace_back(the_hero);
            izvilina.emplace_back(old_izvilina[temp2]);
            //------------------------------------------------лучший корраблик, копирование весов в том числе во второй                   колабрик тоже
            for(j = 0; j < 6; j++)
            {
                for(k = 0; k < 6; k++)
                {
                    izvilina[0].w01.weight[j][k] = the_hero.w01.weight[j][k];
                    izvilina[0].w12.weight[j][k] = the_hero.w12.weight[j][k];
                    izvilina[1].w01.weight[j][k] = old_izvilina[temp2].w01.weight[j][k];
                    izvilina[1].w12.weight[j][k] = old_izvilina[temp2].w12.weight[j][k];
                }
            }
            //-------------------------------------------------
            
            temp2 = 0;
            //            printf("viborka v pokolenie #%d\n", gen);
            for(i = 2; i < 30; i++)      // Выборка в следующее поколение и мутации
            {
                temp2 = ship::da_best_ship(sum, score);
                if(temp2 != -1)
                {
                    //                    printf("vibrano = %d, izvilina #%d\n", temp2, i);
                    izvilina.emplace_back(old_izvilina[temp2]);
                    //------------------------------------------------копировние весов
                    for(j = 0; j < 6; j++)
                    {
                        for(k = 0; k < 6; k++)
                        {
                            izvilina[i].w01.weight[j][k] = old_izvilina[temp2].w01.weight[j][k];
                            izvilina[i].w12.weight[j][k] = old_izvilina[temp2].w12.weight[j][k];
                        }
                    }
                    //------------------------------------------------
                    if(gen <= 3)
                    {

                        if(rand()%10 >= 5)
                        {
                            izvilina[i].w01.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }else
                        {
                            izvilina[i].w12.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }

                        if(rand()%10 >= 5)
                        {
                            izvilina[i].w01.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }else
                        {
                            izvilina[i].w12.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }

                        //                        izvilina[i].w01.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        //                        izvilina[i].w12.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        
                    }else
                    {

                        if(rand()%10 >= 5)
                        {
                            izvilina[i].w01.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }else
                        {
                            izvilina[i].w12.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }

                    }

                    if(lazy_count >=3)
                    {
                        if(rand()%10 >= 5)
                        {
                            izvilina[i].w01.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }else
                        {
                            izvilina[i].w12.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                        }

                        if(lazy_count >4){
                            if(rand()%10 >= 5)
                            {
                                izvilina[i].w01.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                            }else
                            {
                                izvilina[i].w12.weight[rand()%6][rand()%6] = (rand()%100 - 50) / 50.0;
                            }
                        }
                    }





                }else
                {
                    i--;
                }
                
            }
            for(i = 0; i < 30; i++)
            {
                score[i] = -1;
            }
            
            
            
            gen++;
            
            old_izvilina.clear();
            
            

        }
        
        

    }
    
    
    ///----------------------------------------------------осознание столкновения--------------
    if((this->collidingItems().size())==6){
        for(auto i:zelen){
            if(this->collidesWithItem(i)){
                
                
                zpalka++;
                e = false;
                
                //-------------------------------------------------счетчик
                if(zp1!=i && zp2!=i){
                    zp2 = zp1;
                    zp1=i;
                    if(zp2!=zp1) zelchet++;
                }
                //-------------------------------------------------
            }
            
            
            
        }
        if(zpalka == 0) e = true;
        zpalka = 0;
    }
    else{
        if(this->collidingItems().size()<6)  e = false;
        else e = true;
    }
    
    
    
    
    
    ///----------------------------------------------------Трение
    if(acs1 > 0)
    {
        acs1 -= acs1 * acs1 / 100;
    }
    if(acs1 < 0)
    {
        acs1 += acs1 * acs1 / 200;
    }
    
    
}

int ship::da_best_ship(double &sum, vector <double> &score)
{
    int i;
    double temp, r;
    temp = 0;
    r = QRandomGenerator::global()->bounded((int)sum);
    //    printf("random ot sum - %lf, sum - %lf, ", r, sum);
    
    for(i = 0; i < 30; i++)
    {
        temp += score[i];
        //        printf("r = %lf, temp = %lf, score = %lf\n", r, temp, score[i]);
        if(temp >= r)
        {
            return i;
        }
    }
    return -1;
}
