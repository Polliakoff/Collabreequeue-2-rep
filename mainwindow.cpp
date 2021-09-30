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
    straight_line test_1(-1,3);

    test_1.rotate(M_PI/4);
    test_1.rotate(M_PI/4);
    test_1.rotate(M_PI/4);
    test_1.rotate(M_PI/4);


    ui->lineEdit->setText(QString::number(test_1.k));
    ui->lineEdit_2->setText(QString::number(test_1.b));

}

