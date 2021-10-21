#include "pathway.h"

pathway::pathway()
{
    make_my_way();
}

pathway::~pathway(){}

void pathway::make_my_way()
{
    add_point(0,0);
    add_point(700,0);
    add_point(700,700);
    add_point(450,700);
    add_point(450,250);
    add_point(0,250);
}

void pathway::generator()
{
    //Здесь ничего нет. Пока.
}
