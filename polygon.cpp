#include "polygon.h"

polygon::polygon(){}

polygon::~polygon(){}


void polygon::add_point(double x, double y)
{
    point.emplace_back(std::make_pair(x, y));
}

bool polygon::search_for_point(double x, double y)
{
     auto found{ std::find(point.begin(), point.end(), std::make_pair(x, y))};

     if (found == point.end())
     {
       return false;
     }
     else
     {
       return true;
     }
}
