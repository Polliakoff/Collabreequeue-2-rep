#include "polygon.h"

polygon::polygon(){}

polygon::~polygon(){}


void add_point(double x, double y)
{
    point.emplace_back(std::make_pair(x, y));
    //point1.push_back( make_pair(arr[i],arr1[i]) );
}

bool search_for_point(double x, double y)
{

}
