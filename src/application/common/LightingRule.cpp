#include <Arduino.h>
#include <application/common/LightingRule.h>
#include <framework/utils/Time.h>
#include <framework/utils/Math.h>
#include <ezTime.h>

using namespace std;

LightingRule::LightingRule(LightingRuleType type, TimeOfDay beginTransitionStart, TimeOfDay beginTransitionStop, TimeOfDay endTransitionStart, TimeOfDay endTransitionStop)
{
  Type = type;

  BeginTransitionStart = TimeOfDayToSeconds(beginTransitionStart);
  BeginTransitionStop = TimeOfDayToSeconds(beginTransitionStop);
  EndTransitionStart = TimeOfDayToSeconds(endTransitionStart);
  EndTransitionStop = TimeOfDayToSeconds(endTransitionStop);
}

double LightingRule::GetLightRatio(Timezone *timezone, time_t time, const ezLocalOrUTC_t local_or_utc)
{
  double secondsOfDay = Time::GetSecondsOfDay<double>(timezone, time, local_or_utc);
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

  if (Type == LightingRuleType::DenyLight)
  {
    ratio = abs(ratio - 1.0); // Invert
  }

  return Math::Clamp<double>(ratio, 0.0, 1.0);
}

uint32_t LightingRule::TimeOfDayToSeconds(TimeOfDay timeOfDay)
{
  return (timeOfDay.Hours * 3600) + (timeOfDay.Minutes * 60) + (timeOfDay.Seconds * 1);
}