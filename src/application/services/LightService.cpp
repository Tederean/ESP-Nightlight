#include <Arduino.h>
#include <application/services/LightService.h>
#include <application/common/SolarMath.h>
#include <application/common/LightingRule.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace Light
  {

    void Initialize();

    void OnTimeSyncedEvent(void *args);

    void OnCalculateSunAngleEvent(void *args);

    const int64_t OneSecond = 1000LL * 1000LL;

    LightingRule RuleForLighting;

    Event<void> CalculateSunAngleEvent;

    void Initialize()
    {
      Services::Time::TimeSyncedEvent.Subscribe(OnTimeSyncedEvent);

      TimeOfDay morningTransitionStart = {MORNING_TRANSITION_START};
      TimeOfDay morningTransitionStop = {MORNING_TRANSITION_STOP};

      TimeOfDay afternoonTransitionStart = {AFTERNOON_TRANSITION_START};
      TimeOfDay afternoonTransitionStop = {AFTERNOON_TRANSITION_STOP};

      RuleForLighting.SetRule(morningTransitionStart, morningTransitionStop, afternoonTransitionStart, afternoonTransitionStop);
    }

    void OnTimeSyncedEvent(void *args)
    {
      Services::Time::TimeSyncedEvent.Unsubscribe(OnTimeSyncedEvent);

      CalculateSunAngleEvent.Subscribe(OnCalculateSunAngleEvent);
      Services::System::InvokeRepeating(&CalculateSunAngleEvent, OneSecond, OneSecond);
    }

    void OnCalculateSunAngleEvent(void *args)
    {
      auto timezone = &Services::Time::Localtime;
      auto currentTime = UTC.now();

      auto solarAngle = SolarMath::GetSolarElevationAngle(currentTime, timezone, LATITUDE, LONGITUDE);
      //auto solarRatio = SolarMath::SolarAngleToLightRatio(solarAngle);

      Serial.print(solarAngle, 3);
      Serial.print('@');

      Serial.print(timezone->hour(currentTime, UTC_TIME));
      Serial.print(':');
      Serial.print(timezone->minute(currentTime, UTC_TIME));
      Serial.println('\n');
    }

  } // namespace Light
} // namespace Services