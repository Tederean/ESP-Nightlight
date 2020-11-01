#include <framework/services/SystemService.h>
#include <framework/common/Event.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <vector>
//#include <esp_timer.h>
#include <Arduino.h>

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

    Event<void> LoopEvent;

    vector<ScheduledEvent *> ScheduledTargets;

    ScheduledEvent *CreateScheduledEvent()
    {
      return (ScheduledEvent *)malloc(sizeof(ScheduledEvent));
    }

    void DeleteScheduledEvent(ScheduledEvent *event)
    {
      free(event);
    }

    int16_t FindEventIndex(Event<void> *eventToFind)
    {
      for (int16_t index = 0; index < ScheduledTargets.size(); ++index)
      {
        ScheduledEvent *scheduledEvent = ScheduledTargets[index];

        if (scheduledEvent->TargetEvent == eventToFind)
          return index;
      }

      return -1;
    }

    void RemoveEvent(Event<void> *event);

    void OnLoopEvent(void *args)
    {
      vector<Event<void> *> TargetsToRemove;
      int64_t presentTime = esp_timer_get_time();

      for (int16_t index = 0; index < ScheduledTargets.size(); ++index)
      {
        ScheduledEvent *scheduledEvent = ScheduledTargets[index];

        if (scheduledEvent->NextExecution_us > presentTime)
          continue;

        scheduledEvent->TargetEvent->Invoke(nullptr);

        if (scheduledEvent->Interval_us > 0)
        {
          scheduledEvent->NextExecution_us += scheduledEvent->Interval_us;
          continue;
        }

        TargetsToRemove.push_back(scheduledEvent->TargetEvent);
      }

      for (int16_t index = 0; index < TargetsToRemove.size(); ++index)
      {
        RemoveEvent(TargetsToRemove[index]);
      }
    }

    void AddEvent(Event<void> *event, int64_t firstDelay_us, int64_t interval_us)
    {
      int16_t index = FindEventIndex(event);

      if (index < 0)
      {
        int64_t nextExecution_us = esp_timer_get_time() + firstDelay_us;

        ScheduledEvent *scheduledEvent = CreateScheduledEvent();
        scheduledEvent->TargetEvent = event;
        scheduledEvent->NextExecution_us = nextExecution_us;
        scheduledEvent->Interval_us = interval_us;

        if (ScheduledTargets.empty())
        {
          LoopEvent.Subscribe(&OnLoopEvent);
        }

        ScheduledTargets.push_back(scheduledEvent);
      }
    }

    void RemoveEvent(Event<void> *event)
    {
      int16_t index = FindEventIndex(event);

      if (index >= 0)
      {
        ScheduledEvent *scheduledEvent = ScheduledTargets[index];

        ScheduledTargets.erase(ScheduledTargets.begin() + index);

        if (ScheduledTargets.empty())
        {
          LoopEvent.Unsubscribe(&OnLoopEvent);
        }

        DeleteScheduledEvent(scheduledEvent);
      }
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

    void Initialize()
    {
    }

  } // namespace System
} // namespace Services