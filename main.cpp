#include "mainwindow.h"

#include <QApplication>
#include "brain.h"
#include <QIODevice>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    srand(4541);
    QVector<QByteArray> m_dataStreamExports;
    QVector<brain*> m_sensors;
    m_sensors.append(new brain);
    m_sensors[0]->think();

    QByteArray content;
    QDataStream writeStream(&content, QIODevice::WriteOnly);
    writeStream << *m_sensors[0];

    m_dataStreamExports.append(content);

    QDataStream readStream(&m_dataStreamExports[0], QIODevice::ReadOnly);
    brain sensorImport;
    readStream >> sensorImport;
    delete m_sensors[0];


}
