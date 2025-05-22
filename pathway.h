#ifndef PATHWAY_H
#define PATHWAY_H

#include "polygon.h"
#include <QRandomGenerator>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>

class pathway : public polygon
{
protected:
    void clear_data();                            // ← общее обнуление
public:
    pair<double, double> final_point;
    pair<double, double> start_point;
    vector <double> glacier_x;
    vector <double> glacier_y;

    bool generator_switch = false;
    bool geojson_loaded   = false;
    pathway();
    explicit pathway(const QString &geoFile);
    ~pathway();
    int  l_count = 0, r_count = 0, main_count = 0;
    bool generated = false;
    void make_my_way();
    void generator();
    bool load_geojson(const QString &);         // ← парсер .geojson
    void switcher(bool tmblr_generator);
    void switcher(const QString &geoFile);
};

#endif // PATHWAY_H
