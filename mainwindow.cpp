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


void MainWindow::on_pushButton_clicked()
{

    straight_line test_1(2,1,1,2);
    straight_line test_2(1,1,3,3);
    straight_line test_3(1,0,1,3);
    straight_line test_4(3,0,3,3);
    straight_line test_5(4,1,1,4);

    ui->lineEdit->setText(QString::number(intersect(test_1,test_2)));
    ui->lineEdit_2->setText(QString::number(intersect(test_1,test_3)));
    ui->lineEdit_3->setText(QString::number(intersect(test_3,test_4)));
    ui->lineEdit_4->setText(QString::number(intersect(test_1,test_5)));
}

