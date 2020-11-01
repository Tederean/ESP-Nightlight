#ifndef _SystemService_
#define _SystemService_

#include <framework/common/Event.h>
#include <stdint.h>

using namespace std;

namespace Services
{
  namespace System
  {

    void Initialize();

    void InvokeOnce(Event<void> *event, int64_t delay_us);

    void InvokeRepeating(Event<void> *event, int64_t firstDelay_us, int64_t repeatingDelay_us);

    void InvokeCancel(Event<void> *event);

    extern Event<void> LoopEvent;

  } // namespace System
} // namespace Services

#endif
