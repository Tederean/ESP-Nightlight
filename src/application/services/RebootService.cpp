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

    void SetupDailyMiddayReboot();

    void OnNetworkTimeoutRebootEvent(void *args);

    void OnTimeSyncedEvent(void *args);

    void OnDailyMiddayRebootEvent(void *args);

    time_t GetDailyMiddayRebootTime();

    const int64_t TenMinutesFiveSeconds = (5LL + 10LL * 60LL) * 1000LL * 1000LL;

    const long TenMinutes = 600L;

    const long HalfAnHour = 1800L;

    const long OneDay = 3600L * 24L;

    Event<void> NetworkTimeoutRebootEvent;

    Event<void> DailyMiddayRebootEvent;

    void Initialize()
    {
      NetworkTimeoutRebootEvent.Subscribe(OnNetworkTimeoutRebootEvent);
      Services::System::InvokeOnce(&NetworkTimeoutRebootEvent, TenMinutesFiveSeconds);

      Services::Time::TimeSyncedEvent.Subscribe(OnTimeSyncedEvent);
    }

    void SetupDailyMiddayReboot()
    {
      time_t rebootTime = GetDailyMiddayRebootTime();
      Timezone *localtime = &Services::Time::Localtime;

      Debug("ESP restart time: " + localtime->dateTime(rebootTime, LOCAL_TIME) + "\n");

      localtime->setEvent([] { DailyMiddayRebootEvent.Invoke(nullptr); }, rebootTime, LOCAL_TIME);
    }

    void OnNetworkTimeoutRebootEvent(void *args)
    {
      NetworkTimeoutRebootEvent.Unsubscribe(OnNetworkTimeoutRebootEvent);

      Debug("ESP is not able to connect to WiFi and/or NTP service.\nRebooting ESP, hoping to fix issue with it.\n");
      DebugFlush();

      ESP.restart();
    }

    void OnDailyMiddayRebootEvent(void *args)
    {
      DailyMiddayRebootEvent.Unsubscribe(OnDailyMiddayRebootEvent);

      Debug("Daily midday reboot!.\n");
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

    time_t GetDailyMiddayRebootTime()
    {
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