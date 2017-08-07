/*!
** The main.cpp file hosts the main method and also the usage method which provides
** insigts on how to use the executable.
**
** @author Daniel Palenicek
** @version 1.0 / 05.10.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include <unistd.h>
#include "VehicleController.hpp"
#include "LauncherController.hpp"
#include "VideoProcessor.hpp"
#include "Brain.hpp"
#include "Properties.hpp"
#include "Exceptions.hpp"
#include "RelativePosition.hpp"

/**
 * This function prints information about the usage of the launcher executable
 * to the console.
 *
 * @param argc number of arguments.
 * @param argv array that contains the arguments.
 */
void usage(int argc, char *argv[]) {
    std::cout
    << "Usage: Launcher { -a | -m | -rf }\n\n"
    << "\n"
    << "Note: Most operations require to be run in super user mode.\n"
    << "      So in case there are any exceptions during the start\n"
    << "      up phase run 'sudo " << argv[0] << "  { -a | -m | -rf }'.\n"
    << "\n"
    << "Options:\n"
    << "-a,  --autonomous     \tRobot will search the target in autonomous mode.\n"
    << "-m,  --manual         \tRobot will be controllable using the keyboard.\n"
    << "-r,  --reinforcement  \tRobot will seach the target using reinforcement learning.\n"
    << "-h,  --help           \tDisplay this message and exit.\n"
    << std::endl;
}

int main(int argc, char *argv[]) {

    try {
    /* Vehicle Test


        VehicleController * vehicleController = new VehicleController();
        vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::right, 540);
*/

    /* Launcher Test

        LauncherController * lc = new LauncherController();
        lc->executeCommand(LauncherController::launcherCommand::fire);
        usleep(2000000);
        lc->executeCommand(LauncherController::launcherCommand::stop);
        */


    /* VideoProcessor Test
        VideoProcessor * vp = new VideoProcessor();
        vp->startCapturing();
    */

    /* Brain Test */

/*
        for (int i = 0; i < argc; i++) {
            std::cout << i << ":" << argv[i] << std::endl;
        }
        */

        if (argc == 2) {
            if      (std::string(argv[1]) == "-a"   || std::string(argv[1]) == "--autonomous") {
                Brain * brain = new Brain();
                brain->stateMachineLoop();
            }
            else if (std::string(argv[1]) == "-m"   || std::string(argv[1]) == "--manual") {
                Brain * brain = new Brain();
                brain->startSDLControlWindow();
            }
            else if (std::string(argv[1]) == "-r"  || std::string(argv[1]) == "--reinforcement") {
                Brain * brain = new Brain();
                brain->startReinforcementLearning();
            }
            else if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
                usage(argc, argv);
                exit(0);
            } else {
                std::cout << "Not a valid call for " << argv[0] << ". Run '" << argv[0] << " --help' for info about the usage." << std::endl;
            }
        } else {
            usage(argc, argv);
        }
        exit(0);

/*
        else if (std::string(argv[1]) == "1") {
            brain->mainLoop();
        }
//#ifdef USING_SDL
        else if (std::string(argv[1]) == "2") {
            brain->startSDLControlWindow();
        }
//#endif
        else if (std::string(argv[1]) == "3") {
            brain->trainingLoop();
        }
        */

    /* Properties Test
        Properties * properties;
        properties = Properties::getInstance();
        std::cout << properties->getStringPropertyWithName("vehicle_port") << std::endl;
        std::cout << properties->getNumberPropertyWithName("webcam_device_name") << std::endl;
    */

    /* Exception Test

        //throw FileNotFoundException("test.txt");
        //throw PropertyNotFoundException("test_property");
        throw DeviceNotFoundException("TestDevice", "test.txt");
    */

   /* RelativePosition Test
        RelativePosition rel = RelativePosition();
        */

  }
  catch (Exception &e) {
      // std::cout << "Exception Handler reached" << std::endl;
      std::cout << e.message() << std::endl;
      exit(EXIT_FAILURE);
  }
}
