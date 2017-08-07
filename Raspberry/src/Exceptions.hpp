/*! \class Exception Exception.hpp "Exception.hpp"
** The Exception file defines multiple custom exception that are used throughout
** the entire project
**
** @author Daniel Palenicek
** @version 0.1 / 31.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/


#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP

#include <iostream>
#include <exception>
#include <string>

/**
 * base exception
 */
struct Exception : public std::exception
{
    virtual ~Exception() {};

    const char * what() const throw () {
        return name.c_str();
    }

    virtual std::string message() const throw () = 0;

protected:
    std::string path;
    std::string name = "Exception";
};


/**
 * This exception is thrown when a file that was supposed to be there was not found.
 */
struct FileNotFoundException : public Exception
{
    FileNotFoundException(std::string path) {
        this->path = path;
        name = "FileNotFoundException";
    }

    const char * what() const throw () {
        return path.c_str();
    }

    std::string message() const throw () {
        return name + ": File not found: " + path;
    }
};

/**
 * This exception is thrown when a property that is not present in the properties file is tried to be read.
 */
struct PropertyNotFoundException : public Exception
{
    PropertyNotFoundException(std::string path) {
        this->path = path;
        name = "PropertyNotFoundException";
    }

    std::string message() const throw () {
        return name + ": Property not found: " + path;
    }
};

/**
 * This exception is thrown when a device that should be connected is not found while starting the program. These devices can be The Camera, the Launcher or the Arduino.
 */
struct DeviceNotFoundException : public Exception
{
    DeviceNotFoundException() {};

    DeviceNotFoundException(std::string device) {
        name = "DeviceNotFoundException";
        text = name + ": " + device;
    }

    DeviceNotFoundException(std::string device, std::string path) {
        name = "DeviceNotFoundException";
        text = name + ": " + device + " not found at: " + path;;
    }

    std::string message() const throw () {
        return text;
    }

    private:
        std::string text;
};

/**
 * This exception is thrown when the launcher cannot be claimed while the program is being set up.
 */
struct ClaimLauncherException : public Exception
{
    ClaimLauncherException() {}

    std::string message() const throw () {
        return "Cannot claim launcher. Usually this is because the program was not run with sudo.";
    }
};

#endif //EXCEPTIONS_HPP
