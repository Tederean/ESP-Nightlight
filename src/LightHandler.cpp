#include <LightHandler.h>
#include <LightUtils.h>
#include <Arduino.h>
#include <ezTime.h>


namespace LightHandler
{
    #if defined(ESP32) && (PWM_RESOLUTION > 1)
        #define PWM_CHANNEL 0
    #endif


    const uint16_t PWM_MIN = 0;
    const uint16_t PWM_MAX = roundFloatingToInteger( (pow(2, PWM_RESOLUTION)-1) * PWM_RATIO );

    Timezone *_local_tz;


    void writePWM(uint16_t value)
    {
        #if defined(PWM_INVERTED)
            value = ~value;
            value = value << (16 - PWM_RESOLUTION);
            value = value >> (16 - PWM_RESOLUTION);
        #endif

        #if (PWM_RESOLUTION > 1)
            #if defined(ESP8266)
                analogWrite(PWM_PIN, value);
            #elif defined(ESP32)
                ledcWrite(PWM_CHANNEL, value);
            #endif
        #else
            digitalWrite(PWM_PIN, value);
        #endif
    }
    
    double getElapsedSecondsSinceMidnight(time_t now)
    {
        return
        (
            _local_tz->hour(now, UTC_TIME)*3600.0
            +
            _local_tz->minute(now, UTC_TIME)*60.0
            +
            _local_tz->second(now, UTC_TIME)
            +
            _local_tz->ms(LAST_READ)*0.001
        );
    }
    
    uint16_t convertSunPathToDutyCycle(double value)
    {
        if (value >= 0.0)
        {
            return PWM_MIN;
        }

        if (value <= -6.0)
        {
            return PWM_MAX;
        }

        return map((-50.0/3.0) * value, 0.0, 100.0, PWM_MIN, PWM_MAX);
    }

    uint16_t convertSecondsSinceMidnightToDutyCycle(double value)
    {
        #ifdef EVENING_LIGHT_ENABLED
            if (value >= (23*60*60)) // >23:00
            {
                return PWM_MIN;
            }

            if (value >= (22*60*60 + 59*60)) // >22:59
            {
                return map((-5.0/3.0)*value +138000.0, 0.0, 100.0, PWM_MIN, PWM_MAX);
            }
        #else
            if (value >= (12*60*60 + 1*60)) // >12:01
            {
                return PWM_MIN;
            }
        #endif

        #ifdef MORING_LIGHT_ENABLED
            if (value >= (6*60*60 + 1*60)) // >06:01
            {
                return PWM_MAX;
            }

            if (value >= (6*60*60)) // >06:00
            {
                return map((5.0/3.0)*value -36000.0, 0.0, 100.0, PWM_MIN, PWM_MAX);
            }
        #else
            if (value >= (12*60*60)) // >12:00
            {
                return PWM_MAX;
            }
        #endif


        return PWM_MIN;
    }

    double getSunPathAngleInDegrees(time_t now)
    {
        if (_local_tz->isDST(now, UTC_TIME))
        {
            now -= _local_tz->getOffset(now, UTC_TIME) *60L; //We require natural time.
        }

        double declin = -23.45*cos(DEG_TO_RAD*360.0*(_local_tz->dayOfYear(now, UTC_TIME) +10.0)/365.0);

        double time_equation = 60.0*(-0.171*sin(0.0337*_local_tz->dayOfYear(now, UTC_TIME) + 0.465) - 0.1299*sin(0.01787*_local_tz->dayOfYear(now, UTC_TIME) - 0.168));

        double hour_angle = 15.0*( (getElapsedSecondsSinceMidnight(now)/3600.0) - (15.0-LONGITUDE)/15.0 - 12.0 + time_equation/60.0);

        double sun_angle = sin(DEG_TO_RAD*LATITUDE)*sin(DEG_TO_RAD*declin) + cos(DEG_TO_RAD*LATITUDE)*cos(DEG_TO_RAD*declin)*cos(DEG_TO_RAD*hour_angle);

        return asin(sun_angle)/DEG_TO_RAD;
    }

    void doRestartEvent()
    {
        #ifdef SERIAL_DEBUG
            Serial.print("Daily restart event triggered!\n\n");
            Serial.flush();
        #endif

        ESP.restart();
    }

    void setupRestartEvent()
    {
        time_t now = _local_tz->now();
        tmElements_t timeBuilder;

        // Load present time into builder. 
        breakTime(now, timeBuilder);

        timeBuilder.Hour = 12;
        timeBuilder.Minute = 0;
        timeBuilder.Second = 0;

        // Add or subtract 10 minutes random.
        time_t restart_time = makeTime(timeBuilder) + random(-600, 600);

        // Postpone to next day if period to restart is fewer than half an hour.
        if (((int64_t) restart_time - now) < 1800L)
        {
            restart_time += (3600UL*24UL);
        }

        #ifdef SERIAL_DEBUG
            Serial.print("ESP restart time: ");
            Serial.print(_local_tz->dateTime(restart_time, LOCAL_TIME));
            Serial.println('\n');
        #endif

        _local_tz->setEvent(doRestartEvent, restart_time, LOCAL_TIME);
    }

    bool isTimeSynced()
    {
        return timeStatus() == timeStatus_t::timeSet;
    }

    void loop()
    {
        events();

        time_t now = UTC.now();
        
        #ifdef TIME_LIB_DST_FIX
            if (_local_tz->isDST(now, UTC_TIME))
            {
                now += 3L*60L*60L; //Fix shitty dst calculation fail...
            }
        #endif

        double sunPath = getSunPathAngleInDegrees(now);
        uint16_t sunPathDutyCycle = convertSunPathToDutyCycle(sunPath);

        double secondsSinceMidnight = getElapsedSecondsSinceMidnight(now);
        uint16_t elapsedDayTimeDutyCycle = convertSecondsSinceMidnightToDutyCycle(secondsSinceMidnight);

        writePWM( min(sunPathDutyCycle, elapsedDayTimeDutyCycle) );

        /*#ifdef SERIAL_DEBUG
            #if defined(ESP8266)
                yield();
            #endif

            Serial.print(sunPath, 3);
            Serial.print('@');

            Serial.print(_local_tz->hour(now, UTC_TIME));
            Serial.print(':');
            Serial.print(_local_tz->minute(now, UTC_TIME));
            Serial.println('\n');

            delay(1000);
        #endif*/
    }

    void setupPart2()
    {
        #ifdef SERIAL_DEBUG
            setDebug(INFO);
        #endif

        setServer(TZ_NTP_SERVER);
        waitForSync();

        _local_tz = new Timezone();
        _local_tz->setPosix(TZ_INFO);
        _local_tz->setDefault();

        setupRestartEvent();
        setInterval(0);
    }

    void setupPart1()
    {
        #if (PWM_RESOLUTION > 1)
            #if defined(ESP8266)
                analogWriteFreq(PWM_FREQUENCY);
                analogWriteRange(pow(2, PWM_RESOLUTION) -1);
                pinMode(PWM_PIN, OUTPUT);
            #elif defined(ESP32)
                ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
                ledcAttachPin(PWM_PIN, PWM_CHANNEL);
            #endif
        #else
            pinMode(PWM_PIN, OUTPUT);
        #endif

        writePWM(0);
    }
}