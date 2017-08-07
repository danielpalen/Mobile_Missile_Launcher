/*! \class LauncherController LauncherController.hpp "LauncherController.hpp"
**
** This class defines functions to controll the launcher. It does this by writing
** codes to the serial USB connection that the launcher is connected to. The launcher
** listens to these codes and starts executing the desired action.
**
** @author Daniel Palenicek
** @version 1.0 / 26.08.2015
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef LAUNCHERCONTROLLER_HPP
#define LAUNCHERCONTROLLER_HPP

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <libusb-1.0/libusb.h>

#include "Exceptions.hpp"
#include "Logger.hpp"

/** Launcher vendor id macro to find the launcher. */
#define VENDOR_ID       0xA81
/** Launcher product id macro to find the launcher. */
#define PRODUCT_ID      0x701
/** Launcher macro that specifies the time the launcher waits when the fireing command is executed */
#define FIRE_DELAY      8000 //5300

/** Launcher controll sequence macro */
#define DEVICE_UP       "\x02\x00\x00\x00\x00\x00\x00\x00"
/** Launcher controll sequence macro */
#define DEVICE_DOWN     "\x01\x00\x00\x00\x00\x00\x00\x00"
/** Launcher controll sequence macro */
#define DEVICE_LEFT     "\x04\x00\x00\x00\x00\x00\x00\x00"
/** Launcher controll sequence macro */
#define DEVICE_RIGHT    "\x08\x00\x00\x00\x00\x00\x00\x00"
/** Launcher controll sequence macro */
#define DEVICE_FIRE     "\x10\x00\x00\x00\x00\x00\x00\x00"
/** Launcher controll sequence macro */
#define DEVICE_STOP     "\x20\x00\x00\x00\x00\x00\x00\x00"

class LauncherController {

public:
    enum launcherCommand{
      up, down, left, right, fire, stop
    };

    LauncherController();
    void executeCommand(enum launcherCommand command);
    void executeCommand(enum launcherCommand command, int time);

private:
    libusb_context * context;
    libusb_device * device;
    libusb_device_handle * launcher;
    static const char * commandHex[];

    void init(void);
};

#endif /* LAUNCHERCONTROLLER_HPP */
