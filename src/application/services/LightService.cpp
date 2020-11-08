#include <Arduino.h>
#include <application/services/LightService.h>
#include <application/common/SolarMath.h>
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

    Event<void> CalculateSunAngleEvent;

    void Initialize()
    {
      Services::Time::TimeSyncedEvent.Subscribe(OnTimeSyncedEvent);
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
      auto currentTime = timezone->now();
      auto angle = SolarMath::GetSolarElevationAngle(currentTime, timezone, LATITUDE);

      Serial.println(angle, 4);
    }





    double SolarAngleToLightRatio(double value)
    {
      if (value >= 0.0)
        return 0.0;

      if (value <= -6.0)
        return 1.0;

      return (-50.0 / 300.0) * value;
    }

  } // namespace Light
} // namespace Services