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

    qDebug().noquote() << "\n=== Pathway dump =====================";

    for (size_t i = 0; i < vertexes.size(); ++i) {
        qDebug().noquote()
        << "  [" << qSetFieldWidth(2) << i << qSetFieldWidth(0) << "]  ("
        << vertexes[i].first  << "," << vertexes[i].second << ")";
    }

    qDebug().noquote()
        << "----------------------------------------"
        << "\n  start_point : (" << start_point.first  << "," << start_point.second << ")"
        << "\n  final_point : (" << final_point.first  << "," << final_point.second << ")";

    const double DEG = 180.0 / M_PI;
    qDebug().noquote()
        << "  spawn_heading : "
        << spawn_heading          << " rad  ("
        << spawn_heading * DEG    << "°)";
    qDebug().noquote() << "=======================================\n";
}

bool pathway::load_geojson(const QString &fileName)
{
    /* ---------- 1. чтение GeoJSON как было ----------------------- */
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly))  return false;

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (err.error != QJsonParseError::NoError)   return false;

    QJsonObject root = doc.object();
    if (root["type"] != "FeatureCollection")     return false;

    QVector<QPointF> safeRing;
    QPointF trackStart, trackEnd;
    for (const QJsonValue &v : root["features"].toArray())
    {
        QJsonObject feat = v.toObject();
        QString kind = feat["properties"].toObject()["kind"].toString();
        QJsonObject geom = feat["geometry"].toObject();

        if (kind == "safe" && geom["type"] == "Polygon") {
            QJsonArray ring = geom["coordinates"].toArray().first().toArray();
            for (int i=0; i<ring.size()-1; ++i) {
                QJsonArray a = ring[i].toArray();
                safeRing.append(QPointF(a[0].toDouble(), a[1].toDouble()));
            }
        }
        if (kind == "track" && geom["type"] == "LineString") {
            QJsonArray line = geom["coordinates"].toArray();
            if (!line.isEmpty()) {
                trackStart = QPointF(line.first ().toArray()[0].toDouble(),
                                     line.first ().toArray()[1].toDouble());
                trackEnd   = QPointF(line.last  ().toArray()[0].toDouble(),
                                   line.last  ().toArray()[1].toDouble());
            }
        }
    }
    if (safeRing.isEmpty()) return false;

    /* ---------- 2. проекция lon/lat → метры ---------------------- */
    double lat0 = 0.0;
    for (const auto &p : safeRing) lat0 += p.y();
    lat0 /= safeRing.size();

    const double m_per_deg_lat = 110574.0;
    const double m_per_deg_lon = 111320.0 * std::cos(lat0*M_PI/180.0);

    double minLon=safeRing[0].x(), maxLat=safeRing[0].y();
    for (const auto &p : safeRing) {
        minLon = std::min(minLon, p.x());
        maxLat = std::max(maxLat, p.y());
    }
    auto mapPt = [&](const QPointF &q)->QPointF {
        return {(q.x()-minLon)*m_per_deg_lon,
                (maxLat-q.y())*m_per_deg_lat};
    };

    glacier_x.clear(); glacier_y.clear();
    for (const auto &p : safeRing) {
        QPointF m = mapPt(p);
        glacier_x.push_back(m.x());
        glacier_y.push_back(m.y());
    }
    l_count = glacier_x.size()/2;
    r_count = glacier_x.size()-l_count-1;
    generated = true;

    /* ---------- 3. сервисные лямбды ------------------------------ */
    auto inPoly = [&](double x,double y)->bool
    {
        bool inside=false; int n=glacier_x.size();
        for (int i=0,j=n-1;i<n;j=i++)
        {
            double yi=glacier_y[i], yj=glacier_y[j];
            if ( (yi>y) != (yj>y) )
            {
                double xi=glacier_x[i], xj=glacier_x[j];
                if (x < (xj-xi)*(y-yi)/(yj-yi)+xi) inside=!inside;
            }
        }
        return inside;
    };

    auto distSeg = [](double px,double py,
                      double x1,double y1,double x2,double y2)->double
    {
        double vx=x2-x1, vy=y2-y1;
        double t=((px-x1)*vx+(py-y1)*vy)/(vx*vx+vy*vy);
        t=std::clamp(t,0.0,1.0);
        double dx=x1+vx*t-px, dy=y1+vy*t-py;
        return std::hypot(dx,dy);
    };

    auto minDist = [&](double x,double y)->double
    {
        double m=1e9; int n=glacier_x.size();
        for(int i=0;i<n;++i)
            m=std::min(m,distSeg(x,y,
                                    glacier_x[i],glacier_y[i],
                                    glacier_x[(i+1)%n],glacier_y[(i+1)%n]));
        return m;
    };

    /* ---------- 4. функция «внутренний сдвиг» -------------------- */
    auto inwardShift = [&](QPointF p0)->QPointF
    {
        /* 4.1 ищем ПРОЕКЦИЮ P′ на ближайший отрезок ---------------- */
        double best=1e99; double projX=0, projY=0;
        int bestIdx=-1, n=glacier_x.size();
        for(int i=0;i<n;++i)
        {
            int k=(i+1)%n;
            double x1=glacier_x[i], y1=glacier_y[i];
            double x2=glacier_x[k], y2=glacier_y[k];
            /* проектируем p0 на отрезок i-k */
            double vx=x2-x1, vy=y2-y1;
            double t=((p0.x()-x1)*vx+(p0.y()-y1)*vy)/(vx*vx+vy*vy);
            t=std::clamp(t,0.0,1.0);
            double qx=x1+vx*t, qy=y1+vy*t;
            double d=std::hypot(qx-p0.x(),qy-p0.y());
            if(d<best){best=d; projX=qx; projY=qy; bestIdx=i;}
        }

        /* 4.2 нормаль к рёбру bestIdx ------------------------------ */
        int j=bestIdx, k=(bestIdx+1)%glacier_x.size();
        double ex=glacier_x[k]-glacier_x[j];
        double ey=glacier_y[k]-glacier_y[j];
        double len=std::hypot(ex,ey);
        ex/=len; ey/=len;
        double nx= ey, ny=-ex;              // нормаль «влево»
        if(!inPoly(projX+nx,projY+ny)) {nx=-nx; ny=-ny;} // внутрь

        /* 4.3 двоичный поиск макс. сдвига ≤ SHIFT ------------------ */
        double lo=0, hi=SHIFT;
        for(int it=0; it<25; ++it)
        {
            double mid=(lo+hi)*0.5;
            double tx=projX+nx*mid, ty=projY+ny*mid;
            if(inPoly(tx,ty)&&minDist(tx,ty)>=SAFE_MARGIN) lo=mid;
            else hi=mid;
        }

        // /* 4.4 если даже lo=0 не удовлетворяет SAFE_MARGIN,
        //        берём половину минимального пролёта */
        // if(minDist(projX+nx*lo, projY+ny*lo) < SAFE_MARGIN)
        // {
        //     double d=minDist(projX,projY);
        //     lo=std::max(0.0, d*0.5);              // половина ширины
        // }

        // return {projX+nx*lo, projY+ny*lo};

        /* 4.4 итоговое смещение lo даёт точку Q = P′ + n·lo            */
        double qx = projX + nx*lo;
        double qy = projY + ny*lo;
        double cur = minDist(qx,qy);

        /* 4.5 если расстояние до стены < SAFE_MARGIN,
           доталкиваемся ещё чуть-чуть внутрь                       */
        if (cur < SAFE_MARGIN - 1e-6)           // небольшой EPS
        {
            double delta = SAFE_MARGIN - cur + 1.0;   // «+1 м» запас
            lo = std::min(lo + delta, SHIFT);         // не выходим за SHIFT
            qx = projX + nx*lo;
            qy = projY + ny*lo;
        }

        return {qx,qy};
    };

    /* ---------- 5. итоговые точки ------------------------------- */
    QPointF spawn  = inwardShift(!trackStart.isNull()
                                    ? mapPt(trackStart)
                                    : QPointF(glacier_x.front(),glacier_y.front()));

    QPointF finish = inwardShift(!trackEnd.isNull()
                                     ? mapPt(trackEnd)
                                     : QPointF(glacier_x.back(),glacier_y.back()));

    start_point  ={spawn.x(),  spawn.y()};
    final_point  ={finish.x(), finish.y()};

    /* ---------- 6. курс на цель --------------------------------- */
    double dx=final_point.first-start_point.first;
    double dy=final_point.second-start_point.second;
    spawn_heading=std::atan2(dx,dy)+M_PI;
    if(spawn_heading> M_PI) spawn_heading-=2*M_PI;
    if(spawn_heading<=-M_PI) spawn_heading+=2*M_PI;

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
