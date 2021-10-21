#include "func.h"

double round_to(double inpValue, int inpCount)
{
    double result = std::floor(inpValue * pow(10,inpCount) + 0.5) / pow(10,inpCount);
    return result;
}

double vector_module(const double &x, const double &y)
{
 return pow(pow(x,2)+pow(y,2),0.5);
}

double skalar_multipl(const double &x1, const double &y1, const double &x2, const double &y2)
{
    return x1*x2 + y1*y2;
}
