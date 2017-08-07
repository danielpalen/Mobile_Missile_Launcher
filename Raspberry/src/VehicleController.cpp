/*
** @author Daniel Palenicek
** @version 1.0 / 24.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "VehicleController.hpp"

/**
 * The VehicleController constructor sets up the necessary properties and
 * calls the initialization function.
 */
VehicleController::VehicleController()
{
    Logger::debug("VehicleController Constructor");
    Properties * properties = Properties::getInstance();
    port           = properties->getStringPropertyWithName("vehicle_port").c_str();
    leftIntersect  = properties->getFloatPropertyWithName("vehicle_turn_left_intersect");
    leftSlope      = properties->getFloatPropertyWithName("vehicle_turn_left_slope");
    rightIntersect = properties->getFloatPropertyWithName("vehicle_turn_right_intersect");
    rightSlope     = properties->getFloatPropertyWithName("vehicle_turn_right_slope");

    init();
}

/**
 * This function initializes the vehicle controller by opening up a serial port
 * connection to the vehicle.
 */
void VehicleController::init()
{
    struct termios toptions;

    // open serial port
    fd = open(port, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        throw DeviceNotFoundException("Arduino", port);
    }

    // get current serial port settings
    tcgetattr(fd, &toptions);

    // set 9600 baud both ways
    cfsetispeed(&toptions, B9600);
    cfsetospeed(&toptions, B9600);

    // 8 bits, no parity, no stop bits
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;

    // Canonical mode
    toptions.c_lflag |= ICANON;

    // commit the serial port settings
    tcsetattr(fd, TCSANOW, &toptions);

    printf("VehicleController: Arduino connected!\n");

    // sleep for a little while so that there is enough time for the settings to be set
    sleep(2);
}

/**
 * This function receives a command and writes the corresponding
 * character to the Arduino's serial port so that the arduino can
 * execute the desired command.
 *
 * @param: the command the Arduino is supposed to exectue
 */
void VehicleController::executeCommand(VehicleController::vehicleCommand command)
{
    const char * tempCommand = "";

    switch (command) {
        case forward : tempCommand = "f"; break;
        case backward: tempCommand = "b"; break;
        case left    : tempCommand = "l"; break;
        case right   : tempCommand = "r"; break;
        case stop    : tempCommand = "s"; break;
    }

    size_t length = (size_t) strlen(tempCommand);
    ssize_t n  = write(fd, tempCommand, length);
    // printf("VehicleController: %zi bytes write : %s\n", n, tempCommand);
}

/**
* This method executes a vehicle command for a specified time before it sends
* the stop command to the vehicle.
*
* @param command: command that to be executeCommand
* @param time: the time that the command should be executed for in milliseconds
*/
void VehicleController::executeCommand(vehicleCommand command, int time)
{
    executeCommand(command);
    usleep(std::abs(time) * 1000);
    executeCommand(vehicleCommand::stop);
}

/**
 * This function executes a turn command (either left or right vehicleCommand)
 * for a specified number of pixels. It therefore translates the number of pixels
 * into milliseconds and calls the executeCommand(command, int) function.
 * To translate pixel into milliseconds it uses a linear function of the form
 * f(x) = a + bx. The parameters a and b calculated using a linear regression
 * model on the training data stored on ../resources/calibration/vehicleTurn.txt
 *
 * @param command the turn command to execute
 * @param pixel   the number of pixels
 */
void VehicleController::executeTurnPixelCommand(enum vehicleCommand turnCommand, int pixel)
{
    pixel = std::abs(pixel);

    if (turnCommand == vehicleCommand::left) {
        executeCommand(turnCommand, leftIntersect + leftSlope * pixel);
    }
    else if (turnCommand == vehicleCommand::right) {
        executeCommand(turnCommand, rightIntersect + rightSlope * pixel);
    }
    else {
        // TODO: throw exception here
    }
}

/**
 * This function closes the connection to the Arduino.
 */
void VehicleController::closeArduino()
{
    close(fd);
    printf("VehicleController: Arduino released!");
}
