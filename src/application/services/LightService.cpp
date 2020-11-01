#include <Arduino.h>
#include <application/services/LightService.h>
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

    void OnLoopEvent(void *args);

    void Initialize()
    {
      Services::Time::TimeSyncedEvent.Subscribe(OnTimeSyncedEvent);
    }

    void OnTimeSyncedEvent(void *args)
    {
      Services::Time::TimeSyncedEvent.Unsubscribe(OnTimeSyncedEvent);

      Services::System::LoopEvent.Subscribe(OnLoopEvent);
    }

    void OnLoopEvent(void *args)
    {
      // TODO: Handle light calculation here
    }

  } // namespace Light
} // namespace Services