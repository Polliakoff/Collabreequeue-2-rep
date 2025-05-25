#include "pathway.h"

pathway::pathway()
{
    make_my_way();
}

pathway::pathway(const QString &geoFile)
{
    geojson_loaded = load_geojson(geoFile);  // true, если файл успешно прочитан
    make_my_way();
}

pathway::~pathway(){}

void pathway::make_my_way()
{
    if (geojson_loaded) {
        for (size_t i = 0; i < glacier_x.size(); ++i)
            add_point(glacier_x[i], glacier_y[i]);
    }
    else {
        if(generator_switch == true)
        {
            generator();
        }

        if(generated == false)
        {
            final_point = std::make_pair(0,125);
            start_point = std::make_pair(575,600);
            add_point(-110,0);
            add_point(700,0);
            add_point(700,700);
            add_point(450,700);
            add_point(450,250);
            add_point(-110,250);
        }else
        {
            for (int i = 0; i <= l_count + r_count + 1; i++)
            {
                add_point(glacier_x[i], glacier_y[i]);
            }
        }
    }
}

bool pathway::load_geojson(const QString &fileName)
{
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))  return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError)   return false;

    QJsonObject root = doc.object();
    if (root["type"] != "FeatureCollection")     return false;

    /* --- извлекаем границы полигона "safe" и линию "track" --- */
    QVector<QPointF> safeRing;
    QPointF trackStart, trackEnd;
    for (const QJsonValue &v : root["features"].toArray()) {
        QJsonObject feat = v.toObject();
        QString kind = feat["properties"].toObject()["kind"].toString();
        QJsonObject geom = feat["geometry"].toObject();

        if (kind == "safe" && geom["type"] == "Polygon") {
            QJsonArray ring = geom["coordinates"].toArray().first().toArray();
            int n = ring.size();
            // последний элемент — тот же, что и первый, его пропускаем
            for (int i = 0; i < n - 1; ++i) {
                QJsonArray a = ring[i].toArray();
                safeRing.append(QPointF(a[0].toDouble(), a[1].toDouble())); // lon,lat
            }
        }
        if (kind == "track" && geom["type"] == "LineString") {
            QJsonArray line = geom["coordinates"].toArray();
            if (!line.isEmpty()) {
                QJsonArray a0 = line.first().toArray();
                QJsonArray a1 = line.last ().toArray();
                trackStart = QPointF(a0[0].toDouble(), a0[1].toDouble());
                trackEnd   = QPointF(a1[0].toDouble(), a1[1].toDouble());
            }
        }
    }
    if (safeRing.isEmpty())  return false;

    // находим среднюю широту для расчёта «метров на градус долгот»
    double lat0 = 0.0;
    for (const QPointF &p : safeRing) lat0 += p.y();
    lat0 /= safeRing.size();  // средняя широта
    const double m_per_deg_lat = 110574.0;                                // метры/° широты
    const double m_per_deg_lon = 111320.0 * std::cos(lat0 * M_PI/180.0);  // метры/° долготы
    // находим минимальные координаты (для смещения в ноль)
    double minLon = safeRing[0].x(), minLat = safeRing[0].y(),
        maxLat = safeRing[0].y();
    for (const QPointF &p : safeRing) {
        minLon = std::min(minLon, p.x());
        minLat = std::min(minLat, p.y());
        maxLat = std::max(maxLat, p.y());
    }
    // функция маппинга: 1° → m_per_deg_* метров, судно остаётся ±10 м
    auto mapPt = [&](const QPointF &q)->QPointF {
        double x = (q.x() - minLon) * m_per_deg_lon;
        double y = (maxLat - q.y()) * m_per_deg_lat;
        return QPointF(x, y);
    };

    glacier_x.clear();
    glacier_y.clear();
    for (const QPointF &p : safeRing) {
        QPointF m = mapPt(p);
        glacier_x.push_back(m.x());
        glacier_y.push_back(m.y());
    }
    l_count = glacier_x.size() / 2;
    r_count = glacier_x.size() - l_count - 1;
    generated = true;          // «как будто» сгенерирована

    // --- определяем центроид safe-полигона (в метрах) ---
    double cx = 0.0, cy = 0.0;
    for (double x : glacier_x) cx += x;
    for (double y : glacier_y) cy += y;
    cx /= glacier_x.size();
    cy /= glacier_y.size();

    /*------------------------------------------------------------------
        1. базовая точка (как раньше)  →  s0                           */
    QPointF s0;
    if (!trackStart.isNull())
        s0 = mapPt(trackStart);
    else
        s0 = QPointF(cx, cy);

    /* 2. смещаем к центроиду на SHIFT и ДОПОЛНИТЕЛЬНО так,
          чтобы отступ от любого ребра ≥ SAFE_MARGIN                  */
    auto pointToSeg = [](double px, double py,
                         double x1,double y1,double x2,double y2) -> double
    {
        double vx = x2-x1,  vy = y2-y1;
        double t = ((px-x1)*vx + (py-y1)*vy) / (vx*vx+vy*vy);
        t = std::clamp(t,0.0,1.0);
        double dx = x1 + t*vx - px;
        double dy = y1 + t*vy - py;
        return std::hypot(dx,dy);
    };

    double vx0 = cx - s0.x();
    double vy0 = cy - s0.y();
    double len0 = std::hypot(vx0, vy0);
    if (len0 > 1e-6) { vx0 = vx0/len0;  vy0 = vy0/len0; }

    QPointF spawn = s0 + QPointF(vx0*SHIFT, vy0*SHIFT);

    /* итеративно отталкиваемся от стен, пока минимальное
      расстояние не станет ≥ SAFE_MARGIN                           */
    auto minDist = [&] (const QPointF &p)->double {
        double mn = 1e9;
        int n = glacier_x.size();
        for (int i=0;i<n;++i)
            mn = std::min(mn,
                          pointToSeg(p.x(),p.y(),
                                     glacier_x[i], glacier_y[i],
                                     glacier_x[(i+1)%n], glacier_y[(i+1)%n]));
        return mn;
    };
    while (minDist(spawn) < SAFE_MARGIN)
        spawn += QPointF(vx0, vy0) *
                 (SAFE_MARGIN - minDist(spawn) + 1.0);

    /* 3. финальная точка финиша (если trackEnd был)                   */
    // QPointF finish;
    // if (!trackEnd.isNull())
    //     finish = mapPt(trackEnd);
    // else
    //     finish = QPointF(cx, glacier_y.front());

    QPointF f0;
    if (!trackEnd.isNull())
        f0 = mapPt(trackEnd);
    else
        f0 = QPointF(cx, glacier_y.front());

    // аналогично старту: смещение к центроиду + safe margin
    double vx1 = cx - f0.x();
    double vy1 = cy - f0.y();
    double len1 = std::hypot(vx1, vy1);
    if (len1 > 1e-6) { vx1 /= len1;  vy1 /= len1; }

    QPointF finish = f0 + QPointF(vx1*SHIFT, vy1*SHIFT);
    // отталкиваем от стен, пока не окажемся в_safe_zone_
    while (minDist(finish) < SAFE_MARGIN) {
        finish += QPointF(vx1, vy1) *
                  (SAFE_MARGIN - minDist(finish) + 1.0);
    }

    start_point = std::make_pair(spawn.x(), spawn.y());
    final_point = std::make_pair(finish.x(), finish.y());

    /*------------------------------------------------------------------
        4. выбираем курс на цель */

    double dx = final_point.first  - start_point.first;
    double dy = final_point.second - start_point.second;
    spawn_heading = std::atan2(dx, dy) + M_PI;
    if (spawn_heading > M_PI) spawn_heading -= 2*M_PI;
    else if (spawn_heading <= -M_PI) spawn_heading += 2*M_PI;

    return true;
}

void pathway::generator()
{
    generated = true;

    vector <int> xmain;
    vector <int> ymain;
    vector <int> xl;
    vector <int> xr;
    vector <int> yl;
    vector <int> yr;
    int i;
    int a, b;

    // Стартовые координаты
    xl.emplace_back(450);
    yl.emplace_back(750);
    xr.emplace_back(250);
    yr.emplace_back(750);

    xmain.emplace_back(350);
    ymain.emplace_back(600);
    xmain.emplace_back(350);
    ymain.emplace_back(565);

    l_count = 0;
    r_count = 0;
    main_count = 1;

    for (i = 2; i < 30; i++)
    {
        // Основной путь, вокруг которого строется проход

        double main_angle = vectors_angle(0, -1, xmain[i - 1] - xmain[i - 2], ymain[i - 1] - ymain[i - 2]);

        a = QRandomGenerator::global()->bounded(30);
        b = QRandomGenerator::global()->bounded(30);
        xmain.emplace_back(xmain[i - 1] + 60*sin(main_angle + M_PI/100*(a - b)));
        a = QRandomGenerator::global()->bounded(30);
        b = QRandomGenerator::global()->bounded(30);
        ymain.emplace_back(ymain[i - 1] - 60*cos(main_angle + M_PI/100*(a - b)));
    }
    ///------------------------------------------------------------------------------------------------------------

    for (i = 1; i < 30; i++)
    {
        // Основные стены прохода
        l_count++;
        r_count++;
        a = QRandomGenerator::global()->bounded(25);
        b = QRandomGenerator::global()->bounded(25);
        xl.emplace_back(xmain[i] - 2*(ymain[i] - ymain[i - 1]) + a - b);
        a = QRandomGenerator::global()->bounded(25);
        b = QRandomGenerator::global()->bounded(25);
        yl.emplace_back(ymain[i] + 2*(xmain[i] - xmain[i - 1]) + a - b);
        a = QRandomGenerator::global()->bounded(25);
        b = QRandomGenerator::global()->bounded(25);
        xr.emplace_back(xmain[i] + 2*(ymain[i] - ymain[i - 1]) + a - b);
        a = QRandomGenerator::global()->bounded(25);
        b = QRandomGenerator::global()->bounded(25);
        yr.emplace_back(ymain[i] - 2*(xmain[i] - xmain[i - 1]) + a - b);
    }

    for (i = 1; i < 30; i++)
    {
        // Ограничение прохода - устранние тонких щелей и огромных пустых пространств

        if (vector_length(xl[i], yl[i], xr[i], yr[i]) > 200)
        {
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            xl[i] -= (vector_length(xl[i], yl[i], xr[i], yr[i]) - 200)/2 + a - b;
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            yl[i] -= (vector_length(xl[i], yl[i], xr[i], yr[i]) - 200)/2 + a - b;
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            xr[i] -= (vector_length(xl[i], yl[i], xr[i], yr[i]) - 200)/2 + a - b;
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            yl[i] -= (vector_length(xl[i], yl[i], xr[i], yr[i]) - 200)/2 + a - b;
        }

        if (vector_length(xl[i], yl[i], xr[i], yr[i]) < 90)
        {
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            xl[i] += (90 - vector_length(xl[i], yl[i], xr[i], yr[i]))/2 + a - b;
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            yl[i] += (90 - vector_length(xl[i], yl[i], xr[i], yr[i]))/2 + a - b;
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            xr[i] += (90 - vector_length(xl[i], yl[i], xr[i], yr[i]))/2 + a - b;
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);
            yl[i] += (90 - vector_length(xl[i], yl[i], xr[i], yr[i]))/2 + a - b;
        }
    }

    // Перегоняем координаты для удобства в один вектор
    for (i = 0; i <= r_count; i++)
    {
        glacier_x.emplace_back(xr[i]);
        glacier_y.emplace_back(yr[i]);
    }
    for (i = l_count; i >= 0; i--)
    {
        glacier_x.emplace_back(xl[i]);
        glacier_y.emplace_back(yl[i]);
    }

    final_point = std::make_pair(((xl[l_count] + xr[r_count])/2), ((yl[l_count] + yr[r_count])/2));
    start_point = std::make_pair(350,700);
}

void pathway::switcher(bool tmblr_generator)
{
    clear_data();
    generator_switch = tmblr_generator;
    geojson_loaded   = false;
    make_my_way();
}

void pathway::switcher(const QString &geoFile)
{
    clear_data();
    generator_switch = false;            // отключаем генератор
    geojson_loaded   = load_geojson(geoFile);   // пытаемся загрузить файл
    make_my_way();                       // строим границу (если файл не
}                                        // прочитался, вернётся STATIC)

void pathway::clear_data()
{
    glacier_x.clear();
    glacier_y.clear();
    l_count = r_count = main_count = 0;
    vertexes.clear();
    faces.clear();
}
