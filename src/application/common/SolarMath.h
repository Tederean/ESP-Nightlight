#ifndef _SolarMath_
#define _SolarMath_

#include <Arduino.h>
#include <ezTime.h>

using namespace std;

namespace SolarMath
{

  double GetSolarElevationAngle(time_t time, Timezone *timezone, double latitude);

  double SolarAngleToLightRatio(double value);
  
} // namespace SolarMath

#endif
