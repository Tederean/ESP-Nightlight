#include <Arduino.h>
#include <ezTime.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>

using namespace std;

namespace Services
{
  namespace Time
  {

    void Initialize();

    bool IsTimeSynced();

    void OnLoopEvent(void *args);

    void OnLoopEventTimeSync(void *args);

    Event<void> TimeSyncedEvent;

    bool TimeSynced;

    void Initialize()
    {
      TimeSynced = false;

#ifdef SERIAL_DEBUG
      ezt::setDebug(INFO);
#endif

      ezt::setServer(TZ_NTP_SERVER);

      Services::System::LoopEvent.Subscribe(OnLoopEvent);
      Services::System::LoopEvent.Subscribe(OnLoopEventTimeSync);
    }

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

        Services::System::LoopEvent.Unsubscribe(OnLoopEventTimeSync);

        ezt::setInterval(0);
        TimeSynced = true;
      }
    }

  } // namespace Time
} // namespace Services