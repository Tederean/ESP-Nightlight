#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>
#include <Arduino.h>

using namespace std;

namespace Services
{
  namespace System
  {

    Event<void> LoopEvent;

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