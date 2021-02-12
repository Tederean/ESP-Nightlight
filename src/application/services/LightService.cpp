#include <Arduino.h>
#include <application/services/LightService.h>
#include <application/common/SolarMath.h>
#include <application/common/LightingRule.h>
#include <framework/peripherals/PWMChannel.h>
#include <framework/services/OtaService.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>
#include <framework/utils/Math.h>

using namespace std;

namespace Services
{
  namespace Light
  {

    void Initialize();

    void OnTimeSyncedEvent(void *args);

    void OnLoopEvent(void *args);

#if (PWM_RESOLUTION == 1)
    PWMChannel Channel(PWM_PIN, PWM_INVERTED);
#elif defined(ESP32)
    PWMChannel Channel(PWM_PIN, PWM_INVERTED, PWM_FREQUENCY, PWM_RESOLUTION, PWM_CHANNEL, 0.0, PWM_RATIO);
#elif defined(ESP8266)
    PWMChannel Channel(PWM_PIN, PWM_INVERTED, PWM_FREQUENCY, PWM_RESOLUTION, 0.0, PWM_RATIO);
#endif

    LightingRule RuleForLighting;

    void Initialize()
    {
      Channel.Initialize();

      TimeOfDay morningTransitionStart = {MORNING_TRANSITION_START};
      TimeOfDay morningTransitionStop = {MORNING_TRANSITION_STOP};

      TimeOfDay afternoonTransitionStart = {AFTERNOON_TRANSITION_START};
      TimeOfDay afternoonTransitionStop = {AFTERNOON_TRANSITION_STOP};

      RuleForLighting.SetRule(morningTransitionStart, morningTransitionStop, afternoonTransitionStart, afternoonTransitionStop);

      Services::Time::TimeSyncedEvent.Subscribe(OnTimeSyncedEvent);
    }

    void OnTimeSyncedEvent(void *args)
    {
      Services::Time::TimeSyncedEvent.Unsubscribe(OnTimeSyncedEvent);

      Services::System::LoopEvent.Subscribe(OnLoopEvent);
    }

    void OnLoopEvent(void *args)
    {
      if (Services::Ota::IsUpdating())
        return;

      auto timezone = &Services::Time::Localtime;
      auto currentTime = UTC.now();

      auto solarAngle = SolarMath::GetSolarElevationAngle(currentTime, timezone, LATITUDE, LONGITUDE);
      auto solarRatio = SolarMath::SolarAngleToLightRatio(solarAngle);

      auto maxAllowedRatio = RuleForLighting.GetLightRatio(currentTime, timezone);

      Channel.WriteRatio(Math::Min<double>(solarRatio, maxAllowedRatio));
    }

  } // namespace Light
} // namespace Services