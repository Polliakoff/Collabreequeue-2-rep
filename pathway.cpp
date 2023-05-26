#include "pathway.h"

pathway::pathway()
{
    make_my_way();
}

pathway::~pathway(){}

void pathway::make_my_way()
{
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
    generator_switch = tmblr_generator;
    glacier_x.clear();
    glacier_y.clear();
    l_count = 0;
    r_count = 0;
    main_count = 0;
    vertexes.clear();
    faces.clear();
    make_my_way();
}
