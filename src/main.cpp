#if !defined(ESP8266) && !defined(ESP32)
#error "Unkown or unsupported architecture!"
#endif

#include <Arduino.h>
#include <application/services/SchedulerService.h>
#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/services/TimeService.h>
#include <framework/services/SystemService.h>

void loop()
{
	Services::System::LoopEvent.Invoke(nullptr);
}

void setup()
{
#ifdef SERIAL_DEBUG
	Serial.begin(115200UL);
	Serial.setDebugOutput(true);
#endif

	Services::System::Initialize();
	Services::Wifi::Initialize();
	Services::Ota::Initialize();
	Services::Time::Initialize();

	Services::Scheduler::Initialize();
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