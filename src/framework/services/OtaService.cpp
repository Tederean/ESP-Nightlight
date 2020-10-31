#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/services/SystemService.h>
#include <ArduinoOTA.h>
#include <string>

using namespace std;

namespace Services
{
  namespace Ota
  {

    string Password;

    string Hostname;

    bool OtaEnabled;

    bool OtaIsUpdating;

    Event<void> UpdateStartEvent;

    Event<void> UpdateEndEvent;

    Event<void> UpdateErrorEvent;

    void OnLoopEvent(void *args)
    {
      ArduinoOTA.handle();
    }

    void EndOta()
    {
#if defined(ESP32)
      ArduinoOTA.end();
#endif

      Services::System::LoopEvent.Unsubscribe(&OnLoopEvent);
    }

    void BeginOta()
    {
      ArduinoOTA.setHostname(Hostname.c_str());
      ArduinoOTA.setPassword((const char *)Password.c_str());
      ArduinoOTA.setPort(8266);

#if defined(ESP8266)
      ArduinoOTA.begin(true);
#elif defined(ESP32)
      ArduinoOTA.setMdnsEnabled(true);
      ArduinoOTA.begin();
#endif

      Services::System::LoopEvent.Subscribe(&OnLoopEvent);
    }

    void OnAccessPointDisconnectedEvent(void *args)
    {
      EndOta();
    }

    void OnAccessPointConnectedEvent(void *args)
    {
      BeginOta();
    }

    bool IsEnabled()
    {
      return OtaEnabled;
    }

    bool IsUpdating()
    {
      return OtaIsUpdating;
    }

    void DisableOta()
    {
      if (!OtaEnabled || OtaIsUpdating)
        return;

      if (Services::Wifi::IsConnected())
      {
        EndOta();
      }

      Services::Wifi::AccessPointConnectedEvent.Unsubscribe(&OnAccessPointConnectedEvent);
      Services::Wifi::AccessPointDisconnectedEvent.Unsubscribe(&OnAccessPointDisconnectedEvent);

      Hostname.clear();
      Password.clear();

      OtaEnabled = false;
    }

    void EnableOta(string hostname, string password)
    {
      if (OtaEnabled || OtaIsUpdating)
        return;

      Hostname.assign(hostname);
      Password.assign(password);

      if (Services::Wifi::IsConnected())
      {
        BeginOta();
      }

      Services::Wifi::AccessPointConnectedEvent.Subscribe(&OnAccessPointConnectedEvent);
      Services::Wifi::AccessPointDisconnectedEvent.Subscribe(&OnAccessPointDisconnectedEvent);

      OtaEnabled = true;
    }

    void Initialize()
    {
      OtaEnabled = false;
      OtaIsUpdating = false;

      ArduinoOTA.onStart([]
      {
        UpdateStartEvent.Invoke(nullptr);

        OtaIsUpdating = true;
      });

      ArduinoOTA.onEnd([]
      {
        UpdateEndEvent.Invoke(nullptr);

        OtaIsUpdating = false;
      });

      ArduinoOTA.onError([](ota_error_t error)
      {
        UpdateErrorEvent.Invoke(nullptr);

        OtaIsUpdating = false;
      });
    }

  } // namespace Ota
} // namespace Services