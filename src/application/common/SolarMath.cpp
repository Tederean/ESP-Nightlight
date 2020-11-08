#include <Arduino.h>
#include <application/common/SolarMath.h>
#include <framework/utils/Time.h>
#include <ezTime.h>

using namespace std;

namespace SolarMath
{

  double GetSolarElevationAngle(time_t time, Timezone *timezone, double latitude)
  {
    if (timezone->isDST(time, UTC_TIME))
    {
      time -= timezone->getOffset(time, UTC_TIME) * 60L; // We require natural time.
    }

    auto dayOfYear = timezone->dayOfYear(time, UTC_TIME);
    auto hoursOfDay = Time::GetSecondsOfDay<double>(timezone, time, UTC_TIME) / 3600.0;

    // Here the magic beginns.

    auto declin = -23.45 * cos(DEG_TO_RAD * 360.0 * (dayOfYear + 10.0) / 365.0);

    auto timeEquation = 60.0 * (-0.171 * sin(0.0337 * dayOfYear + 0.465) - 0.1299 * sin(0.01787 * dayOfYear - 0.168));

    auto hourAngle = 15.0 * (hoursOfDay - (15.0 - LONGITUDE) / 15.0 - 12.0 + timeEquation / 60.0);

    auto sunAngle = sin(DEG_TO_RAD * latitude) * sin(DEG_TO_RAD * declin) + cos(DEG_TO_RAD * latitude) * cos(DEG_TO_RAD * declin) * cos(DEG_TO_RAD * hourAngle);

    auto sunAngleDegree = asin(sunAngle) / DEG_TO_RAD;

    // Here the magic ends.

    return sunAngleDegree;
  }

  double SolarAngleToLightRatio(double value)
  {
    if (value >= 0.0)
      return 0.0;

    if (value <= -6.0)
      return 1.0;

    return (-50.0 / 300.0) * value;
  }

} // namespace SolarMath