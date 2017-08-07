/*
** @author Daniel Palenicek
** @version 0.1 / 31.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "Properties.hpp"

bool Properties::instanceExists = false;
Properties * Properties::propertiesInstance = NULL;

/**
 *  The constructor if the Properties class. The consturcor parses the
 *  properties.txt file which is a configuration file. It then stores
 *  the properties in two maps depending on their rype (string or int).
 */
Properties::Properties()
{
    Logger::debug("Properties Constructor");

    std::string path = "/home/daniel/Documents/BA/Raspberry/resources/properties.txt";
    std::string line;
    std::regex rgx("([[:w:]]+) *= *(?:\"(.+)\"|([[:d:]]+)).*");
    std::smatch match;
    std::ifstream propertiesFile (path);

    if (propertiesFile.is_open()) {

        std::cout << "\nReading properties.txt file:" << std::endl;

        while ( getline(propertiesFile,line) ) {

            if (line.substr(0,1).compare("#") != 0) {

                if (std::regex_search(line, match, rgx)) {
                    //std::cout << "0: '" << match[0] << "'\n1: '" << match[1] << "'\n2: '" << match[2] << "'\n3: '"<< match[3] << "'\n\n" << std::endl;
                    if (match[2].length() > 0) { // if match[3]==0 then the line is a string property else it is a number property.
                        //std::cout << "s: " << match[1] << " : '" << match[2] << "'" << std::endl;
                        stringPropertiesMap.insert(std::pair<std::string, std::string>(match[1],match[2]));
                    }
                    else {
                        //std::cout << "d: " << match[1] << " : " << match[3] << std::endl;
                        numberPropertiesMap.insert(std::pair<std::string, int>(match[1], std::stoi(match[3])));
                    }

                }
            }
        }

        propertiesFile.close();
    }
    else {
        throw FileNotFoundException(path);
        //std::cout << "Could not open properties.txt" << std::endl;
    }
}

/**
 * Returns apointer to an instance of the Properties class. Since this class
 * follows the Singleton Design Pattern there can only be one instance of this
 * class at a time. So if an instance exist it will just return a pointer to
 * the existing instance. Otherwise it will first create a new instance.
 *
 * @return pointer to an instance of the Properies class.
 */
Properties * Properties::getInstance()
{
    if (!instanceExists) {
        propertiesInstance = new Properties();
        instanceExists = true;
    }

    return propertiesInstance;
}

/**
 * Looks for a string property from the properties.txt file.
 *
 * @param  propertyName the name of the string property to look for.
 * @return              the value of the property.
 */
std::string Properties::getStringPropertyWithName(std::string propertyName)
{
    stringPropertiesMapIterator = stringPropertiesMap.find(propertyName);

    if (stringPropertiesMapIterator == stringPropertiesMap.end()) {
        throw PropertyNotFoundException(propertyName);
    }

    return stringPropertiesMapIterator->second;
}

/**
 * Looks for a number property from the properties.txt file.
 *
 * @param  propertyName the name of the number property to look for.
 * @return              the value of the property.
 */
int Properties::getNumberPropertyWithName(std::string propertyName)
{
    numberPropertiesMapIterator = numberPropertiesMap.find(propertyName);

    if (numberPropertiesMapIterator == numberPropertiesMap.end()) {
        throw PropertyNotFoundException(propertyName);
    }

    return numberPropertiesMapIterator->second;
}

/**
 * This method reads float properties from the string properties map and returns
 * them. NOTE that float properties have to be saved as strings in the properties file.
 *
 * @param  propertyName the name of the property to look for
 * @return              the values of the property
 */
float Properties::getFloatPropertyWithName(std::string propertyName)
{
    return std::stof(getStringPropertyWithName(propertyName));
}

/**
 *  Destructor
 */
Properties::~Properties()
{
    instanceExists = false;
}
