#include "func.h"

double round_to(double inpValue, int inpCount)
{
    double result = std::floor(inpValue * pow(10,inpCount) + 0.5) / pow(10,inpCount);
    return result;
}

double vector_module(const double &x, const double &y)
{
 return sqrt(pow(x,2)+pow(y,2));
}

double skalar_multipl(const double &x1, const double &y1, const double &x2, const double &y2)
{
    return x1*x2 + y1*y2;
}

double vectors_projection(const double &x1, const double &y1, const double &x2, const double &y2)
{
    return skalar_multipl(x1,x2,y1,y2)/vector_module(x1,x2);
}

double vectors_angle(const double &x1, const double &y1, const double &x2, const double &y2)
{
    return acos(skalar_multipl(x1,y1,x2,y2)/(vector_module(x1,y1)*vector_module(x2,y2)));
}

double vector_length(const double &x1, const double &y1, const double &x2, const double &y2)
{
    return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}
