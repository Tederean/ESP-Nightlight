#ifndef _OtaService_
#define _OtaService_

#include <string>

using namespace std;

namespace Services
{
  namespace Ota
  {

    void Initialize();

    void EnableOta(string hostname, string password);

    void DisableOta();

    bool IsEnabled();

    bool IsUpdating();

    extern Event<void> UpdateStartEvent;

    extern Event<void> UpdateEndEvent;

    extern Event<void> UpdateErrorEvent;

  } // namespace Ota
} // namespace Services

#endif