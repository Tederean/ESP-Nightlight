
#include <Arduino.h>
#include <ezTime.h>
#include <framework/services/SystemService.h>
#include <framework/services/WifiService.h>
#include <framework/services/TimeService.h>

void loop()
{
	Services::System::LoopEvent.Invoke(nullptr);
}

void OnWifiEnabledEvent(void *args)
{
	Serial.println("OnWifiEnabledEvent");
}

void OnWifiDisabledEvent(void *args)
{
	Serial.println("OnWifiDisabledEvent");
}

void OnAccessPointConnectedEvent(void *args)
{
	Serial.println("OnAccessPointConnectedEvent");
}

void OnAccessPointDisconnectedEvent(void *args)
{
	Serial.println("OnAccessPointDisconnectedEvent");
}

void OnTimeSyncedEvent(void *args)
{
	Timezone local;

	local.setPosix(TZ_INFO);
	local.setDefault();

	time_t now = local.now();

	Serial.println(local.dateTime(now, LOCAL_TIME));
}

void setup()
{
	Serial.println("main_1");

	Services::System::InitializeAllServices();

	Serial.println("main_2");

	Services::Wifi::WifiEnablingEvent.Subscribe(&OnWifiEnabledEvent);
	Services::Wifi::WifiDisablingEvent.Subscribe(&OnWifiDisabledEvent);

	Services::Wifi::AccessPointConnectedEvent.Subscribe(&OnAccessPointConnectedEvent);
	Services::Wifi::AccessPointDisconnectedEvent.Subscribe(&OnAccessPointDisconnectedEvent);

	Services::Time::TimeSyncedEvent.Subscribe(&OnTimeSyncedEvent);

	Serial.println("main_3");

	Services::Wifi::EnableWifi(WIFI_SSID, WIFI_PSK, WIFI_NAME);

	Serial.println("main_4");
}

/*
#include <Arduino.h>
#include <LightHandler.h>
#include <WiFiHandler.h>


#if !defined(ESP8266) && !defined(ESP32)
	#error "Unkown or unsupported architecture!"
#endif


void loop()
{
	WiFiHandler::loop();
	
	LightHandler::loop();
}

void setup()
{
	#ifdef SERIAL_DEBUG
		Serial.begin(115200UL);
		Serial.setDebugOutput(true);
	#endif

	LightHandler::setupPart1();

	WiFiHandler::setup();

	LightHandler::setupPart2();
}
*/