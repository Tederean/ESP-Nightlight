#ifndef _TimerService_
#define _TimerService_

#include <Arduino.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace Time
  {

    void Initialize();

    bool IsTimeSynced();

    extern Event<void> TimeSyncedEvent;

  } // namespace Wifi
} // namespace Services

#endif
