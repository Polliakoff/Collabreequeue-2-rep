#include "pattern.h"

pattern::pattern()
{

    swing = (-100 + rand()%201)/100.0;
    speed = (-100 + rand()%201)/100.0;
    h_factor = (0 + rand()%101)/100.0;
}

pattern::pattern(pattern &a, pattern &b)
{
    swing = (a.swing+b.swing)/2.0;
    speed = (a.speed+b.speed)/2.0;
    h_factor = (a.h_factor+b.h_factor)/2.0;
}


