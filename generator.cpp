#include "generator.h"




void generate(QVector<int> &xl,
              QVector<int> &xr,
              QVector<int> &yl,
              QVector<int> &yr,
              QVector<int> &xo,
              QVector<int> &yo){

    int i, j, k = 1, a, b, c, d;

    // Стартовые координаты
    xl[0] = 250;
    yl[0] = 600;
    xr[0] = 350;
    yr[0] = 600;
    xl[1] = 240;
    yl[1] = 590;
    xr[1] = 360;
    yr[1] = 590;

    // Основной генератор неровностей
    for (i = 2; i < 99; i++)
    {
        // Основная неровность по Х и сдвиг по У
        a = QRandomGenerator::global()->bounded(30);
        b = QRandomGenerator::global()->bounded(30);
        xl[i] = xl[i-1] + a - b;
        yl[i] = yl[i-1] - 8;
        a = QRandomGenerator::global()->bounded(30);
        b = QRandomGenerator::global()->bounded(30);
        xr[i] = xr[i-1] + a - b;
        yr[i] = yr[i-1] - 8;

        // Заворот в сторону со смещением У
        a = QRandomGenerator::global()->bounded(5);
        if (a == 1)
        {
            a = QRandomGenerator::global()->bounded(8);
            b = QRandomGenerator::global()->bounded(10);
            yr[i] -= 8 + a;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xr[i] += 30 - a + b;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xl[i] += 20 - a + b;


            a = QRandomGenerator::global()->bounded(8);
            b = QRandomGenerator::global()->bounded(10);
            yr[i+1] = yr[i] - 8 - a;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xr[i+1] = xr[i] + 30 - a + b;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xl[i+1] = xl[i] + 20 - a + b;
            i++;
        } else if (a == 2)
        {
            a = QRandomGenerator::global()->bounded(8);
            b = QRandomGenerator::global()->bounded(10);
            yl[i] -= 8 + a;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xl[i] -= 30 - a + b;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xr[i] -= 20 - a + b;

            a = QRandomGenerator::global()->bounded(8);
            b = QRandomGenerator::global()->bounded(10);
            yl[i+1] = yl[i] - 8 - a;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xl[i+1] = xl[i] - 30 - a + b;
            a = QRandomGenerator::global()->bounded(10);
            b = QRandomGenerator::global()->bounded(10);
            xr[i+1] = xr[i] - 20 - a + b;
            i++;
        }else if (a == 3)
        {
            yl[i] -= 6;
        }else if (a == 4)
        {
            yr[i] -= 6;
        }

        // Контроль минимальной ширины прохода, мягкий
        if (xr[i] - xl[i] < 90)
        {
            xr[i] += 25;
        }

        // Контроль максимальной ширины прохода, мягкий
        if (xr[i] - xl[i] > 150)
        {
            xr[i] -= 25;
        }


        // Контроль минимальной ширины прохода в почти предельных случаях, жестский
        if (xr[i] - xl[i] < 10)
        {
            xr[i] += 35;
        }

        if (xr[i] - xl[i-1] < 10)
        {
            xr[i] += 45;
        }

        if (xr[i] - xl[i+1] < 10)
        {
            xr[i] += 45;
        }


        // Возвращение пути к примерно середине к концу маршрута
        if (yl[i] < 200 || yr[i] < 200)
        {
            if (xr[i] > 400)
            {
                xr[i] -= 25;
                xl[i] -= 25;
            }
            if (xl[i] < 200)
            {
                xr[i] += 25;
                xl[i] += 25;
            }
        }

        // Ограничение выхода за карту 600х600, предельный случай
        if (xr[i] > 600)
        {
            a = QRandomGenerator::global()->bounded(10);
            xr[i] = 600 - a;
            xl[i] -= a;
        }
        if (xl[i] < 0)
        {
            a = QRandomGenerator::global()->bounded(10);
            xl[i] = a;
            xr[i] += a;
        }

    }

    // Генератор широких разломов в стороны в случайных местах
    for (i = 10; i < 100; i++)
    {
        d = QRandomGenerator::global()->bounded(10);
        if(i > 10 && d == 1)
        {
            d = QRandomGenerator::global()->bounded(100);
            c = QRandomGenerator::global()->bounded(10);
            for(j = 0; j < c; j++)
            {
                a = QRandomGenerator::global()->bounded(45);
                b = QRandomGenerator::global()->bounded(45);
                if(d >= 50)
                {
                    if(j == 0)
                    {
                        xr[i - j] += 40 + a;
                    } else if(j + 1 == c)
                    {
                        xr[i - j] += 40 + a;
                    }else
                    {
                        xr[i - j] += 100 + a;
                    }

                }else
                {
                    if(j == 0)
                    {
                        xl[i - j] -= 40 + a;
                    } else if(j + 1 == c)
                    {
                        xl[i - j] -= 40 + a;
                    } else
                    {
                        xl[i - j] -= 100 + a;
                    }

                }

            }
            a = QRandomGenerator::global()->bounded(25);
            b = QRandomGenerator::global()->bounded(25);


            // Изменение У в разломах для создания объема
            if(d >= 50 && xr[i - 1] > xr[i])
            {
                yr[i - 1] -= a;
            }
            if(d >= 50 && xr[i - c + 1] > xr[i - c])
            {
                yr[i - c] += b;
            }
            if(d < 50 && xl[i - 1] < xl[i])
            {
                yl[i - 1] -= a;
            }
            if(d < 50 && xl[i - c + 1] < xl[i - c])
            {
                yl[i - c] += b;
            }
            i += c;
        }

    }


    // Изменение У при слишком большом вертикальном расхождении
    for (i = 1; i < 100; i++)
    {
        if(yr[i] - yl[i] > 25)
        {
            yr[i] -= 10;
        }

        if(yl[i] - yr[i] > 25)
        {
            yl[i] -= 10;
        }
    }

    // Aнти"узелковая" система.
    for (i = 1; i < 100; i++)
    {
        if (yr[i] >= yr[i-1])
        {
            a = QRandomGenerator::global()->bounded(5);
            yr[i] = yr[i-1] - a;
        }
        if (yl[i] >= yl[i-1])
        {
            a = QRandomGenerator::global()->bounded(5);
            yl[i] = yl[i-1] - a;
        }
    }

    // Генератор узких разломов
    for (i = 1; i < 90; i++)
    {
        d = QRandomGenerator::global()->bounded(40);
        if(i > 10 && d == 1)
        {
            d = QRandomGenerator::global()->bounded(2);
            if(d == 0)
            {
                a = QRandomGenerator::global()->bounded(40);
                xr[i] = (xr[i] + xr[i + 2]) / 2 + 20 + a;
                a = QRandomGenerator::global()->bounded(40);
                xr[i + 2] = xr[i] - 20 + a;
                a = QRandomGenerator::global()->bounded(40);
                xr[i + 1] = (xr[i] + xr[i + 2]) / 2 + 60 + a;
                a = QRandomGenerator::global()->bounded(40);
                yr[i + 1] += a - 20;
                a = QRandomGenerator::global()->bounded(10);
                yr[i + 2] = yr[i] - 10 - a;
            } else
            {
                a = QRandomGenerator::global()->bounded(40);
                xl[i] = (xl[i] + xl[i + 2]) / 2 - 20 - a;
                a = QRandomGenerator::global()->bounded(40);
                xl[i + 2] = xl[i] + 20 - a;
                a = QRandomGenerator::global()->bounded(40);
                xl[i + 1] = (xl[i] + xl[i + 2]) / 2 - 60 - a;
                a = QRandomGenerator::global()->bounded(40);
                yl[i + 1] += a - 20;
                a = QRandomGenerator::global()->bounded(10);
                yl[i + 2] = yl[i] - 10 - a;
            }

        }
        // Ограничение выхода за карту 600х600, предельный случай
        if (xr[i] > 600)
        {
            a = QRandomGenerator::global()->bounded(10);
            xr[i] = 600 - a;
            xl[i] -= a;
        }
        if (xl[i] < 0)
        {
            a = QRandomGenerator::global()->bounded(10);
            xl[i] = a;
            xr[i] += a;
        }
    }

    // Присваивание значений -1 массивам координат островков для дальнейшей обработки
    for (i = 0; i < 100; i++)
    {
        xo[i] = -1;
        yo[i] = -1;
    }

    // Генератор островков в свободном пространстве
    // Их координаты записываются в массивы хо и уо с разделителем -1.
    // Отрисовывать, соответственно, либо с начала до разделителя, либо что между разделителями.
    for (i = 10; i < 80; i++)
    {
        if(xr[i] - xl[i] > 120 && xr[i-1] - xl[i-1] > 120 && xr[i-2] - xl[i-2] > 120  && xr[i+1] - xl[i+1] > 120 && xr[i+2] - xl[i+2] > 120 && xr[i+3] - xl[i+3] > 120 && xr[i+4] - xl[i+4] > 120 && xr[i+1] - xl[i] > 120 && xr[i] - xl[i+1] > 120 && xr[i+2] - xl[i] > 120 && xr[i] - xl[i+2] > 120 && xr[i+5] - xl[i+5] > 120 && xr[i+6] - xl[i+6] > 120)
        {
            c = QRandomGenerator::global()->bounded(10) + 5;
            a = QRandomGenerator::global()->bounded(15);
            b = QRandomGenerator::global()->bounded(15);
            // Выбор места начала островка, примерно посередине полости
            xo[k] = (xr[i + 1] + xl[i + 1] + xr[i + 4] + xl[i + 4] + a - b) / 4;
            yo[k] = (yr[i + 1] + yl[i + 1] + yr[i + 4] + yl[i + 4] + a) / 4;
            //xo[k] = (xr[i + 1] + xl[i + 1] + a - b) / 2;
            //yo[k] = (yr[i + 1] + yl[i + 1] + a) / 2;
            k++;
            // Генерация островка
            for (j = k; j < c + k; j++)
            {
                if(j - k < c / 4)
                {
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    b = QRandomGenerator::global()->bounded(5);
                    xo[j] = xo[j - 1] + a - b;
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    yo[j] = yo[j - 1] - a;
                } else if(j - k < c / 2)
                {
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    b = QRandomGenerator::global()->bounded(5);
                    xo[j] = xo[j - 1] - a + b;
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    yo[j] = yo[j - 1] - a;
                } else if(j - k < 3 * c / 4)
                {
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    b = QRandomGenerator::global()->bounded(5);
                    xo[j] = xo[j - 1] - a + b;
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    yo[j] = yo[j - 1] + a;
                } else
                {
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    b = QRandomGenerator::global()->bounded(10);
                    xo[j] = xo[j - 1] + a - b;
                    a = QRandomGenerator::global()->bounded(10) + 5;
                    yo[j] = yo[j - 1] + a;
                }
            }

            // Островок формой напоминает ромб, следующий цикл его немного разнообразит
            for (j = k; j < c + k; j++)
            {
                if(j - k < c / 4)
                {
                    a = QRandomGenerator::global()->bounded(10);
                    xo[j] += a;
                    a = QRandomGenerator::global()->bounded(5);
                    yo[j] += a;
                } else if(j - k < c / 2)
                {
                    a = QRandomGenerator::global()->bounded(10);
                    xo[j] += a;
                    a = QRandomGenerator::global()->bounded(5);
                    yo[j] -= a;
                } else if(j - k < 3 * c / 4)
                {
                    a = QRandomGenerator::global()->bounded(10);
                    xo[j] -= a;
                    a = QRandomGenerator::global()->bounded(5);
                    yo[j] -= a;
                } else
                {
                    a = QRandomGenerator::global()->bounded(10);
                    xo[j] -= a;
                    a = QRandomGenerator::global()->bounded(5);
                    yo[j] += a;
                }
            }

            // Замыкание островка
            yo[j] = yo[k - 1];
            xo[j] = xo[k - 1];

            // Смещение островка для избегания пересечений


            i += c;
            k += c + 2;

        }
    }


}

// Генератор контрольных точек
// Их координаты записываются в массивы хp и уp, с разделителем -1.

void ctpts(const QVector<int> &xl,
           const QVector<int> &xr,
           const QVector<int> &yl,
           const QVector<int> &yr,
           QVector<int> &xp,
           QVector<int> &yp)
{
    int i, j, k = 1, temp, temp2 = 1000, locmax, mindis;

    // Заполняем массивы -1, чтобы потом легко понять, где кончаются данные и начинается пустое пространство
    for(i = 0; i < 100; i++)
    {
        xp[i] = -1;
        yp[i] = -1;
    }

    for(i = 5; i < 95; i++)
    {
        // Ищем локальные максимумы по Х левого берега в пределах пяти точек
        temp = 0;
        for(j = 0; j < 5; j++)
        {
            if(k - 3 >= 0)
            {
                if(xl[i + j] > temp && xl[i + j] != xp[k - 3])
                {
                    temp = xl[i + j];
                    locmax = i + j;
                }
            } else if(xl[i + j] > temp)
            {
                temp = xl[i + j];
                locmax = i + j;
            }
        }
        xp[k] = xl[locmax];
        yp[k] = yl[locmax];

        // Ищем минимальное расстояние до выбранной точки левого берега на правом берегу в пределах десяти точек
        temp = 999999;
        for(j = 0; j < 10; j++)
        {
            if(sqrt((xl[locmax] - xr[locmax - 5 + j]) * (xl[locmax] - xr[locmax - 5 + j]) + (yl[locmax] - yr[locmax - 5 + j]) * (yl[locmax] - yr[locmax - 5 + j])) < temp && yr[locmax - 5 + j] < temp2)
            {
                temp = sqrt((xl[locmax] - xr[locmax - 5 + j]) * (xl[locmax] - xr[locmax - 5 + j]) + (yl[locmax] - yr[locmax - 5 + j]) * (yl[locmax] - yr[locmax - 5 + j]));
                mindis = locmax - 5 + j;
            }
        }
        xp[k + 1] = xr[mindis];
        yp[k + 1] = yr[mindis];
        temp2 = yp[k + 1];

        k += 3;
        i += 3;
    }



}
