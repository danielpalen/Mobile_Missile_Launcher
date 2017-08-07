/*
** This Arduino program for the Arduino Uno R3 reads control signals from its serial port
** and translates them into actions that are sent to the Arduino Motor Shield R3.
**
** The motorshield controlls a "dfrobot 4WD Arduino Mobile Platform":
** http://www.robotshop.com/en/dfrobot-4wd-arduino-mobile-platform.html [23.08.2016]
**
** @author: Daniel Palenicek
** @version 1.0 - 22.08.2016
*/

// action macros for the transmitted actions
#define FORWARD 'f'
#define BACKWARD 'b'
#define RIGHT 'r'
#define LEFT 'l'
#define STOP 's'

// macros for controlling the motors
#define DIRECTION_FORWARD LOW
#define DIRECTION_BACKWARD HIGH
#define BRAKE_ENGAGE HIGH
#define BRAKE_DISENGAGE LOW
#define SPEED_HIGH 255
#define SPEED_MID 125
#define SPEED_LOW 100

// macros for motor A (right motor)
#define LEFT_DIRECTION 12
#define LEFT_BRAKE 9
#define LEFT_SPEED 3

// macros for motor B (left motor)
#define RIGHT_DIRECTION 13
#define RIGHT_BRAKE 8
#define RIGHT_SPEED 11


/*
** This function is run once in the beginning and sets upp
** all things necessary.
*/
void setup() {

    // setting up the serial port communication at 9600
    Serial.begin(9600);

    // setting up the arduino pins as outputs
    pinMode(LEFT_DIRECTION, OUTPUT);
    pinMode(LEFT_BRAKE,     OUTPUT);
    pinMode(RIGHT_DIRECTION, OUTPUT);
    pinMode(RIGHT_BRAKE,     OUTPUT);
}

/*
** This function is repeated indefinetely. It reads available control signals
** from the serial port and translated them to motor actions.
**
** Arduino Serial Port API reference: https://www.arduino.cc/en/Reference/Serial
*/
void loop() {

    if (Serial.available() > 0){

        // read the next charcter from the serial bus
        char c=Serial.read();

        if(c==FORWARD){
            controllMotor(LEFT, DIRECTION_FORWARD, BRAKE_DISENGAGE, SPEED_HIGH);
            controllMotor(RIGHT, DIRECTION_FORWARD, BRAKE_DISENGAGE, SPEED_HIGH);
        }
        else if(c==BACKWARD){
            controllMotor(RIGHT, DIRECTION_BACKWARD, BRAKE_DISENGAGE, SPEED_HIGH);
            controllMotor(LEFT, DIRECTION_BACKWARD, BRAKE_DISENGAGE, SPEED_HIGH);
        }
        else if(c==LEFT){
            controllMotor(RIGHT, DIRECTION_FORWARD, BRAKE_DISENGAGE, SPEED_HIGH);
            controllMotor(LEFT, DIRECTION_BACKWARD, BRAKE_DISENGAGE, SPEED_HIGH);
        }
        else if(c==RIGHT){
            controllMotor(LEFT, DIRECTION_FORWARD, BRAKE_DISENGAGE, SPEED_HIGH);
            controllMotor(RIGHT, DIRECTION_BACKWARD, BRAKE_DISENGAGE, SPEED_HIGH);
        }
        else if (c==STOP) {
            stopMotors();
        }
        else {
            // character read from the serial port was not recognized as an action.
            // Serial.print("Warning: Character not recognized");
        }
    }
}

/*
** Controll the specified motor. The exact action the motor should
** take can be controlled via the paramaters.
**
** @param the motor to be controlled
** @param the direction the motor should turn
** @param engage or disengage the motors break
** @param the speed at whoch the motos should turn (maximum speed = SPEED_HIGH = 255)
*/
void controllMotor(char motor, int dir, int brake, int speed) {
    if (motor==LEFT){
        digitalWrite(LEFT_DIRECTION, dir); //Establishes direction of Channel A
        digitalWrite(LEFT_BRAKE, brake);   //controlls the Brake for Channel A
        analogWrite(LEFT_SPEED, speed);    //Spins the motor on Channel A at specified speed
    }
    else if (motor==RIGHT){
        digitalWrite(RIGHT_DIRECTION, dir); //Establishes direction of Channel A
        digitalWrite(RIGHT_BRAKE, brake);   //controlls the Brake for Channel A
        analogWrite(RIGHT_SPEED, speed);    //Spins the motor on Channel A at specified speed
    }
}

/*
** stops both motors
*/
void stopMotors() {
    stopMotor(LEFT);
    stopMotor(RIGHT);
}

/*
** Stops the motor
** @param the motor to be stopped
*/
void stopMotor(char motor) {
    if (motor==LEFT) {
        digitalWrite(LEFT_BRAKE, HIGH);
    }
    else if (motor==RIGHT) {
        digitalWrite(RIGHT_BRAKE, HIGH);
    }
}
