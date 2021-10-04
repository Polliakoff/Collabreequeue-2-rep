#include "polygon.h"

polygon::polygon(){}

polygon::~polygon(){}


void polygon::add_point(double x, double y)
{
    vertexes.emplace_back(std::make_pair(x, y));
    size_t length = vertexes.size();
    if(length>=2){
        faces.emplace_back(straight_line(vertexes[length-2].first,vertexes[length-2].second,
                vertexes[length-1].first,vertexes[length-1].second));
    }
}

bool polygon::search_for_point(double x, double y)
{
     auto found{ std::find(vertexes.begin(), vertexes.end(), std::make_pair(x, y))};

     if (found == vertexes.end())
     {
       return false;
     }
     else
     {
       return true;
     }
}
