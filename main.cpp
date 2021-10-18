#include "mainwindow.h"

#include <QApplication>
#include "brain.h"
#include <QIODevice>


int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();

    srand(4541);
    QVector<QByteArray> m_dataStreamExports;
    QVector<brain*> m_sensors;
    m_sensors.append(new brain);
    m_sensors.append(new brain);
    m_sensors[0]->think();


    for (auto &m: m_sensors){
        for (auto &w: m->W){
            int i = 0;
            for (auto &wt: w.reshaped())
                wt=++i;
        }
    }


    m_sensors.append(new brain(*m_sensors[0],*m_sensors[1],0.8));
    //brain b(*m_sensors[0],*m_sensors[1],80);
//    QByteArray content;
//    QDataStream writeStream(&content, QIODevice::WriteOnly);
//    writeStream << *m_sensors[0];

//    m_dataStreamExports.append(content);

//    QDataStream readStream(&m_dataStreamExports[0], QIODevice::ReadOnly);
//    brain sensorImport;
//    readStream >> sensorImport;
    delete m_sensors[0];
    delete m_sensors[1];
    delete m_sensors[2];
}
