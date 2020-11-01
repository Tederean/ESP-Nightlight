#include <Arduino.h>
#include <application/services/SchedulerService.h>
#include <application/services/LightService.h>
#include <framework/services/SystemService.h>
#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>

void loop()
{
	Services::System::LoopEvent.Invoke(nullptr);
}

void setup()
{
	Services::System::Initialize();

	Services::Light::Initialize();
	Services::Scheduler::Initialize();

	Services::Wifi::EnableWifi(WIFI_SSID, WIFI_PSK, WIFI_NAME);
	Services::Ota::EnableOta(WIFI_NAME, WIFI_NAME);
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