/*
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "Brain.hpp"

/**
 * The constructor just sets up the class variables.
 */
Brain::Brain(void)
{
	Logger::debug("Brain Constructor");

	Properties * properties = Properties::getInstance();
	windowName 		= properties->getStringPropertyWithName("sdl_window_name").c_str();

	// Autonomous mode properties
	vehicleTurnPath = properties->getStringPropertyWithName("vehicle_turn_calibration");
	searchStrategy  = properties->getStringPropertyWithName("robot_search_strategy");
	positionInSearchStrategy = 0;

	// Reinforcement Learning properties
	alpha 			= properties->getFloatPropertyWithName("rl_alpha");
	gamma 			= properties->getFloatPropertyWithName("rl_gamma");
	epsilon 		= properties->getFloatPropertyWithName("rl_epsilon");
	qValuesPath 	= properties->getStringPropertyWithName("rl_qValues_path");
	learningLogPath = properties->getStringPropertyWithName("rl_learningLog_path");

	launcherController = new LauncherController();
	vehicleController  = new VehicleController();
	relativePosition   = new RelativePosition();
	videoProcessor     = new VideoProcessor(relativePosition);

	currentState = Brain::roboterState::start;
}

/**
 * This function starts the video processing. It is mainly used for testing now.
 */
void Brain::mainLoop(void)
{
	videoProcessor->startTrainingLoop();

	while (true) {
		videoProcessor->processNextFrame();
	}
}

/**
 * This function is used to collect training data for callibrating the robot's turn left and turn right action functions.
 */
void Brain::trainingLoop()
{
	//std::fstream outfile;
	//outfile.open("../resources/test.txt", std::fstream::in | std::fstream::out | std::fstream::app );
	std::fstream outfile;
	outfile.open(vehicleTurnPath, std::fstream::out | std::fstream::app );
	outfile.flush();
	outfile.close();

	videoProcessor->startTrainingLoop();

	while (true) {

		int time = rand()%200 + 1;

		int direction = rand()%2;
		switch (direction) {
			case 0: vehicleController->executeCommand(VehicleController::vehicleCommand::left, time); break;
			case 1: vehicleController->executeCommand(VehicleController::vehicleCommand::right, time);break;
		}

		std::string directionString = direction == 1 ? "right" : "left ";
    	outfile.open(vehicleTurnPath, std::fstream::out | std::fstream::app );
		outfile << directionString << " time: " << time << " ";
		outfile.flush();
		outfile.close();

		videoProcessor->waitForMouseEvent();
		//usleep(1000000);
	}

}

/**
 * This function implements a state machine that contains every state that is
 * necessacary during the process of the robot finding and shooting the target.
 * Depending on the current state different actions are taken and depending on
 * their outcome the current state changes.
 * TODO: For a visual representation of the state machine see the documentation.
 */
void Brain::stateMachineLoop()
{
	while (true) {

		std::cout << "CURRENT STATE: \t" << roboterStateNames[currentState] << std::endl;

		switch (currentState) {

			case start:
				videoProcessor->processNextFrame();
				currentState = Brain::roboterState::frameProcessed;
				break;

			case frameProcessed:
				if (relativePosition->objectDetected())
					currentState = Brain::roboterState::targetDetected;
				else
				 	currentState = Brain::roboterState::noTargetDetected;
				break;

				case noTargetDetected:
					searchSystematically();
					currentState = Brain::roboterState::movedToNewPosition;
					break;

				case targetDetected:
					if (evaluatePositionAndImprove()) {
						currentState = Brain::roboterState::goodPosition;
					} else {
						currentState = Brain::roboterState::badPosition;
					}
					break;

					case badPosition:
						currentState = Brain::roboterState::movedToNewPosition;
						break;

					case goodPosition:
						shootTarget();
						currentState = Brain::roboterState::end;
						break;

			case movedToNewPosition:
				videoProcessor->processNextFrame();
				currentState = Brain::roboterState::frameProcessed;
				break;

			case end:
				return;
		}
	}
}


/**
 * This function makes the robot execute a random move in search for the target.
 * It is invoced, when the robot does not detect the target and therefore hast
 * to change its current position in hope of tetecting the target afterwards.
 */
void Brain::moveRandomly()
{
	// TODO

	switch (rand()%1) {
		//case 0: launcherController->executeCommand(LauncherController::launcherCommand::left); break;
		//case 1: launcherController->executeCommand(LauncherController::launcherCommand::right); break;
		case 0: vehicleController->executeCommand(VehicleController::vehicleCommand::left, 250); break;
		case 1: vehicleController->executeCommand(VehicleController::vehicleCommand::right,250); break;
		case 2: launcherController->executeCommand(LauncherController::launcherCommand::up); break;
		case 3: launcherController->executeCommand(LauncherController::launcherCommand::down); break;
	}

	//launcherController->executeCommand(LauncherController::launcherCommand::stop);
	//vehicleController->executeCommand(VehicleController::vehicleCommand::stop, 100);
}

/**
 * This function makes the robot follow a systematic search path that is defined in the properties file.
 * Each time this function is called the next action from the search path will be exectued. So calling this
 * function multiple will have different results.
 */
void Brain::searchSystematically()
{
	char temp = searchStrategy[positionInSearchStrategy];
	std::cout << temp << std::endl;

	if 		(temp == 'f') {
		vehicleController->executeCommand(VehicleController::vehicleCommand::forward,  700);
	}
	else if (temp == 'b') {
		vehicleController->executeCommand(VehicleController::vehicleCommand::backward, 700);
	}
	else if (temp == 'l') {
		vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::left, 500);
	}
	else if (temp == 'r') {
		vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::right, 540);
	}

	positionInSearchStrategy = (positionInSearchStrategy + 1) % searchStrategy.length();
}

/**
 * This function evaluates the robots current position towards the target object.
 * For doing this it uses the relativePosition object. Depending on whether the
 * position is good or bad it trys to improve the position towards the target.
 *
 * @return whether the current position was good (true) or bad (false) before it
 *         was improved.
 */
bool Brain::evaluatePositionAndImprove()
{
	/* The position is good if the target can be shot. */
	bool positionIsGood = true;

	// is the target in the center so that the projectile will hit?
	if ( !relativePosition->cameraCenterIntersectsTargetHorizontaly() ) {

		cv::Point2f distance = relativePosition->distanceOfObjectToCameraCenter();

		if ( distance.x < 0 ) {
			std::cout << distance.x << " pixels to the left" << std::endl;
			vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::left, distance.x);
		}
		else {
			std::cout << distance.x << " pixels to the right" << std::endl;
			vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::right, distance.x);

			//vehicleController->executeCommand(VehicleController::vehicleCommand::right, 125);
		}
		positionIsGood = false;
	}

	// Is the target close enough?
	float objectArea = relativePosition->getRelativeObjectArea();

	if (objectArea < 0.15 /* TODO: just some random threashold*/) {
		int distance = 250/(objectArea*6);
		printf("objectArea to small: %f. Moving %d forward.\n", objectArea, distance);
		vehicleController->executeCommand(VehicleController::vehicleCommand::forward, distance);
		positionIsGood = false;
	}

	return positionIsGood;
}

/**
 * This function represents an action from the state machine. It simply envoces
 * the fireing function of the launcher.
 */
void Brain::shootTarget()
{
	std::cout << "SHOOTing at target!" << std::endl;
	vehicleController->executeCommand(VehicleController::vehicleCommand::stop);
	launcherController->executeCommand(LauncherController::launcherCommand::fire);
}


//#ifdef USING_SDL
/**
 * This function is mainly for testing the controll functions of the roboter.
 * It opens up a window that listens to key events that then get translated to
 * certain launcher and vehicle commands that then are executed.
 *
 * The vehicle can be controlled via the arrow keys whereas the launcher can be
 * controller usting the key: w,a,s,d and the space bar.
 *
 */
void Brain::startSDLControlWindow()
{
	//First we need to start up SDL, and make sure it went ok
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return;
	}


	//Now create a window with title "Hello World" at 100, 100 on the screen with w:640 h:480 and show it
	SDL_Window *win = SDL_CreateWindow("contrl window", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	SDL_Event e, lastEvent;
	keydownDetected = false;

	std::thread videoCapturingThread(&VideoProcessor::startCapturing, videoProcessor);

	while(true) {
		if (SDL_PollEvent(&e) && handleSDLEvent(e) != 0) break;
	}

	videoCapturingThread.join();

	SDL_DestroyWindow(win);
	SDL_Quit();
}

/**
 * This function decides what actions to take for SDL_Events
 * @param  e the SDL_Event it gets passed
 * @return
 */
int Brain::handleSDLEvent(SDL_Event e)
{
	if (e.type == SDL_KEYDOWN && !keydownDetected) {
		keydownDetected = true;
		std::cout << "KEY_DOWN detected" << std::endl;
		switch (e.key.keysym.sym) {

			/* Launcher Cases */
			case SDLK_w:
			launcherController->executeCommand(LauncherController::launcherCommand::up);
			break;

			case SDLK_s:
			launcherController->executeCommand(LauncherController::launcherCommand::down);
			break;

			case SDLK_a:
			launcherController->executeCommand(LauncherController::launcherCommand::left);
			break;

			case SDLK_d:
			launcherController->executeCommand(LauncherController::launcherCommand::right);
			break;

			case SDLK_SPACE:
			launcherController->executeCommand(LauncherController::launcherCommand::fire);
			break;

			/* Vehicle Cases */
			case SDLK_UP:
			vehicleController->executeCommand(VehicleController::vehicleCommand::forward);
			break;

			case SDLK_DOWN:
			vehicleController->executeCommand(VehicleController::vehicleCommand::backward);
			break;

			case SDLK_LEFT:
			vehicleController->executeCommand(VehicleController::vehicleCommand::left);
			break;

			case SDLK_RIGHT:
			vehicleController->executeCommand(VehicleController::vehicleCommand::right);
			break;

			/* Other Cases */
			case SDLK_ESCAPE:
			return -1;
		}
	}
	else if (e.type == SDL_KEYUP) {
		std::cout << "KEY_UP detected" << std::endl;
		launcherController->executeCommand(LauncherController::launcherCommand::stop);
		vehicleController->executeCommand(VehicleController::vehicleCommand::stop);
		keydownDetected = false;
	}
	else if (e.type == SDL_QUIT) {
		return -1;
	}

	return 0;
}
//#endif

// MARK: Reinforcement Learning

/**
 * Starts a reinforcement learning episode.
 */
void Brain::startReinforcementLearning()
{
	std::cout << "starting reinforcement learning\n" << std::endl;
	runEpisode();
}

/**
 * This function represents one episode.
 */
void Brain::runEpisode()
{
	float totalReward = 0, reward = 0;
	reinforcementAction action;
	reinforcementState  state, newState;
	readQValuesFromFile(qValuesPath);

	videoProcessor->processNextFrame();
	state = observeState();

	while (true) {

		action = choseAction(state);
		takeAction(action);
		videoProcessor->processNextFrame();

		reward = collectReward(state, action);
		totalReward += reward;
		newState = observeState();
		std::cout 	<< " [state " 	 << std::setw(12)<<rl_stateNames[state]   	<< "]"
		 			<< " [action " 	 << std::setw(8)<< rl_actionNames[action] 	<< "]"
					<< " [reward " 	 << std::setw(3)<< reward  					<< "]"
					<< " [n_ state " << std::setw(12)<<rl_stateNames[newState]	<< "]"
					<< " [t_reward " << std::setw(3)<< totalReward             	<< "]\n"
					<< std::endl;

		updateQValues(state, newState, action, reward);


		state = newState;

		if (action == reinforcementAction::rl_fire || totalReward <= -30) break;
	}

	writeQValuesToFile(qValuesPath);
	writeTotalReward(totalReward);
}

/**
 * This function chooses one of the available actions. There are two possible ways
 * how the action can be chosen: greediely or randomly. Which one is used is also
 * random.
 * The greedy option is the default option (it has a way higher probability to be
 * chosen). The qValues represent how good the actions are in the specified state.
 * Therefore the action with the highest qValue for the current state is going to
 * be the greedy option.
 *
 * epsilon% of the time the action will be chosen randomly though.
 *
 * @param  state is the state for which an action is to be chosen.
 * @return       action that was chosen.
 */
Brain::reinforcementAction Brain::choseAction(reinforcementState state)
{
	int chooseRandomAction = rand()%100;

	if (chooseRandomAction < epsilon*100) {
		std::cout << "choosing random" << std::endl;
		return static_cast<reinforcementAction>(rand()%reinforcementAction::NUM_OF_RL_ACTIONS);
	} else {
		std::cout << "choosing greedy" << std::endl;
		return bestActionInState(state);
	}
}

/**
 * This function exectures an action that it is passed. It therefore translates
 * the action enum it reseives into the right executing fuction from the right
 * controller instance.
 *
 * @param action to execute
 */
void Brain::takeAction(reinforcementAction action)
{
	switch (action) {

		case rl_turnLeft:
		vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::left,  200);
		break;

		case rl_turnRight:
		vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::right, 200);
		break;

		case rl_turnTowardTarget:
		{
			if (relativePosition->objectDetected()) {
				cv::Point2f distance = relativePosition->distanceOfObjectToCameraCenter();
				if ( distance.x < 0 ) {
					std::cout << distance.x << " pixels to the left" << std::endl;
					vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::left, distance.x);
				}
				else {
					std::cout << distance.x << " pixels to the right" << std::endl;
					vehicleController->executeTurnPixelCommand(VehicleController::vehicleCommand::right, distance.x);
				}
			}
		}
		break;

		case rl_driveForward:
		vehicleController->executeCommand(VehicleController::vehicleCommand::forward,  700);
		break;

		case rl_driveBackward:
		vehicleController->executeCommand(VehicleController::vehicleCommand::backward, 700);
		break;

		case rl_searchSystematically:
		searchSystematically();
		break;

		case rl_fire:
		launcherController->executeCommand(LauncherController::launcherCommand::fire);
		break;
	}
}

/**
 * This function represents the reward function and returns the appropriote
 * reward for a specific action that was taken in a specific state.
 *
 * @param  state  the state the robot was in when taking the action.
 * @param  action the action taken in the specified state.
 * @return        the reward the robot receives for taking action a in state s.
 */
float Brain::collectReward(reinforcementState state, reinforcementAction action)
{
	float result = 0;

	switch (state) {

		case rl_noTargetDetected:
		switch (action) {
		case rl_turnLeft:  		result =  -3; break;
	        case rl_turnRight: 		result =  -3; break;
		case rl_turnTowardTarget: 	result =  -5; break;
	        case rl_driveForward: 		result =  -2; break;
	        case rl_driveBackward: 		result =  -2; break;
		case rl_searchSystematically: 	result =  -1; break;
	        case rl_fire: 			result = -20; break;
		}
		break;

		case rl_badPosition:
		switch (action) {
		case rl_turnLeft:  		result =  -1; break;
	        case rl_turnRight: 		result =  -1; break;
		case rl_turnTowardTarget:	result =   3; break;
	        case rl_driveForward: 		result =   0; break;
	        case rl_driveBackward: 		result =   0; break;
		case rl_searchSystematically: 	result =  -1; break;
	        case rl_fire: 			result = -20; break;
		}
		break;

		case rl_toFarAway:
		switch (action) {
		case rl_turnLeft:  		result =  -3; break;
	        case rl_turnRight: 		result =  -3; break;
		case rl_turnTowardTarget: 	result =  -1; break;
	        case rl_driveForward: 		result =   3; break;
	        case rl_driveBackward: 		result =  -2; break;
		case rl_searchSystematically: 	result =  -3; break;
	        case rl_fire: 			result =  -1; break;
		}
		break;

		case rl_goodPosition:
		switch (action) {
		case rl_turnLeft:  		result =  -3; break;
	        case rl_turnRight: 		result =  -3; break;
		case rl_turnTowardTarget: 	result =  -1; break;
	        case rl_driveForward: 		result =  -1; break;
	        case rl_driveBackward: 		result =  -2; break;
		case rl_searchSystematically: 	result =  -3; break;
	        case rl_fire: 			result =  10; break;
		}
		break;
	}

	return result;
}

/**
 * The function determains the current state. It does this by taking different
 * circumstances into account like whether the target object was detected in the
 * scene and if how big it is and how far it shifted from the camera's center.
 *
 * @return the state that robot is currently assumed to be in.
 */
Brain::reinforcementState Brain::observeState()
{

	if (relativePosition->objectDetected()) {

		// is the target in the center so that the projectile will hit?
		if ( !relativePosition->cameraCenterIntersectsTargetHorizontaly() ) {

			return reinforcementState::rl_badPosition;
		}
		if (relativePosition->getRelativeObjectArea() < 0.15 /* TODO: just some random threashold*/) {

			return reinforcementState::rl_toFarAway;
		} else {

			return reinforcementState::rl_goodPosition;
		}
	} else {
		return reinforcementState::rl_noTargetDetected;
	}
}

/**
 * This function updated the qValues. It does this by calculating the new qValues
 * using a formular that is specified by the Q-Learning algorithm:
 *
 * Q(s,a) = Q(s,a) + α(r + γ * max(a')Q(s',a') - Q(s,a))
 *
 * This function is called during every iteration within an episode after a reward
 * was earned.
 *
 * @param  oldState the robot was in before it took the action.
 * @param  newState the robot is in after taking the action.
 * @param  action   the action that transformed the old state into the new state.
 * @param  reward   the reward that the robot earned for taking the action in the old state.
 */
void  Brain::updateQValues(reinforcementState oldState, reinforcementState newState, reinforcementAction action, float reward)
{
	qValues[oldState][action] = qValues[oldState][action] + alpha * (reward + gamma * maximumQValueInState(newState) - qValues[oldState][action]);
}

/**
 * This function is a helper function that returns the maximumQValue for a
 * specified state.
 *
 * @param  state The state that should be examined.
 * @return       The maximum qValue for the specified state.
 */
float Brain::maximumQValueInState(reinforcementState state)
{
	return qValues[state][bestActionInState(state)];
}

/**
 * This function analyzes which action is the best to take when the robot is in
 * a specific state. It assumes that 'best' means the gready option so this will
 * return the action that has the highes qValues for the specified state.
 *
 * This function is a helper function and is used by updateQValues()
 * and maximumQValueInState().
 *
 * @param  state The state for which the possible actions should be examinded.
 * @return       The action that has the best qValue in the specified state.
 */
Brain::reinforcementAction Brain::bestActionInState(reinforcementState state)
{
	int action = 0;
	float temp = qValues[state][action];
	reinforcementAction result = static_cast<reinforcementAction>(action);

	for (action++; action < reinforcementAction::NUM_OF_RL_ACTIONS; action++) {

		if (temp < qValues[state][action]) {
			temp   = qValues[state][action];
			result = static_cast<reinforcementAction>(action);
		}
	}

	return result;
}

/**
 * This function reads the qValues from a setup file from the filesystem. This is
 * so that values already learned can be reused after episodes.
 * It uses a regulat expression to parse the file.
 *
 * @param path The input files path.
 */
void Brain::readQValuesFromFile(std::string path)
{
	std::ifstream qValuesFile (path);

	if (qValuesFile.is_open()) {

		std::string line;
		std::string regexString = "(?:.*[[:s:]]+)?";
		std::string floatMatcher = "([-+]?[0-9]*[.]?[0-9]+)";

		for (int action=0; action < reinforcementAction::NUM_OF_RL_ACTIONS - 1; action ++) {
			regexString += floatMatcher + "[[:s:]]+";
		}

		regexString += floatMatcher + "[[:s:]]*";
		std::regex rgx(regexString);
	    std::smatch match;
		int rowForReinforcementLearningState = 0;
		qValues = std::array<std::array<float, reinforcementAction::NUM_OF_RL_ACTIONS>, reinforcementState::NUM_OF_RL_STATES>();

		while (getline(qValuesFile, line)) {

			if (std::regex_search(line, match, rgx)) {

				std::array<float, reinforcementAction::NUM_OF_RL_ACTIONS> temp;

				for (int action=0; action < reinforcementAction::NUM_OF_RL_ACTIONS; action++) {

					temp.at(action) = std::stof(match[action+1]);
				}

				qValues.at(rowForReinforcementLearningState++) = temp;
			}
		}


		qValuesFile.close();

		std::cout << "read qValues from file: " << qValuesPath << std::endl;
		printQValues();

	}
	else {
		throw FileNotFoundException(qValuesPath);
	}
}

/**
 * This function will wrote the total reward that was earned during an episode to a file. The file will be appended and not overwritten.
 *
 * @param reward the total reward
 */
void Brain::writeTotalReward(float reward)
{
	std::ofstream outfile;

 	outfile.open("../resources/training/totalReward.txt", std::ios_base::app);
 	outfile << reward << ",";

	outfile.flush();
	outfile.close();
}


/**
 * This function writes the qValues to a file to preserve them bevor the program quits.
 * It saves them in a human readably format into a .txt file. The content is
 * written in form of a table with the columns representing all the actions and
 * the rows representing the different states.
 * By saving qValues values already learned can be reused in the future by calling the
 * readQValuesFromFile() function.
 *
 * @param path The path of the file to write to. This will usually be that sdl_window_name
 *             file path as the one used for reading the qValues in the beginning.
 */
void Brain::writeQValuesToFile(std::string path)
{

	std::fstream qValuesFile;
	qValuesFile.open(path, std::fstream::out );

	for (int state=-1; state < reinforcementState::NUM_OF_RL_STATES; state++) {

		if (state == -1) {
			qValuesFile << std::setw(14) << "state/action";
		} else {
			qValuesFile <<  std::setw(13) << rl_stateNames[state] << ":";
		}

		for (int action=0; action < reinforcementAction::NUM_OF_RL_ACTIONS; action++) {
			int width = 12;
			if (state == -1) {
				qValuesFile << std::setw(width) << rl_actionNames[action];
			} else {
				qValuesFile << std::setw(width) << qValues[state][action];
			}
		}
		qValuesFile << std::endl;
	}

	qValuesFile.flush();
	qValuesFile.close();

	std::cout << "updated qValues written to file: " << qValuesPath << std::endl;
	printQValues();
}

/**
 * This function simply prints the qValues to the console as they currently are
 * during the execution of the program.
 * The output is structured as a nicely readable table of actions as rows and
 * states as collumns.
 */
void Brain::printQValues()
{
	for (int state=-1; state < reinforcementState::NUM_OF_RL_STATES; state++) {

		if (state == -1) {
			std::cout << std::setw(14) << "state/action";
		} else {
			std::cout <<  std::setw(13) << rl_stateNames[state] << ":";
		}

		for (int action=0; action < reinforcementAction::NUM_OF_RL_ACTIONS; action++) {

			int width = 12;

			if (state == -1) {
				std::cout << std::setw(width) << rl_actionNames[action];
			} else {
				std::cout << std::setw(width) << qValues[state][action];
			}
		}

		std::cout << std::endl;
	}
}
