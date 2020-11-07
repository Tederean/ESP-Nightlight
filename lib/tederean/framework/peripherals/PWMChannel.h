#ifndef _LightChannelConfig_
#define _LightChannelConfig_

#include <Arduino.h>

using namespace std;

enum class LightChannelType
{
  DigitalPin,
  ESP8266PWM,
  ESP32PWM,
};

class PWMChannel
{

private:
  LightChannelType ChannelType;

  uint8_t Pin;

  bool InvertSignal;

  double Frequency_Hz;

  uint8_t Resolution_bit;

  uint16_t PwmMinValue;

  uint16_t PwmMaxValue;

  uint8_t LedChannel;

  uint16_t PwmRatioToValue(uint8_t resolution_bit, double ratio);

  uint16_t ResolutionToCounts(uint8_t resolution_bit);

public:
  PWMChannel(uint8_t pin, bool invertSignal);

  PWMChannel(uint8_t pin, bool invertSignal, double frequency_Hz, uint8_t resolution_bit, uint8_t ledChannel, double pwmMinRatio = 0.0, double pwmMaxRatio = 1.0);

  PWMChannel(uint8_t pin, bool invertSignal, double frequency_Hz, uint8_t resolution_bit, double pwmMinRatio = 0.0, double pwmMaxRatio = 1.0);

  void Initialize();

  void Write(uint16_t value);
};

#endif