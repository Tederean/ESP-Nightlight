#include <framework/services/WifiService.h>
#include <framework/common/Event.h>
#include <string>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

using namespace std;

namespace Services
{
  namespace Wifi
  {

    string SSID;

    string Password;

    string Hostname;

    bool WifiEnabled;

    bool WifiConnected;

#if defined(ESP8266)
    WiFiEventHandler WifiConnectHandler;

    WiFiEventHandler WifiDisconnectHandler;
#endif

    Event<void> AccessPointConnectedEvent;

    Event<void> AccessPointDisconnectedEvent;

    Event<void> WifiEnablingEvent;

    Event<void> WifiDisablingEvent;

    void OnAccessPointDisconnected()
    {
      if (WifiConnected)
      {
        AccessPointDisconnectedEvent.Invoke(nullptr);

        WifiConnected = false;
      }
    }

    void OnAccessPointConnected()
    {
      AccessPointConnectedEvent.Invoke(nullptr);

      WifiConnected = true;
    }

#if defined(ESP32)
    void OnWifiEvent(WiFiEvent_t event)
    {
      switch (event)
      {
      case SYSTEM_EVENT_STA_CONNECTED:
        OnAccessPointConnected();
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        OnAccessPointDisconnected();
        break;

      case SYSTEM_EVENT_WIFI_READY:
      case SYSTEM_EVENT_SCAN_DONE:
      case SYSTEM_EVENT_STA_START:
      case SYSTEM_EVENT_STA_STOP:
      case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
      case SYSTEM_EVENT_STA_GOT_IP:
      case SYSTEM_EVENT_STA_LOST_IP:
      case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
      case SYSTEM_EVENT_STA_WPS_ER_FAILED:
      case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
      case SYSTEM_EVENT_STA_WPS_ER_PIN:
      case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
      case SYSTEM_EVENT_AP_START:
      case SYSTEM_EVENT_AP_STOP:
      case SYSTEM_EVENT_AP_STACONNECTED:
      case SYSTEM_EVENT_AP_STADISCONNECTED:
      case SYSTEM_EVENT_AP_STAIPASSIGNED:
      case SYSTEM_EVENT_AP_PROBEREQRECVED:
      case SYSTEM_EVENT_GOT_IP6:
      case SYSTEM_EVENT_ETH_START:
      case SYSTEM_EVENT_ETH_STOP:
      case SYSTEM_EVENT_ETH_CONNECTED:
      case SYSTEM_EVENT_ETH_DISCONNECTED:
      case SYSTEM_EVENT_ETH_GOT_IP:
      case SYSTEM_EVENT_MAX:
      default:
        break;
      }
    }

#elif defined(ESP8266)

    void OnWifiDisconnect(const WiFiEventStationModeDisconnected &event)
    {
      OnAccessPointDisconnected();
    }

    void OnWifiConnect(const WiFiEventStationModeGotIP &event)
    {
      OnAccessPointConnected();
    }

#endif

    void DisconnectFromWifi()
    {
      WiFi.disconnect(true);
    }

    void ConnectToWifi()
    {
      WiFi.begin(SSID.c_str(), Password.c_str());

#if defined(ESP8266)
      WiFi.hostname(Hostname.c_str());
#elif defined(ESP32)
      WiFi.setHostname(Hostname.c_str());
#endif
    }

    bool IsConnected()
    {
      return WifiConnected;
    }

    bool IsEnabled()
    {
      return WifiEnabled;
    }

    void DisableWifi()
    {
      if (!WifiEnabled)
        return;

      WifiDisablingEvent.Invoke(nullptr);

      DisconnectFromWifi();

      SSID.clear();
      Password.clear();
      Hostname.clear();

      WifiEnabled = false;
    }

    void EnableWifi(string ssid, string password, string hostname)
    {
      if (WifiEnabled)
        return;

      SSID.assign(ssid);
      Password.assign(password);
      Hostname.assign(hostname);

      WifiEnablingEvent.Invoke(nullptr);

      ConnectToWifi();

      WifiEnabled = true;
    }

    void Initialize()
    {
      WifiEnabled = false;
      WifiConnected = false;

#if defined(ESP8266)
      WifiConnectHandler = WiFi.onStationModeGotIP(OnWifiConnect);
      WifiDisconnectHandler = WiFi.onStationModeDisconnected(OnWifiDisconnect);
#elif defined(ESP32)
      WiFi.onEvent(OnWifiEvent);
#endif
    }
  } // namespace Wifi
} // namespace Services