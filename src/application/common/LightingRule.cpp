#include <Arduino.h>
#include <application/common/LightingRule.h>
#include <framework/utils/Time.h>
#include <framework/utils/Math.h>
#include <ezTime.h>

using namespace std;

void LightingRule::SetRule(TimeOfDay beginTransitionStart, TimeOfDay beginTransitionStop, TimeOfDay endTransitionStart, TimeOfDay endTransitionStop)
{
  BeginTransitionStart = TimeOfDayToSeconds(beginTransitionStart);
  BeginTransitionStop = TimeOfDayToSeconds(beginTransitionStop);
  EndTransitionStart = TimeOfDayToSeconds(endTransitionStart);
  EndTransitionStop = TimeOfDayToSeconds(endTransitionStop);
}

double LightingRule::GetLightRatio(time_t time, Timezone *timezone)
{
  double secondsOfDay = Time::GetSecondsOfDay<double>(time, timezone);
  double ratio = 0.0;

  if (secondsOfDay >= BeginTransitionStop && secondsOfDay <= EndTransitionStart)
  {
    ratio = 1.0;
  }

  else if (secondsOfDay > BeginTransitionStart && secondsOfDay < BeginTransitionStop)
  {
    ratio = Math::Map<double>(secondsOfDay, BeginTransitionStart, BeginTransitionStop, 0.0, 1.0);
  }

  else if (secondsOfDay > EndTransitionStart && secondsOfDay < EndTransitionStop)
  {
    ratio = Math::Map<double>(secondsOfDay, EndTransitionStart, EndTransitionStop, 0.0, 1.0);
  }

  return Math::Clamp<double>(ratio, 0.0, 1.0);
}

uint32_t LightingRule::TimeOfDayToSeconds(TimeOfDay timeOfDay)
{
  return (timeOfDay.Hours * 3600) + (timeOfDay.Minutes * 60) + (timeOfDay.Seconds * 1);
}