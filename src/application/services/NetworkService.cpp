#include <Arduino.h>
#include <application/services/NetworkService.h>
#include <framework/services/SystemService.h>
#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/common/Event.h>

using namespace std;

namespace Services
{
  namespace Network
  {

    void Initialize();

    void OnEnableWifiEvent(void *args);

    void OnDisableWifiEvent(void *args);

    const timespan_t WifiEnableTime = WIFI_ENABLE_TIME * 1000 * 1000;

    const timespan_t WifiDisableTime = WIFI_DISABLE_TIME * 1000 * 1000;

    Event<void> EnableWifiEvent;

    Event<void> DisableWifiEvent;

    void Initialize()
    {
      EnableWifiEvent.Subscribe(OnEnableWifiEvent);
      Services::System::InvokeLater(&EnableWifiEvent, WifiEnableTime, TimerMode::Single);

      DisableWifiEvent.Subscribe(OnDisableWifiEvent);
      Services::System::InvokeLater(&DisableWifiEvent, WifiDisableTime, TimerMode::Single);
    }

    void OnEnableWifiEvent(void *args)
    {
      EnableWifiEvent.Unsubscribe(OnEnableWifiEvent);

      Services::Wifi::EnableWifi(WIFI_SSID, WIFI_PSK, WIFI_NAME);
      Services::Ota::EnableOta(WIFI_NAME, WIFI_NAME);

      Debug("Enabled WiFi.\n");
    }

    void OnDisableWifiEvent(void *args)
    {
      DisableWifiEvent.Unsubscribe(OnDisableWifiEvent);

      Debug("Disabled WiFi.\n");

      Services::Ota::DisableOta();
      Services::Wifi::DisableWifi();
    }

  } // namespace Network
} // namespace Services