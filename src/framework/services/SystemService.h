#ifndef _SystemService_
#define _SystemService_

#include <framework/common/Event.h>
#include <stdint.h>

#if !defined(ESP8266) && !defined(ESP32)
#error "Unkown or unsupported architecture!"
#endif

using namespace std;

namespace Services
{
  namespace System
  {

    void InitializeAllServices();

    void InvokeOnce(Event<void> *event, int64_t delay_us);

    void InvokeRepeating(Event<void> *event, int64_t firstDelay_us, int64_t repeatingDelay_us);

    void InvokeCancel(Event<void> *event);

    extern Event<void> LoopEvent;

  } // namespace System
} // namespace Services

#endif
