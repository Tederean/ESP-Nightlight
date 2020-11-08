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

      auto solarAngle = SolarMath::GetSolarElevationAngle(currentTime, timezone, LATITUDE);
      auto solarRatio = SolarMath::SolarAngleToLightRatio(solarAngle);

      Serial.print("Angle: ");
      Serial.print(solarAngle, 4);
      Serial.print(" Ratio: ");
      Serial.println(solarRatio, 4);
    }

  } // namespace Light
} // namespace Services