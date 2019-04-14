
#ifndef _LightUtils_
#define _LightUtils_

#include<Arduino.h>


namespace LightHandler
{
    uint16_t roundFloatingToInteger(double value);

    uint16_t map(double in_value, double in_min, double in_max, double out_min, double out_max);
}


#endif
