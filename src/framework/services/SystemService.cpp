#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>
#include <stdint.h>
#include <Arduino.h>

using namespace std;

namespace Services
{
  namespace System
  {

    Event<void> LoopEvent;

    void InvokeOnce(Event<void> *event, int64_t delay_us)
    {
    }

    void InvokeRepeating(Event<void> *event, int64_t firstDelay_us, int64_t repeatingDelay_us)
    {
    }

    void InvokeCancel(Event<void> *event)
    {
    }

    void InitializeAllServices()
    {
#ifdef SERIAL_DEBUG
      Serial.begin(115200UL);
      Serial.setDebugOutput(true);
#endif

      Services::Wifi::Initialize();
      Services::Ota::Initialize();
      Services::Time::Initialize();
    }

  } // namespace System
} // namespace Services