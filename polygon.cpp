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
    if(length>=3){
        if(length > 3){
            faces.pop_back();
        }
        faces.emplace_back(straight_line(vertexes[length].first,vertexes[length].second,
                vertexes[0].first,vertexes[0].second));
    }
}
