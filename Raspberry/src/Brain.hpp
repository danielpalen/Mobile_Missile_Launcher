/*! \class Brain Brain.hpp "Brain.hpp"
** The Brain class as the name suggests is the 'brain' of the launcher. This is
** where all the single parts are put together and a lot of the 'intelligent'
** processing takes place.
** The brain has different functions for for operting the launcher with different
** the three main stratefies: maual mode, autonomous mode and reinforcement learning mode.
**
** In manual mode the user can controll the launcher using the keyboard.
**
** -In autonomous mode the launcher tries to acheive it's goal by folling rules and.
** strategies that were hard coded.
**
** -In reinfocement learning mode the launcher tries to acheive its goal by using.
** an reinforcement learning approach.
**
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef BRAIN_HPP
#define BRAIN_HPP

//#define USING_SDLs

//#ifdef USING_SDL
#include "SDL.h"
//#endif


#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <iterator>
#include <thread>
#include <array>
#include <regex>
#include "RelativePosition.hpp"
#include "LauncherController.hpp"
#include "VehicleController.hpp"
#include "VideoProcessor.hpp"
#include "Exceptions.hpp"
#include "Logger.hpp"

class Brain {

public:
    Brain(void);
    void mainLoop(void);
    void trainingLoop();
    void stateMachineLoop();
    void startSDLControlWindow();
    void startReinforcementLearning();

private:

    const char * windowName;
    bool         keydownDetected;
    LauncherController * launcherController;
    VehicleController  * vehicleController;
    VideoProcessor     * videoProcessor;
    RelativePosition   * relativePosition;

    // MARK: Automnomous State Machine
    std::string vehicleTurnPath;

    enum roboterState {
        start,
        frameProcessed,
        noTargetDetected,
        targetDetected,
        badPosition,
        goodPosition,
        movedToNewPosition,
        end
    };

    std::string roboterStateNames[9] = {
        "start",
        "frameProcessed",
        "noTargetDetected",
        "targetDetected",
        "badPosition",
        "goodPosition",
        "movedToNewPosition",
        "end"
    };

    roboterState currentState;
    std::string  searchStrategy;
    int          positionInSearchStrategy;

    void moveRandomly();
    void searchSystematically();
    bool evaluatePositionAndImprove();
    void shootTarget();

    // MARK: SDL
//#ifdef USING_SDL
        SDL_Window * win;
        SDL_Event    e;
        int handleSDLEvent(SDL_Event e);
//#endif

    // MARK: Reiforcement Learning
    enum reinforcementState {
        rl_noTargetDetected,
        rl_badPosition,
        rl_toFarAway,
        rl_goodPosition,
        NUM_OF_RL_STATES
    };

    enum reinforcementAction {
        rl_turnLeft,
        rl_turnRight,
        rl_turnTowardTarget,
        rl_fire,
        rl_driveForward,
        rl_driveBackward,
        rl_searchSystematically,
        NUM_OF_RL_ACTIONS
    };

    std::string rl_stateNames[reinforcementState::NUM_OF_RL_STATES] = {
        "noTarget",
        "badPosition",
        "toFar",
        "goodPosition"
    };

    std::string rl_actionNames[reinforcementAction::NUM_OF_RL_ACTIONS] = {
        "left",
        "right",
        "toward",
        "fire",
        "forward",
        "backward",
        "search"
    };

    // Q function is represented by a data structure of the form [reinforcementState][reinforcementAction]
    std::array<std::array<float, reinforcementAction::NUM_OF_RL_ACTIONS>, reinforcementState::NUM_OF_RL_STATES> qValues;
    float alpha, gamma, epsilon;
    std::string qValuesPath, learningLogPath;

    void runEpisode();
    reinforcementAction choseAction(reinforcementState state);
    void takeAction(reinforcementAction action);
    float collectReward(reinforcementState state, reinforcementAction action);
    reinforcementState observeState();
    void  updateQValues(reinforcementState oldState, reinforcementState newState, reinforcementAction action, float reward);
    reinforcementAction bestActionInState(reinforcementState state);
    float maximumQValueInState(reinforcementState state);
    void readQValuesFromFile(std::string path);
    void writeTotalReward(float reward);
    void writeQValuesToFile(std::string path);
    void printQValues();

};

#endif //BRAIN_HPP
