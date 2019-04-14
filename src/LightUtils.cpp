#include <LightUtils.h>
#include <Arduino.h>

namespace LightHandler
{
    uint16_t roundFloatingToInteger(double value)
    {
        return value >= 0.0 ? (uint16_t) ((value)+0.5) : (uint16_t) ((value)-0.5); // Round and cast to integer
    }

    uint16_t map(double in_value, double in_min, double in_max, double out_min, double out_max)
    {
        return roundFloatingToInteger((in_value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
    }
}