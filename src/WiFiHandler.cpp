#include <WiFiHandler.h>
#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LightHandler.h>

#if defined(ESP8266)
	#include <ESP8266WiFi.h>
	#include <Ticker.h>
#elif defined(ESP32)
	#include <WiFi.h>
#endif


namespace WiFiHandler
{
	#if defined(ESP8266)
		WiFiEventHandler esp8266WifiConnectHandler;
		WiFiEventHandler esp8266WifiDisconnectHandler;

		Ticker wiFiShutdownEventTimer;
	#elif defined(ESP32)
		TimerHandle_t wiFiShutdownEventTimer;
	#endif

	volatile bool wiFiShutdownEventTriggered;


	void reconnectToWiFi()
	{
		#ifdef SERIAL_DEBUG
			Serial.println("Reconnecting to WiFi...");
		#endif

		WiFi.disconnect(true);
		WiFi.begin(WIFI_SSID, WIFI_PSK);

		#if defined(ESP8266)
			WiFi.hostname(WIFI_NAME);
		#elif defined(ESP32)
			WiFi.setHostname(WIFI_NAME);
		#endif
	}

	void doWiFiShutdownEvent()
	{
		wiFiShutdownEventTriggered = true;

		if (LightHandler::isTimeSynced())
		{
			#ifdef SERIAL_DEBUG
				Serial.print("Disconnecting from WiFi after await period has expired, everything ok!\n\n");
			#endif

			WiFi.disconnect(true);
		}
		else
		{
			#ifdef SERIAL_DEBUG
				Serial.print("ESP is not able to connect to WiFi and/or NTP service.\n");
				Serial.print("Rebooting ESP, hoping to fix issue with it.\n\n");
				Serial.flush();
			#endif

			ESP.restart();
		}
	}

	void onWifiConnectEvent()
	{
		#ifdef SERIAL_DEBUG
			Serial.println("Connected to WiFi...");
		#endif

		ArduinoOTA.setHostname(WIFI_NAME);
		ArduinoOTA.setPassword((const char*) WIFI_NAME);
		ArduinoOTA.setPort(8266);

		#if defined(ESP8266)
			ArduinoOTA.begin(true);
		#elif defined(ESP32)
			ArduinoOTA.setMdnsEnabled(true);
			ArduinoOTA.begin();
		#endif
	}

	void onWifiDisconnectEvent()
	{
		#ifdef SERIAL_DEBUG
			Serial.println("Disconnected from WiFi...");
		#endif

		#if defined(ESP32)
			ArduinoOTA.end();
		#endif

		if (!wiFiShutdownEventTriggered)
			reconnectToWiFi();
	}

	#if defined(ESP8266)
		void esp8266OnWifiConnect(const WiFiEventStationModeGotIP &event)
		{
			onWifiConnectEvent();
		}

		void esp8266OnWifiDisconnect(const WiFiEventStationModeDisconnected &event)
		{
			onWifiDisconnectEvent();
		}
	#elif defined(ESP32)
		void esp32OnWifiEvent(WiFiEvent_t event)
		{
			switch (event)
			{
			case SYSTEM_EVENT_STA_GOT_IP:
				onWifiConnectEvent();
				break;

			case SYSTEM_EVENT_STA_DISCONNECTED:
				onWifiDisconnectEvent();
				break;

			case SYSTEM_EVENT_WIFI_READY:
			case SYSTEM_EVENT_SCAN_DONE:
			case SYSTEM_EVENT_STA_START:
			case SYSTEM_EVENT_STA_STOP:
			case SYSTEM_EVENT_STA_CONNECTED:
			case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
			case SYSTEM_EVENT_STA_LOST_IP:
			case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
			case SYSTEM_EVENT_STA_WPS_ER_FAILED:
			case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
			case SYSTEM_EVENT_STA_WPS_ER_PIN:
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
	#endif

	void loop()
	{
		if (WiFi.status() == WL_CONNECTED)
		{
			ArduinoOTA.handle();
		}
	}

	void setup()
	{
		wiFiShutdownEventTriggered = false;

		#if defined(ESP8266)
			esp8266WifiConnectHandler = WiFi.onStationModeGotIP(esp8266OnWifiConnect);
			esp8266WifiDisconnectHandler = WiFi.onStationModeDisconnected(esp8266OnWifiDisconnect);

			wiFiShutdownEventTimer.once(WIFI_SHUTDOWN_TIME, doWiFiShutdownEvent);
		#elif defined(ESP32)
			WiFi.onEvent(esp32OnWifiEvent);

			wiFiShutdownEventTimer = xTimerCreate("wiFiTimer", pdMS_TO_TICKS(WIFI_SHUTDOWN_TIME*1000UL), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(doWiFiShutdownEvent));
			xTimerStart(wiFiShutdownEventTimer, 0);
		#endif

		reconnectToWiFi();

		while (WiFi.status() != WL_CONNECTED) delay(10);
	}
}