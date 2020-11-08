#ifndef _LightingRule_
#define _LightingRule_

#include <Arduino.h>
#include <ezTime.h>

using namespace std;

enum class LightingRuleType
{
  AllowLight,
  DenyLight
};

typedef struct
{
  uint8_t Hours;
  uint8_t Minutes;
  uint8_t Seconds;
} TimeOfDay;

class LightingRule
{

private:
  LightingRuleType Type;

  uint32_t BeginTransitionStart;

  uint32_t BeginTransitionStop;

  uint32_t EndTransitionStart;

  uint32_t EndTransitionStop;

  uint32_t TimeOfDayToSeconds(TimeOfDay timeOfDay);

public:
  LightingRule(LightingRuleType type, TimeOfDay beginTransitionStart, TimeOfDay beginTransitionStop, TimeOfDay endTransitionStart, TimeOfDay endTransitionStop);

  double GetLightRatio(Timezone *timezone, time_t time, const ezLocalOrUTC_t local_or_utc);
};

#endif