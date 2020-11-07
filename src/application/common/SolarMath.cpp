#include <Arduino.h>
#include <application/common/SolarMath.h>
#include <framework/services/TimeService.h>
#include <ezTime.h>

using namespace std;

namespace SolarMath
{

  double GetDaytime_h(time_t time, Timezone *timezone)
  {
    auto hours = timezone->hour(time, UTC_TIME);
    auto minutes = timezone->minute(time, UTC_TIME);
    auto seconds = timezone->second(time, UTC_TIME);
    auto millis = timezone->ms(LAST_READ);

    return hours + (minutes / 60.0) + (seconds / 3600.0) + (millis / 3600000.0);
  }

  double GetSolarElevation_degree(time_t time, Timezone *timezone, double latitude)
  {
    if (timezone->isDST(time, UTC_TIME))
    {
      time -= timezone->getOffset(time, UTC_TIME) * 60L; // We require natural time.
    }

    auto declin = -23.45 * cos(DEG_TO_RAD * 360.0 * (timezone->dayOfYear(time, UTC_TIME) + 10.0) / 365.0);

    auto time_equation = 60.0 * (-0.171 * sin(0.0337 * timezone->dayOfYear(time, UTC_TIME) + 0.465) - 0.1299 * sin(0.01787 * timezone->dayOfYear(time, UTC_TIME) - 0.168));

    auto hour_angle = 15.0 * (GetDaytime_h(time, timezone) - (15.0 - LONGITUDE) / 15.0 - 12.0 + time_equation / 60.0);

    auto sun_angle = sin(DEG_TO_RAD * latitude) * sin(DEG_TO_RAD * declin) + cos(DEG_TO_RAD * latitude) * cos(DEG_TO_RAD * declin) * cos(DEG_TO_RAD * hour_angle);

    return asin(sun_angle) / DEG_TO_RAD;
  }

} // namespace SunMath