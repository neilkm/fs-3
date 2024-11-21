#pragma once

#include "mbed.h"
#include "AnalogIn.h"
#include "pinmap.h"

#include <chrono>
#include <cstdint>
#include <cstdio>

namespace ETC {

#if !DEVICE_CAN
#error [NOT_SUPPORTED] CAN not supported for this target
#endif

#define MAX_V 3.3
#define BRAKE_TOL .1
#define MAXSPEED 7500
#define CAN_RX_PIN D10
#define CAN_TX_PIN D2
#define CAN_FREQ 500000
#define MAX_TORQUE 20000

#define DEBUGLEVEL 0

class ThrottleController
{
    public:
        ThrottleController();
        ~ThrottleController();

    private:
        // Constant tested range of values for pedal travel calculation
        const float HE1_LOW = .15;
        const float HE1_HIGH = .73;
        const float HE2_LOW = .14;
        const float HE2_HIGH = .57;

        mbed::AnalogIn HE1;
        mbed::AnalogIn HE2;
        mbed::AnalogIn brakes;
        mbed::InterruptIn Cockpit;
        mbed::DigitalOut RTDScontrol;
        mbed::DigitalOut test_led;
};

//NNK instead of commenting out prints, we can add global debug variables of different levels to turn on or off debug output
void debugLevelPrint(int level, std::string message) {
    if (level >= DEBUGLEVEL) {
        printf(message);
    }
    return;
}

}