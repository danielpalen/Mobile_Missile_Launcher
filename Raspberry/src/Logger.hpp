/*! \class Logger Logger.hpp "Logger.hpp"
** Simple class that provides a way for debug console log function. The console
** logging can be turned on and off using the preprocessor macro: DEBUG.
**
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef LOGGER_HPP
#define LOGGER_HPP

//#define DEBUG

class Logger {

public:

#ifdef DEBUG
    static void debug(std::string x) {
        std::cout << x << std::endl;
    };
#else
    static void debug(std::string x) {};
#endif



private:

};

#endif
