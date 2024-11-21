//#include "mbed.h"
#include "TaskQueue.h"
#include "ThrottleController.h"

TaskQueue eventQueue(64);

int main()
{
    // Setup main threadcontroller
    ETC::ThrottleController controller

    int stt = 0; // steps till tick
    while(1)
    {
        // rest of event loop above
        if(stt == 10)
        {
            eventQueue.handle_tick_event();
            stt = 0; // reset steps till tick
        }
        else
        {
            stt++;
        }

        // sleep for 1ms, smallest step
        //ThisThread::sleep_for(1ms);
    }

    eventQueue.clean();
    return 0;
}

//NNK Below is the old main function from fs-2 please leave comments
// how super loop is run and what functions get called and what variables are getting updated

/*
int main()
{
    // printf("main\n");
    // Initiate CAN Bus 
    initIO();
    // Begin waiting for start conditions

    // forward decl. for timer tracking 100ms implausabilities for rules
    Timer implausability_timer;

    while(1) {
        // printf("loop\n");
        if(CANFlag) {
            CANMessage msg;
            CANFlag = false;
            
            if(canBus->read(msg)) {
                uint32_t id = msg.id;
                unsigned char* data = msg.data;

                switch (id) {
                    case 0x183:
                        if (data[3] & 0x4) {
                            // printf("TS RDY rx");
                            TS_Ready = true;
                        } else {
                            TS_Ready = false;
                        }
                        break;
                    default:
                        break;
                }
            }
        }

        if (Motor_On) {
            // testing implausability
            // for some reason it is not dispatching send events
            float pedalTravel = getPedalTravel(&implausability_timer);
            // THROTTLE (uint16 torquedemand (%of0xffff), uint16 maxspeed (rpm), 
            // bool powerrdy, bool reverse = False, bool forward = True, 
            // uint8 nbbalive (increment and reset when max)) 
            // send 20 ms freq

            //Sync message  (ID is 0x80) (Msg is 0x00 100 ms freq)

            powerRdy = true;
            motorReverse = false;
            motorForward = true;
            torqueDemand = int16_t(MAX_TORQUE * pedalTravel); // Dunno if it should be between 0 and 1 or 0 and 100
            maxSpeed = MAXSPEED;
        } else {
            powerRdy = false;
            motorReverse = false;
            motorForward = true;
            torqueDemand = 0; 
            maxSpeed = MAXSPEED;
            // printStatusMessage();
        }

        // might be dispatching throttle, sync, and state
        queue.dispatch_once();
        ThisThread::sleep_for(1ms);
    }

}*/