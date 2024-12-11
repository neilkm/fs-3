#include "../header/ThrottleController.h"
#include "pinmap.h"
#include "PinNameAliases.h"

namespace ETC {
    ThrottleController::ThrottleController()
    {
        this->HE1 = mbed::AnalogIn(A2);
    }

    ThrottleController::~ThrottleController()
    {

    }
}