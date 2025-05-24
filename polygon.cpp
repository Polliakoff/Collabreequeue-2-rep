#include "polygon.h"

polygon::polygon(){}

polygon::~polygon(){}

void polygon::add_point(double x, double y)
{
    // ——————————————————————————————————————————————
    // 1) НЕ добавляем точку, если она совпадает с последней:
    if (!vertexes.empty() &&
        std::abs(x - vertexes.back().first)  < 1e-6 &&
        std::abs(y - vertexes.back().second) < 1e-6)
        return;
    // ——————————————————————————————————————————————

    vertexes.emplace_back(std::make_pair(x, y));
    size_t length = vertexes.size();
    if(length > 3){
        faces.pop_back();
    }
    if(length>=2){
        faces.emplace_back(straight_line(vertexes[length-2].first,vertexes[length-2].second,
                vertexes[length-1].first,vertexes[length-1].second));
    }
    if(length>=3){
        faces.emplace_back(straight_line(vertexes[length-1].first,vertexes[length-1].second,
                vertexes[0].first,vertexes[0].second));
    }
}
