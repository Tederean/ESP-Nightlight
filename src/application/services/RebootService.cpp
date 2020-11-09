#include <Arduino.h>
#include <ezTime.h>
#include <application/services/RebootService.h>
#include <framework/services/SystemService.h>
#include <framework/services/TimeService.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace Reboot
  {

    void Initialize();

    void OnNetworkTimeoutRebootEvent(void *args);

    void OnTimeSyncedEvent(void *args);

    void SetupDailyMiddayReboot();

    void OnDailyMiddayRebootEvent(void *args);

    time_t GetDailyMiddayRebootTime();

    const int64_t NetworkTimeoutRebootTime = (5LL + WIFI_DISABLE_TIME) * 1000LL * 1000LL;

    Event<void> NetworkTimeoutRebootEvent;

    Event<void> DailyMiddayRebootEvent;

    void Initialize()
    {
      NetworkTimeoutRebootEvent.Subscribe(OnNetworkTimeoutRebootEvent);
      Services::System::InvokeOnce(&NetworkTimeoutRebootEvent, NetworkTimeoutRebootTime);

      Services::Time::TimeSyncedEvent.Subscribe(OnTimeSyncedEvent);
    }

    void OnNetworkTimeoutRebootEvent(void *args)
    {
      NetworkTimeoutRebootEvent.Unsubscribe(OnNetworkTimeoutRebootEvent);

      Debug("ESP is not able to connect to WiFi and/or NTP service.\nRebooting ESP, hoping to fix issue with it.\n");
      DebugFlush();

      ESP.restart();
    }

    void OnTimeSyncedEvent(void *args)
    {
      Services::Time::TimeSyncedEvent.Unsubscribe(OnTimeSyncedEvent);

      NetworkTimeoutRebootEvent.Unsubscribe(OnNetworkTimeoutRebootEvent);
      Services::System::InvokeCancel(&NetworkTimeoutRebootEvent);

      SetupDailyMiddayReboot();
    }

    void SetupDailyMiddayReboot()
    {
      time_t rebootTime = GetDailyMiddayRebootTime();
      Timezone *localtime = &Services::Time::Localtime;

      Debug("ESP restart time: " + localtime->dateTime(rebootTime, LOCAL_TIME) + "\n");

      DailyMiddayRebootEvent.Subscribe(OnDailyMiddayRebootEvent);

      localtime->setEvent([] { DailyMiddayRebootEvent.Invoke(nullptr); }, rebootTime, LOCAL_TIME);
    }

    void OnDailyMiddayRebootEvent(void *args)
    {
      DailyMiddayRebootEvent.Unsubscribe(OnDailyMiddayRebootEvent);

      Debug("Daily midday reboot!.\n");
      DebugFlush();

      ESP.restart();
    }

    time_t GetDailyMiddayRebootTime()
    {
      const time_t TenMinutes = 600;
      const time_t HalfAnHour = 1800;
      const time_t OneDay = 3600 * 24;

      time_t now = Services::Time::Localtime.now();
      tmElements_t timeBuilder;

      breakTime(now, timeBuilder);
      timeBuilder.Hour = 12;
      timeBuilder.Minute = 0;
      timeBuilder.Second = 0;
      time_t midday = makeTime(timeBuilder);

      time_t rebootTime = midday + random(-TenMinutes, TenMinutes);

      if ((rebootTime - now) < HalfAnHour)
      {
        rebootTime += OneDay;
      }

      return rebootTime;
    }
  } // namespace Reboot
} // namespace Services