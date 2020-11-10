#include <Arduino.h>
#include <application/services/NetworkService.h>
#include <application/services/LightService.h>
#include <application/services/RebootService.h>
#include <framework/services/SystemService.h>
#include <framework/services/WifiService.h>
#include <framework/services/OtaService.h>
#include <framework/services/TimeService.h>

void loop()
{
	Services::System::LoopEvent.Invoke(nullptr);
}

void setup()
{
	Services::System::Initialize();
	Services::Wifi::Initialize();
	Services::Ota::Initialize();
	Services::Time::Initialize();

	Services::Light::Initialize();
	Services::Network::Initialize();
	Services::Reboot::Initialize();
}