#ifndef _Time_
#define _Time_

#include <Arduino.h>
#include <ezTime.h>

using namespace std;

namespace Time
{

  template <typename T>
  T GetSecondsOfDay(Timezone *timezone, time_t time, const ezLocalOrUTC_t local_or_utc)
  {
    auto hours = timezone->hour(time, local_or_utc);
    auto minutes = timezone->minute(time, local_or_utc);
    auto seconds = timezone->second(time, local_or_utc);

    if (is_same<T, float>::value || is_same<T, double>::value)
    {
      auto millis = timezone->ms(LAST_READ);

      return (hours * 3600.0) + (minutes * 60.0) + (seconds * 1.0) + (millis * 0.001);
    }

    return (hours * 3600) + (minutes * 60) + (seconds * 1);
  }

} // namespace Time

#endif
