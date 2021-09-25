#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);

    model1 = new QStandardItemModel(6,6);
    ui->tableView->setModel(model1);

    model2 = new QStandardItemModel(6,6);
    ui->tableView_2->setModel(model2);

    model3 = new QStandardItemModel(30,1);
    ui->tableView_3->setModel(model3);


    model3->setHeaderData(0, Qt::Horizontal, "Cчета");

    timesoftimer = 0;
    zelchet = 0;
    check_zelchet = zelchet;
    a = new int(2);
    b = new int(2);
    pl1 = new int(1);
    pl2 = new int(1);
    pl3 = new int(1);
    pl4 = new int(1);
    c = new double(0);
    d = new double(0);
    scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);


    timer = new QTimer();

    xl = new QVector<int>(111);
    xr = new QVector<int>(111);
    yl = new QVector<int>(111);
    yr = new QVector<int>(111);
    xo = new QVector<int>(111);
    yo = new QVector<int>(111);
    xp = new QVector<int>(111);
    yp = new QVector<int>(111);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    *a = 3;
}

void MainWindow::on_pushButton_3_clicked()
{
    *a = 1;
}
void MainWindow::on_pushButton_2_clicked()
{
    *b = 1;
}
void MainWindow::on_pushButton_4_clicked()
{
    *b = 3;
}
void MainWindow::on_pushButton_5_clicked()
{
    *a = 2;
}
void MainWindow::on_pushButton_6_clicked()
{
    *b = 2;
}
void MainWindow::on_pushButton_7_clicked()
{
    timer->stop();
}
void MainWindow::on_pushButton_8_clicked()
{
    timer->start(0);
}



//отрисовка карты
void MainWindow::on_action_triggered()
{
    int prov1 = 0;
    int prov2 = 0;
    int prov3 = 0;
    int prov4 = 0;
    scene->clear();
    for(int i = 0; i<20; i++){
        generate(*xl,*xr,*yl,*yr,*xo,*yo);
        ctpts(*xl,*xr,*yl,*yr,*xp,*yp);
    }
    ////--------------------------------------левый берег
    for(int i = 0; i<99;i++){
        prov1=3*((*xl)[i]);
        prov2=3*((*yl)[i]);
        prov3=3*(*xl)[i+1];
        prov4=3*(*yl)[i+1];
        if((prov2>0) && (prov4>0)) scene->addLine(prov1,prov2,prov3,prov4,QPen(Qt::blue));
        ////--------------------------------------правый берег
        prov1=3*(*xr)[i];
        prov2=3*(*yr)[i];
        prov3=3*(*xr)[i+1];
        prov4=3*(*yr)[i+1];
        if((prov2>0) && (prov4>0)) scene->addLine(prov1,prov2,prov3,prov4,QPen(Qt::blue));
        ////--------------------------------------острова
        prov1=3*(*xo)[i];
        prov2=3*(*yo)[i];
        prov3=3*(*xo)[i+1];
        prov4=3*(*yo)[i+1];
        if((prov2>0) && (prov4>0)) scene->addLine(prov1,prov2,prov3,prov4,QPen(Qt::blue));


        ////--------------------------------------инициализируем контрольные линии

        prov1=3*(*xp)[i];
        prov2=3*(*yp)[i];
        prov3=3*(*xp)[i+1];
        prov4=3*(*yp)[i+1];
        if((prov2>0) && (prov4>0))
        {
            temp = new QGraphicsLineItem(prov1,prov2,prov3,prov4);
            zelen.push_back(temp);

        }

    }

    ////--------------------------------------барьер сзади

    prov1=3*(*xl)[0];
    prov2=3*(*yl)[0];
    prov3=3*(*xr)[0];
    prov4=3*(*yr)[0];
    scene->addLine(prov1,prov2 ,prov3,prov4,QPen(Qt::blue));

    ////--------------------------------------отрисовываем контрольные линии
    for(auto i:zelen){
        scene->addItem(i);
    }

    for(int i = 1; i<zelen.size(); i++)
    {
        zelen[i]->setVisible(false);
    }




}

//считываем скорость (и не только)
void MainWindow::gauges_controls_algos(){

    ///gauges------------------------------------------------------
    ui->lineEdit_11->setText(QString::number(collabrik->lazy_count));
    ui->lineEdit_10->setText(QString::number(collabrik->fuel));
    ui->lineEdit_9->setText(QString::number(gen + 1));
    ui->lineEdit_8->setText(QString::number(count + 1));
    ui->lineEdit_7->setText(QString::number(zelchet));
    ui->lineEdit->setText(QString::number(-(*c)));
    ui->lineEdit_2->setText(QString::number(*d));
    ui->lineEdit_3->setText(QString::number(*pl1));
    ui->lineEdit_4->setText(QString::number(*pl2));
    ui->lineEdit_5->setText(QString::number(*pl3));
    ui->lineEdit_6->setText(QString::number(*pl4));

    ///центровка на коллабрике----------------------------------------
    ui->graphicsView->centerOn(collabrik);
    ///исчезающие контрольные линии---------------------------------------------------
    if(check_zelchet!=zelchet){

        if(zelchet == 0){
            zelen[check_zelchet]->setVisible(false);
            zelen[0]->setVisible(true);
            check_zelchet = zelchet;
        }
        else
        {
            if(zelchet==zelen.size()){
                zelen[zelchet-1]->setVisible(false);
            }

            else{

                zelen[zelchet-1]->setVisible(false);
                zelen[zelchet]->setVisible(true);
                check_zelchet = zelchet;
            }
        }
    }
    ///веса---------------------------------------------------


    for(int j = 0; j < 6; j++)
    {
        for(int k = 0; k < 6; k++)
        {
            ves_temp1 = (collabrik->izvilina)[count].w01.weight[j][k];
            ind1 = model1->index(j,k);
            model1->setData(ind1, ves_temp1);

            ves_temp2 = (collabrik->izvilina)[count].w12.weight[j][k];
            ind2 = model2->index(j,k);
            model2->setData(ind2, ves_temp2);

        }
    }


    for(unsigned int m = 0; m<30; m++){

        score_temp = collabrik->score[m];
        ind3 = model3->index(m,0);
        if(score_temp <= 1)
        {
            model3->setData(ind3, "-");
        }else
        {
            model3->setData(ind3, score_temp);
        }

    }





}
//добавляем коллабрик, ставим его на начало
void MainWindow::on_action_2_triggered()
{

    collabrik = new ship;

    scene->addItem(collabrik);
    collabrik->setPos(600 * 1.5 ,1180 * 1.5);


}

//подключаем тик таймер к слотам которые нужно обновлять и стартуем его
void MainWindow::on_action_3_triggered()
{

    if(timesoftimer == 0){
        connect(timer, &QTimer::timeout,  [=](){collabrik->gtime(*a,*b,*c,*d,zelen,zelchet,*pl1,*pl2,*pl3,*pl4,count,gen);});
        connect(timer, SIGNAL(timeout()), this, SLOT(gauges_controls_algos()));
        connect(timer, &QTimer::timeout,  [=](){collabrik->palki->plkcheck(*pl1,*pl2,*pl3,*pl4,zelen);});
    }




    timer->start(0);
    timesoftimer++;


}


void MainWindow::on_pushButton_9_clicked()
{

}

void MainWindow::on_pushButton_10_clicked()
{
    timer->stop();
    scene->clear();
    zelen.clear();
    zelchet = 0;

}

void MainWindow::on_pushButton_11_clicked()
{
    collabrik->e=true;
}

void MainWindow::on_action_4_triggered()
{
    int prov1 = 0;
    int prov2 = 0;
    int prov3 = 0;
    int prov4 = 0;
    scene->clear();
    for(int i = 0; i<20; i++){
        generate(*xl,*xr,*yl,*yr,*xo,*yo);
        ctpts(*xl,*xr,*yl,*yr,*xp,*yp);
    }

    for(int i = 2; i<99; i++){

        (*xl)[i] = 250;
        (*xr)[i] = 350;
        (*xo)[i] = -1;
        (*yo)[i] = -1;

    }

    for(int i = 0; i<98; i+=2){
        (*xp)[i] = 250;
        (*xp)[i+1] = 350;

    }


    for(int i = 0; i<99;i++){
        prov1=3*((*xl)[i]);
        prov2=3*((*yl)[i]);
        prov3=3*(*xl)[i+1];
        prov4=3*(*yl)[i+1];
        if((prov2>0) && (prov4>0)) scene->addLine(prov1,prov2,prov3,prov4,QPen(Qt::blue));

        prov1=3*(*xr)[i];
        prov2=3*(*yr)[i];
        prov3=3*(*xr)[i+1];
        prov4=3*(*yr)[i+1];
        if((prov2>0) && (prov4>0)) scene->addLine(prov1,prov2,prov3,prov4,QPen(Qt::blue));

        prov1=3*(*xo)[i];
        prov2=3*(*yo)[i];
        prov3=3*(*xo)[i+1];
        prov4=3*(*yo)[i+1];
        if((prov2>0) && (prov4>0)) scene->addLine(prov1,prov2,prov3,prov4,QPen(Qt::blue));



        prov1=3*(*xp)[i];
        prov2=3*(*yp)[i];
        prov3=3*(*xp)[i+1];
        prov4=3*(*yp)[i+1];
        if((prov2>0) && (prov4>0))
        {
            temp = new QGraphicsLineItem(prov1,prov2,prov3,prov4);
            zelen.push_back(temp);

        }

    }



    prov1=3*(*xl)[0];
    prov2=3*(*yl)[0];
    prov3=3*(*xr)[0];
    prov4=3*(*yr)[0];
    scene->addLine(prov1,prov2 ,prov3,prov4,QPen(Qt::blue));

    for(auto i:zelen){
        scene->addItem(i);
    }

    for(int i = 1; i<zelen.size(); i++)
    {
        zelen[i]->setVisible(false);
    }
}



