/*! \class VehicleController VehicleController.hpp "VehicleController.hpp"
**
** The VehicleController class opens up a serial port Connection to the
** Arduino and provides functions to execute different commands on the
** Arduino and to close the connection to the Arduino.
**
** @author Daniel Palenicek
** @version 1.0 / 24.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef VEHICLECONTROLLER_HPP
#define VEHICLECONTROLLER_HPP

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "Properties.hpp"
#include "Logger.hpp"

class VehicleController {

public:

    enum vehicleCommand {
        forward,
        backward,
        left,
        right,
        stop
    };

    VehicleController(void);
    void executeCommand(enum vehicleCommand command);
    void executeCommand(enum vehicleCommand command, int time);
    void executeTurnPixelCommand(enum vehicleCommand turnCommand, int pixel);
    void closeArduino();

private:

    int          fd = -1;
    const char * port; // The port's identifier that the Arduino is connected to.
    float leftIntersect, leftSlope;
    float rightIntersect, rightSlope;

    void init();
};

#endif //VEHICLECONTROLLER_HPP
