#ifndef _SolarMath_
#define _SolarMath_

#include <Arduino.h>
#include <ezTime.h>

using namespace std;

namespace SolarMath
{

  double GetSolarElevationAngle(time_t time, Timezone *timezone, const ezLocalOrUTC_t local_or_utc, double latitude, double longitude);

  double SolarAngleToLightRatio(double value);
  
} // namespace SolarMath

#endif
