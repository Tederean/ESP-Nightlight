#ifndef _SystemService_
#define _SystemService_

#include <Arduino.h>
#include <framework/common/Event.h>

using namespace std;

#if !defined(ESP8266) && !defined(ESP32)
#error "Unkown or unsupported architecture!"
#endif

#ifdef SERIAL_DEBUG
#define Debug(x) Serial.print(x)
#else
#define Debug(x)
#endif

#ifdef SERIAL_DEBUG
#define DebugFlush(x) Serial.flush()
#else
#define DebugFlush(x)
#endif

namespace Services
{
  namespace System
  {

    void Initialize();

    int64_t GetUptime_us();

    void InvokeOnce(Event<void> *event, int64_t delay_us);

    void InvokeRepeating(Event<void> *event, int64_t firstDelay_us, int64_t repeatingDelay_us);

    void InvokeCancel(Event<void> *event);

    extern Event<void> LoopEvent;

  } // namespace System
} // namespace Services

#endif
