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
    straight_line test(2,1,1,2);
    test.rotate(M_PI/4);
    ui->lineEdit->setText(QString::number(test.direction(0)));
    ui->lineEdit_2->setText(QString::number(test.direction(1)));
    ui->lineEdit_3->setText(QString::number(test.point(0)));
    ui->lineEdit_4->setText(QString::number(test.point(1)));
}

