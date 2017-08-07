/*! \class Properties Properties.hpp "Properties.hpp"
**
** The Properties class is used to manage configuration properties accross the
** entire project. It is designed using the Singleton Design Pattern to ensure
** that there can only be one instance of the class at a time. This makes sense
** because that way we only have to parse the properties.txt file once and not
** everytime another class wants to ask for its properties.
**
** @author Daniel Palenicek
** @version 0.1 / 31.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef PRPOPERTIES_HPP
#define PRPOPERTIES_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <regex>
#include "Exceptions.hpp"
#include "Logger.hpp"

class Properties {

public:

    static Properties *getInstance();
    std::string getStringPropertyWithName(std::string propertyName);
    int         getNumberPropertyWithName(std::string propertyName);
    float       getFloatPropertyWithName(std::string propertyName);
    ~Properties();

private:

    static bool instanceExists;
    static Properties *propertiesInstance;
    std::map<std::string, std::string>           stringPropertiesMap;
    std::map<std::string, std::string>::iterator stringPropertiesMapIterator;
    std::map<std::string, int>                   numberPropertiesMap;
    std::map<std::string, int>::iterator         numberPropertiesMapIterator;

    Properties();
};

#endif //PRPOPERTIES_HPP
