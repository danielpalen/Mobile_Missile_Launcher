FROM ubuntu:16.04

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y  wget \
                        unzip \
                        build-essential \
                        g++ \
                        cmake \
                        libusb-1.0-0-dev \
                        libsdl2-dev \
                        # for OpenCV
                        libjpeg8-dev \
                        libtiff5-dev \
                        libjasper-dev \
                        libpng12-dev \
                        libgtk2.0-dev \
                        libavcodec-dev \
                        libavformat-dev \
                        libswscale-dev \
                        libv4l-dev \
                        libatlas-base-dev \
                        gfortran

RUN wget -O opencv-2.4.9.zip http://sourceforge.net/projects/opencvlibrary/files/opencv-unix/2.4.9/opencv-2.4.9.zip/download && \
    unzip opencv-2.4.9.zip && \
    cd    opencv-2.4.9 && \
    mkdir build && \
    cd build && \
    cmake   -D CMAKE_BUILD_TYPE=RELEASE \
            -D CMAKE_INSTALL_PREFIX=/usr/local \
            -D WITH_OPENGL=ON .. && \
    make && \
    make install && \
    ldconfig && \
    cd ../.. && \
    rm -r opencv-2.4.9 && \
    rm opencv-2.4.9.zip

ADD . /home/daniel/Documents/BA/Raspberry



RUN cd /home/daniel/Documents/BA/Raspberry && \
    rm -r build && \
    mkdir build && \
    cd build && \
    cmake .. && \
    make

WORKDIR /home/daniel/Documents/BA/Raspberry/build

ENV PATH /home/daniel/Documents/BA/Raspberry/build:$PATH

# $docker build -t launcher .
# $docker save launcher > ./Docker/docker-launcher-image.tar
# $docker load < ./Docker/docker-launcher-image.tar
# $docker run -t -i --privileged -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix launcher
