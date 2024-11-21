#pragma once 

#include <mbed.h>
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


class ThrottleController {
    public:
        ThrottleController();
        ~ThrottleController();

        //NNK are these being changed? if not they can be constants at top of .cpp 
        // Constant tested range of values for pedal travel calculation
        const float HE1_LOW = .15;
        const float HE1_HIGH = .73;
        const float HE2_LOW = .14;
        const float HE2_HIGH = .57;

        //NNK where do these come from? when do they need to be instantiated?
        AnalogIn HE1;
        AnalogIn HE2(A0);
        AnalogIn brakes(A2);
        InterruptIn Cockpit(D9);
        DigitalOut RTDScontrol(D6);
        DigitalOut test_led(LED1);
        CAN* canBus;

        //------NNKNNK please leave comments explaining significance and functionality of member variables and functions below
        //-- we will further refactor when we draw up the functionality and ensure we dont need arbitrary wait times.
        // Optimal case: max time from message received to message sent should be: computationTime + 1ms

        bool TS_Ready = false; 
        bool Motor_On = false;
        bool CANFlag = false;
        bool RTDSqueued = false;

        EventQueue queue(64*EVENTS_EVENT_SIZE);

        uint8_t mbbAlive = 0;
        bool powerRdy = Motor_On;
        bool motorReverse = false;
        bool motorForward = true;
        int16_t torqueDemand;
        int16_t maxSpeed = MAXSPEED;

        void initIO();
        void canRX();
        void sendSync();
        void sendThrottle();
        void sendState();
        void runRTDS();
        void stopRTDS();
        void check_start_conditions();
        void cockpit_switch_high();
        void cockpit_switch_low();
        void check_switch_low();
        void implausability();
        void printStatusMessage();
        float getPedalTravel(Timer* implausability_track);
        //?
}

} //namespace