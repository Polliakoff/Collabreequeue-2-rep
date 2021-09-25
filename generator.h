#ifndef GENERATOR_H
#define GENERATOR_H

#include <QVector>
#include <QRandomGenerator>


    void generate(QVector<int> &xl,
                  QVector<int> &xr,
                  QVector<int> &yl,
                  QVector<int> &yr,
                  QVector<int> &xo,
                  QVector<int> &yo );
    void ctpts(const QVector<int> &xl,
                     const QVector<int> &xr,
                     const QVector<int> &yl,
                     const QVector<int> &yr,
                     QVector<int> &xp,
                     QVector<int> &yp);


#endif // GENERATOR_H
