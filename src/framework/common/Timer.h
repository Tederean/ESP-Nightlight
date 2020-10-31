#ifndef _Timer_
#define _Timer_

#include <vector>
#include <stdint.h>
#include <esp_timer.h>
#include <framework/common/Event.h>
#include <framework/services/SystemService.h>

using namespace std;

template <class T>
class Timer
{

private:
  vector<void (*)(T *)> ExecutionTargets;

  int64_t ScheduledExecutionTime;

  T *EventArguments;

  void ResetTimer()
  {
    ScheduledExecutionTime = -1;
    EventArguments = nullptr;
  }

  void OnLoopEvent(void *args)
  {
    if (esp_timer_get_time() >= ScheduledExecutionTime)
    {
      for (auto target : ExecutionTargets)
      {
        target(EventArguments);
      }

      Services::System::LoopEvent.Unsubscribe(&OnLoopEvent);

      ResetTimer();
    }
  }

public:
  void Subscribe(void (*function)(T *))
  {
    if (!(find(ExecutionTargets.begin(), ExecutionTargets.end(), function) != ExecutionTargets.end()))
    {
      ExecutionTargets.push_back(function);
    }
  }

  void Unsubscribe(void (*function)(T *))
  {
    if (find(ExecutionTargets.begin(), ExecutionTargets.end(), function) != ExecutionTargets.end())
    {
      ExecutionTargets.erase(remove(ExecutionTargets.begin(), ExecutionTargets.end(), function), ExecutionTargets.end());
    }
  }

  int64_t GetScheduledExecutionTime()
  {
    return ScheduledExecutionTime;
  }

  void Cancel()
  {
    Services::System::LoopEvent.Unsubscribe(&OnLoopEvent);

    ResetTimer();
  }

  void RunDelayed(int64_t executionDelay, T *handler)
  {
    int64_t scheduledExecutionTime = esp_timer_get_time() + executionDelay;

    RunAt(scheduledExecutionTime, handler);
  }

  void RunAt(int64_t executionTime, T *handler)
  {
    ScheduledExecutionTime = executionTime;
    EventArguments = handler;

    Services::System::LoopEvent.Subscribe(&OnLoopEvent);
  }

  Timer()
  {
    ResetTimer();
  }
};

#endif
