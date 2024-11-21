//#include "mbed.h"
#include "TaskQueue.h"

TaskQueue eventQueue(64);

int main()
{
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