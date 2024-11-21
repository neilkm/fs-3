#include "ThrottleController.h"

//NNK where does throttleMessage come from?
//NNK where does CANMessage come from?
//NNK stateMessage?
//NNK cockpit?
//NNK canBus?
//NNK mbbAlive?
//NNK RTDControl?

//NNK bring all magic numbers to the top.. set variables at the top with pins.
//NNK all consts and io pins should be set at top of file so they are easy to replace and change

namespace ETC {

ThrottleController::ThrottleController()
{
        this->HE1 = AnalogIn(A1);
}

ThrottleController::~ThrottleController()
{

}

void ThrottleController::initIO() {
    debugLevelPrint(1,"initIO\n");
    // printf("initIO\n");
    canBus = new CAN(CAN_RX_PIN, CAN_TX_PIN, CAN_FREQ);

    //ARL change to polling system, queue adds unecessary overhead
    queue.call_every(20ms, &sendThrottle);
    queue.call_every(100ms, &sendSync);
    queue.call_every(100ms, &sendState); //NNK Here is your issue! you are sending sendSync and sendState at the same time

    canBus->attach(canRX);
    
    debugLevelPrint(1,"Waiting for start conditions!\n");
    // printf("Waiting for start conditions!\n");

    Cockpit.rise(&cockpit_switch_high);

    // Event for closing motor
    Cockpit.fall(&cockpit_switch_low);
}

void ThrottleController::canRX() {
        CANFlag = true;
}

void ThrottleController::sendSync() {
        unsigned char data[0];
        CANMessage syncMessage(0x80, data, 0);
        canBus->write(syncMessage);
}

void ThrottleController::sendThrottle() {
        mbbAlive++;
        mbbAlive %= 16;

        CANMessage throttleMessage;
        throttleMessage.id = 0x186;

        throttleMessage.data[0] = torqueDemand;
        throttleMessage.data[1] = torqueDemand >> 8;
        throttleMessage.data[2] = maxSpeed;
        throttleMessage.data[3] = maxSpeed >> 8;
        throttleMessage.data[4] = 0x00 | (0x01 & motorForward) | ((0x01 & motorReverse) << 1) | ((0x01 & powerRdy) << 3);
        throttleMessage.data[5] = 0x00 | (0x0f & mbbAlive);
        throttleMessage.data[6] = 0x00;
        throttleMessage.data[7] = 0x00;

        canBus->write(throttleMessage);
}

void ThrottleController::sendState() {
        // We calculate in a long-winded fashion for debug purposes
        float HE1_read = HE1.read();
        float HE2_read = HE2.read();
        debugLevelPrint(1,std::string("HE1: %f  | HE2: %f\n", HE1_read, HE2_read));
        // printf("HE1: %f  | HE2: %f\n", HE1_read, HE2_read);

        float clamped_HE1 = clamp(HE1_read, HE1_LOW, HE1_HIGH);
        float clamped_HE2 = clamp(HE2_read, HE2_LOW, HE2_HIGH);
        float HE1_travel = (clamped_HE1 - HE1_LOW) / (HE1_HIGH - HE1_LOW);
        float HE2_travel = (clamped_HE2 - HE2_LOW) / (HE2_HIGH - HE2_LOW);

        float pedal_travel = 0.5 * (HE1_travel + HE2_travel); // take the avg of the two pedal travels

        CANMessage stateMessage;
        stateMessage.id = 0x1A1;

        stateMessage.data[0] = 0x00 | ((TS_Ready) | (Motor_On << 1) | (CANFlag << 2) | (RTDSqueued << 3) | (Cockpit.read() << 4));
        stateMessage.data[1] = (int8_t)(brakes.read() * 100);
        stateMessage.data[2] = (int8_t)(HE1_read * 100);
        stateMessage.data[3] = (int8_t)(HE2_read * 100);
        stateMessage.data[4] = (int8_t)(HE1_travel * 100);
        stateMessage.data[5] = (int8_t)(HE2_travel * 100);
        stateMessage.data[6] = (int8_t)(pedal_travel * 100);
        stateMessage.data[7] = 0x00;

        canBus->write(stateMessage);
}

void ThrottleController::runRTDS() {
        debugLevelPrint(1,"RUNNING RTDS\n");
        // printf("RUNNING RTDS\n");
        // test_led.write(true);
        RTDScontrol.write(true);
        queue.call_in(1000ms, &stopRTDS); //NNK Why are we waiting here?
}

void ThrottleController::stopRTDS() {
        // test_led.write(false);
        RTDScontrol.write(false);
        debugLevelPrint(1,"FINISHED RTDS\n");
        // printf("FINISHED RTDS\n");
        RTDSqueued = false;
}

void ThrottleController::check_start_conditions() {
        if (Cockpit.read() == 0)
        {
            return;
        }
        if (TS_Ready /* && brakes.read() >= BRAKE_TOL*/)
        {
            Motor_On = true;
            if (RTDSqueued)
            {
                return;
            }
            queue.call(&runRTDS); //NNK why not runRTSD() by itself?
            RTDSqueued = true;
        }
}

void ThrottleController::cockpit_switch_high() {
        queue.call_in(10ms, &check_start_conditions); //NNK why waiting 10ms?
}

void ThrottleController::cockpit_switch_low() {
        queue.call_in(10ms, &check_switch_low); //NNK why waiting 10ms?
}

void ThrottleController::check_switch_low() {
        if (Cockpit.read() == 1)
        {
            return;
        }
        Motor_On = false; //NNK a lot of these functions seem unneccessary, lets try to merge these smaller functions together
}

void ThrottleController::implausability() {
        // printf("implausability occured");
        Motor_On = false;
}

void ThrottleController::printStatusMessage() {
        int ts_rdy = TS_Ready ? 1 : 0;
        int m_on = Motor_On ? 1 : 0;
        int cockpit = Cockpit.read();
        float b = brakes.read();
        float HE1_read = HE1.read();
        float HE2_read = HE2.read(); 

        //NNK instead of commenting out prints, we can add global debug variables of different levels to turn on or off debug output
        debugLevelPrint(1,std::string("Cockpit Switch: %i | TS_RDY: %i | Brakes: %f | Motor_On: %i | HE1: %f | HE2: %f\n", cockpit, ts_rdy, b, m_on, HE1_read, HE2_read));
        // printf("Cockpit Switch: %i | TS_RDY: %i | Brakes: %f | Motor_On: %i | HE1: %f | HE2: %f\n", cockpit, ts_rdy, b, m_on, HE1_read, HE2_read);
}

float ThrottleController::getPedalTravel(Timer *implausability_track) {
        // We calculate in a long-winded fashion for debug purposes
        float HE1_read = HE1.read();
        float HE2_read = HE2.read();
        debugLevelPrint(1,std::string("HE1: %f  | HE2: %f\n", HE1_read, HE2_read));
        // printf("HE1: %f  | HE2: %f\n", HE1_read, HE2_read);

        float clamped_HE1 = clamp(HE1_read, HE1_LOW, HE1_HIGH);
        float clamped_HE2 = clamp(HE2_read, HE2_LOW, HE2_HIGH);
        float HE1_travel = (clamped_HE1 - HE1_LOW) / (HE1_HIGH - HE1_LOW);
        float HE2_travel = (clamped_HE2 - HE2_LOW) / (HE2_HIGH - HE2_LOW); //NNK comments explaining logic or include logic in documentation
        debugLevelPrint(1,"HE1_travel: %f  | HE2_travel: %f\n", HE1_travel, HE2_travel);
        // printf("HE1_travel: %f  | HE2_travel: %f\n", HE1_travel, HE2_travel);

        // implausibility if greater than 10% pedal travel diff for more than 100 ms.
        float pedal_travel = 0.5 * (HE1_travel + HE2_travel); // take the avg of the two pedal travels
        debugLevelPrint(1,std::string("Pedal Travel: %f\n", pedal_travel));
        // printf("Pedal Travel: %f\n", pedal_travel);
        float travel_diff = std::abs(HE1_travel - HE2_travel);

        if (travel_diff > .1f)
        {
            if (implausability_track->elapsed_time() > 100ms) //NNK explain magic number
            {
                implausability();
            }
            else
            {
                implausability_track->start();
            }
        }
        else
        {
            implausability_track->stop();
            implausability_track->reset();
        }

        if (HE1_read == 0 || HE2_read == 0 || HE1_read >= 0.9 || HE2_read >= 0.9) //NNK explain magic numbers
        {
            debugLevelPrint(1,"implausability\n");
            // printf("implausability\n");
            implausability();
        }

        return pedal_travel;
}

} // namespace