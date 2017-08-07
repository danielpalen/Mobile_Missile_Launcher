# Missile Launcher

The Missile Launcher is the result of my bachelor thesis "Reinforcement Learning for Mobile Missile Launching". The Missile Launcher software combines all the hardware parts described below and has the goal to autonomously find and shoot a specified target. Therefore it uses image processing, serial port communication and other algorithms.

The Missile Launcher is an integrated system consisting off
- a host computer
- a USB missile launcher
- an Arduino and Arduino Motorshield
- and an 4WD Arduino Mobile Platform

***

## Features
The Missile Launcher can run in three different modes:

#### Autonomous Mode
When running in Autonomous Mode the Missile Launcher tries to find and shoot a specified target. In this mode the actions the Missile Launcher uses a state machine and all the actions are all hard coded.

#### Manual Mode
In manual mode the Missile Launcher does not act on itself but rather is directly controlled by keyboard inputs from the user.

#### Reinforcement Learning Mode
In Reinforcement Learning Mode the Missile Launcher tries to find and shoot a specified target using reinforcement learning.

***

## Install and Run
For running the Missile Launcher you have two options:
1. Build from source
2. Running a docker container.

### Building from Source
To build the Missile Launcher from source you have to install the dependencies locally on your system and then build and run the Missile Launcher yourself.
~~~ bash
mkdir build
cd build
cmake ..
sudo ./Launcher --help
~~~

### Using Docker
#### Import an existing launcher image
If a docker image is provided as a \*.tar file you can import the image with the following command:
~~~ bash
docker load < [Path to the docker *.tar file]
~~~

#### Build your own Image
If you want to build your own image you have to run the following command. Depending on the system this can take from 15m up to several hours.
~~~ bash
docker build -t launcher [Path to Dockerfile]
~~~

#### Running the Container
To run the container you first have to enable the docker container to open up X11 windows on the host system. Then you can run the container using the following command. The --privileged flag lets the container run in privileged mode. This lets the container access the devices that are connected to the host system.

> NOTE: Docker on Raspberry Pi requires a docker image that was specifically build on a Raspberry Pi and uses Raspbian as it's base image. This is because of the Raspberry Pi's ARM processor architecture.

~~~ bash
xhost local:docker
docker run -ti --privileged -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix launcher
~~~

***

## Dependencies
The Missile Launcher currently has the following dependencies:
- [libusb-1.0](https://sourceforge.net/projects/libusb/?source=directory)
- [OpenCV 2.4.9](https://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.9/)
- [SDL2](https://www.libsdl.org/download-2.0.php) ( SDL is only needed for the keyboard input so when compiling without manual mode SDL should not be needed. Preprocessor macro to turn off SDL might come ).
