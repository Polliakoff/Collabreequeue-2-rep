#include "func.h"

double roundTo(double inpValue, int inpCount)
{
    double outpValue;
    double tempVal;
    tempVal=inpValue*pow(10,inpCount);
    if (double(int(tempVal))+0.5==tempVal)
    {
        if (int(tempVal)%2==0)
            {outpValue=double(floor(tempVal))/pow(10,inpCount);}
        else
            {outpValue=double(ceil(tempVal))/pow(10,inpCount);}
    }
    else
    {
        if (double(int(tempVal))+0.5>tempVal)
            {outpValue=double(ceil(tempVal))/pow(10,inpCount);}
        else
            {outpValue=double(floor(tempVal))/pow(10,inpCount);}
    }
    return(outpValue);
}
