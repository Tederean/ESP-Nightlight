#include <Arduino.h>
#include <ezTime.h>
#include <application/services/SchedulerService.h>
#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace Scheduler
  {

    const long TenMinutes = 600L;

    const long HalfAnHour = 1800L;

    const long OneDay = 3600L * 24L;

    Event<void> WifiShutdownEvent;

    Timezone Localtime;

    void OnSystemDailyRebootTimer();

    void SetupDailyRestart()
    {
      time_t now = Localtime.now();
      tmElements_t timeBuilder;

      breakTime(now, timeBuilder);
      timeBuilder.Hour = 12;
      timeBuilder.Minute = 0;
      timeBuilder.Second = 0;
      time_t midday = makeTime(timeBuilder);

      time_t restartTime = midday + random(-TenMinutes, TenMinutes);

      if ((restartTime - now) < HalfAnHour)
      {
        restartTime += OneDay;
      }

#ifdef SERIAL_DEBUG
      Serial.print("ESP restart time: ");
      Serial.println(Localtime.dateTime(restartTime, LOCAL_TIME));
#endif

      Localtime.setEvent(OnSystemDailyRebootTimer, restartTime, LOCAL_TIME);
    }

    void InitializeTimeZone()
    {
      Localtime.setPosix(TZ_INFO);
      Localtime.setDefault();
    }

    void OnWifiShutdownEvent(void *args)
    {
      WifiShutdownEvent.Unsubscribe(&OnWifiShutdownEvent);

#ifdef SERIAL_DEBUG
      if (Services::Time::IsTimeSynced())
      {
        Serial.println("Disconnecting from WiFi after await period has expired, everything ok!");
      }
      else
      {
        Serial.println("ESP is not able to connect to WiFi and/or NTP service.");
        Serial.println("Rebooting ESP, hoping to fix issue with it.");
        Serial.flush();
      }
#endif

      if (!Services::Time::IsTimeSynced())
        ESP.restart();

      Services::Ota::DisableOta();
      Services::Wifi::DisableWifi();
    }

    void OnTimeSyncedEvent(void *args)
    {
      Services::Time::TimeSyncedEvent.Unsubscribe(&OnTimeSyncedEvent);

      InitializeTimeZone();
      SetupDailyRestart();

      // TODO: Enable light
    }

    void OnSystemDailyRebootTimer()
    {
#ifdef SERIAL_DEBUG
      Serial.println("Daily restart event triggered!");
      Serial.flush();
#endif

      ESP.restart();
    }

    void Initialize()
    {
      int64_t shutdownDelay_us = WIFI_SHUTDOWN_TIME * 1000LL * 1000LL;

      WifiShutdownEvent.Subscribe(&OnWifiShutdownEvent);
      Services::System::InvokeOnce(&WifiShutdownEvent, shutdownDelay_us);

      Services::Time::TimeSyncedEvent.Subscribe(&OnTimeSyncedEvent);

      Services::Wifi::EnableWifi(WIFI_SSID, WIFI_PSK, WIFI_NAME);
      Services::Ota::EnableOta(WIFI_NAME, WIFI_NAME);
    }

  } // namespace Scheduler
} // namespace Services