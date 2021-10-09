#include "func.h"

double round_to(double inpValue, int inpCount)
{
//    double outpValue;
//    double tempVal;
//    tempVal=inpValue*pow(10,inpCount);
//    if (double(int(tempVal))+0.5==tempVal)
//    {
//        if (int(tempVal)%2==0)
//            {outpValue=double(floor(tempVal))/pow(10,inpCount);}
//        else
//            {outpValue=double(ceil(tempVal))/pow(10,inpCount);}
//    }
//    else
//    {
//        if (double(int(tempVal))+0.5>tempVal)
//            {outpValue=double(ceil(tempVal))/pow(10,inpCount);}
//        else
//            {outpValue=double(floor(tempVal))/pow(10,inpCount);}
//    }
//    return(outpValue);


    double result = std::floor(inpValue * pow(10,inpCount) + 0.5) / pow(10,inpCount);
   // double result = double(int(inpValue * pow(10,inpCount)))/pow(10,inpCount);
    return result;
}
