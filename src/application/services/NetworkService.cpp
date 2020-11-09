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

    const int64_t WifiEnableTime = WIFI_ENABLE_TIME * 1000LL * 1000LL;

    const int64_t WifiDisableTime = WIFI_DISABLE_TIME * 1000LL * 1000LL;

    Event<void> EnableWifiEvent;

    Event<void> DisableWifiEvent;

    void Initialize()
    {
      EnableWifiEvent.Subscribe(OnEnableWifiEvent);
      Services::System::InvokeOnce(&EnableWifiEvent, WifiEnableTime);

      DisableWifiEvent.Subscribe(OnDisableWifiEvent);
      Services::System::InvokeOnce(&DisableWifiEvent, WifiDisableTime);
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