#ifndef _SystemService_
#define _SystemService_

#include <framework/common/Event.h>

#if !defined(ESP8266) && !defined(ESP32)
#error "Unkown or unsupported architecture!"
#endif

using namespace std;

namespace Services
{
  namespace System
  {

    void InitializeAllServices();

    extern Event<void> LoopEvent;

  } // namespace System
} // namespace Services

#endif
