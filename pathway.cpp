#include "pathway.h"

pathway::pathway()
{
    make_my_way();
}

pathway::~pathway(){}

void pathway::make_my_way()
{
    final_point = std::make_pair(0,125);
    start_point = std::make_pair(575,550);
    add_point(-110,0);
    add_point(700,0);
    add_point(700,700);
    add_point(450,700);
    add_point(450,250);
    add_point(-110,250);
}

void pathway::generator()
{
    //Здесь ничего нет. Пока.
}
