/*
** @author Daniel Palenicek
** @version 1.0 / 26.08.2015
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "LauncherController.hpp"

/**
 *  This table should be accessed via the launcherCommand enum.
 *  It holds the commands that are written to the Launchers USB-port
 *  to controll it.
 */
const char * LauncherController::commandHex[] = {
    DEVICE_UP,
    DEVICE_DOWN,
    DEVICE_LEFT,
    DEVICE_RIGHT,
    DEVICE_FIRE,
    DEVICE_STOP
};

/**
 * The LauncherController constructor makes sure that the launcher
 * is completely initialized before other functions can be called on it.
 * Not initializing it before calling executeCommand would lead to
 * segmentation faults.
 */
// TODO: maybe make this singleton
LauncherController::LauncherController()
{
    Logger::debug("Launcher Constructor");
    this->init();
}

/**
 * This function initializes the launcher. It first opens the launcher, then
 * detaches it from kernel drivers (takes it away from the operating system)
 * and then claims it. Claiming it is important so we can send instructions
 * to it.
 *
 * @return  the result of the initialization.
 *          0 for success and -1 for failure.
 */
void LauncherController::init()
{
    // open and claim the launcher
    context = NULL;
    libusb_init(&context);
    // libusb_set_debug(context, 255);
    launcher = libusb_open_device_with_vid_pid(context, VENDOR_ID, PRODUCT_ID);

    if (launcher == NULL) throw DeviceNotFoundException("Launcher. It is necessary to run the program as sudo");

    std::cout << "Launcher opened" << std::endl;

    libusb_set_auto_detach_kernel_driver(launcher, 1);

    if (libusb_claim_interface(launcher, 0) < 0) throw ClaimLauncherException();
}

/**
 * This function makes the launcher execute a command by writing it
 * to the lancher via the usb port.
 *
 * @param command the action that the launcher should execute
 */
void LauncherController::executeCommand(enum launcherCommand command)
{
    unsigned char buf[65535];
    const char * tempCommand = commandHex[(int) command];

    std::memcpy(buf, tempCommand, 0x8);
    libusb_control_transfer(launcher, LIBUSB_REQUEST_TYPE_CLASS +  LIBUSB_RECIPIENT_INTERFACE, 0x9, 0x200, 0x0, buf, 0x8, 1000);

    if (command == fire) {
        usleep(FIRE_DELAY*1000);
        /* For some reason the stop command does not work after the fire command
           this is why we first have to call a different command and then the
           stop command in order for the launcher to stop shooting. The left
           command was chosen for no special purpose other than being different
           from the fire and the stop command.
        */
        executeCommand(left);
        executeCommand(stop);
    }

}

/**
* This method executes a launcher command for a specified time before it sends
* the stop command to the launcher.
*
* @param command: command that to be executed
* @param time: the time that the command should be executed for in milliseconds
*/
void LauncherController::executeCommand(launcherCommand command, int time)
{
    executeCommand(command);
    usleep(time * 1000);
    executeCommand(launcherCommand::stop);
}
