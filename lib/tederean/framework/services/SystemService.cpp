#include <Arduino.h>
#include <memory>
#include <framework/services/SystemService.h>
#include <framework/common/Event.h>

using namespace std;

typedef struct
{
  Event<void> *TargetEvent;
  int64_t NextExecution_us;
  int64_t Interval_us;
} ScheduledEvent;

namespace Services
{
  namespace System
  {

    void Initialize();

    int64_t GetUptime_us();

    void InvokeOnce(Event<void> *event, int64_t delay_us);

    void InvokeRepeating(Event<void> *event, int64_t firstDelay_us, int64_t repeatingDelay_us);

    void InvokeCancel(Event<void> *event);

    void AddEvent(Event<void> *event, int64_t firstDelay_us, int64_t interval_us);

    void RemoveEvent(Event<void> *event);

    int16_t FindEventIndex(Event<void> *eventToFind);

    void OnLoopEvent(void *args);

    Event<void> LoopEvent;

    vector<shared_ptr<ScheduledEvent>> ScheduledTargets;

    void Initialize()
    {
#ifdef SERIAL_DEBUG
      Serial.begin(115200UL);
      Serial.setDebugOutput(true);
#endif
    }

    int64_t GetUptime_us()
    {
#if defined(ESP32)
      return esp_timer_get_time();
#elif defined(ESP8266)
      return micros64();
#endif
    }

    void InvokeOnce(Event<void> *event, int64_t delay_us)
    {
      AddEvent(event, delay_us, -1);
    }

    void InvokeRepeating(Event<void> *event, int64_t firstDelay_us, int64_t repeatingDelay_us)
    {
      AddEvent(event, firstDelay_us, repeatingDelay_us);
    }

    void InvokeCancel(Event<void> *event)
    {
      RemoveEvent(event);
    }

    void AddEvent(Event<void> *event, int64_t firstDelay_us, int64_t interval_us)
    {
      int16_t index = FindEventIndex(event);

      if (index < 0)
      {
        int64_t nextExecution_us = GetUptime_us() + firstDelay_us;

        shared_ptr<ScheduledEvent> scheduledEvent((ScheduledEvent *)malloc(sizeof(ScheduledEvent)));
        scheduledEvent->TargetEvent = event;
        scheduledEvent->NextExecution_us = nextExecution_us;
        scheduledEvent->Interval_us = interval_us;

        if (ScheduledTargets.empty())
        {
          LoopEvent.Subscribe(OnLoopEvent);
        }

        ScheduledTargets.push_back(scheduledEvent);
      }
    }

    void RemoveEvent(Event<void> *event)
    {
      int16_t index = FindEventIndex(event);

      if (index >= 0)
      {
        ScheduledTargets.erase(ScheduledTargets.begin() + index);

        if (ScheduledTargets.empty())
        {
          LoopEvent.Unsubscribe(OnLoopEvent);
        }
      }
    }

    int16_t FindEventIndex(Event<void> *eventToFind)
    {
      for (size_t index = 0; index < ScheduledTargets.size(); ++index)
      {
        shared_ptr<ScheduledEvent> scheduledEvent = ScheduledTargets[index];

        if (scheduledEvent->TargetEvent == eventToFind)
          return index;
      }

      return -1;
    }

    void OnLoopEvent(void *args)
    {
      vector<Event<void> *> TargetsToRemove;
      int64_t presentTime = GetUptime_us();

      for (size_t index = 0; index < ScheduledTargets.size(); ++index)
      {
        shared_ptr<ScheduledEvent> scheduledEvent = ScheduledTargets[index];

        if (scheduledEvent->NextExecution_us > presentTime)
          continue;

        scheduledEvent->TargetEvent->Invoke(nullptr);

        if (scheduledEvent->Interval_us <= 0)
        {
          TargetsToRemove.push_back(scheduledEvent->TargetEvent);
          continue;
        }

        scheduledEvent->NextExecution_us += scheduledEvent->Interval_us;
      }

      for (size_t index = 0; index < TargetsToRemove.size(); ++index)
      {
        RemoveEvent(TargetsToRemove[index]);
      }
    }

  } // namespace System
} // namespace Services