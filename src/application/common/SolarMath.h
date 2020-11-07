#ifndef _SolarMath_
#define _SolarMath_

#include <Arduino.h>
#include <ezTime.h>

using namespace std;

namespace SolarMath
{

  double GetSolarElevation_degree(time_t time, Timezone *timezone, double latitude);

} // namespace SunMath

#endif
