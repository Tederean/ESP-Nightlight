#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>
#include <Arduino.h>
#include <ezTime.h>

using namespace std;

namespace Services
{
  namespace Time
  {

    Event<void> TimeSyncedEvent;

    bool TimeSynced;

    bool IsTimeSynced()
    {
      return TimeSynced;
    }

    void OnLoopEvent(void *args)
    {
      ezt::events();
    }

    void OnLoopEventTimeSync(void *args)
    {
      if (ezt::timeStatus() == timeStatus_t::timeSet)
      {
        TimeSyncedEvent.Invoke(nullptr);

        Services::System::LoopEvent.Unsubscribe(&OnLoopEventTimeSync);

        ezt::setInterval(0);
        TimeSynced = true;
      }
    }

    void Initialize()
    {
      TimeSynced = false;

#ifdef SERIAL_DEBUG
      ezt::setDebug(INFO);
#endif

      ezt::setServer(TZ_NTP_SERVER);

      Services::System::LoopEvent.Subscribe(&OnLoopEvent);
      Services::System::LoopEvent.Subscribe(&OnLoopEventTimeSync);
    }

  } // namespace Time
} // namespace Services