/*! \class VideoProcessor VideoProcessor.hpp "VideoProcessor.hpp"
**
** This class handles everything that has to do with the launchers camera from
** reading the frame from the camera to analyzing the frame and looking for the
** target object in the scene.
** The analysis is done by using SURF and FLANN but can easily be changed by
** swapping out the the processFrameUsingSURFandFLANN() function.
**
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef VIDEOPROCESSOR_HPP
#define VIDEOPROCESSOR_HPP

#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <time.h>
#include <chrono>
#include <string>
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <opencv2/nonfree/features2d.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "Properties.hpp"
#include "Exceptions.hpp"
#include "RelativePosition.hpp"
#include "ObjectBox.hpp"
#include "Logger.hpp"

// webcam specifics
#define WEBCAM_WIDTH 640
#define WEBCAM_HEIGHT 480
#define WEBCAM_DEVNAME 1

class RelativePosition; // Forward Declaration of RelativePosition.

class VideoProcessor {

public:

    VideoProcessor(RelativePosition * relativePosition);
    int  startCapturing(void);
    int  stopCapturing(void);
    void showNextFrame(void);
    void processNextFrame();
    int  getFrameNumber(void);
    void startTrainingLoop();
    void waitForMouseEvent();

private:

    std::string targetImagePath;
    std::string testScenceImagePath;
    std::string windowName;
    //time_t      timeLastFrameCaptured;
    bool    capturing;
    int     webcamIdentifier, minHessian, frameNumber, sampleSize;
    double  maxDistance, minDistance;
    cv::Mat frame, targetImage, sceneFrame, dashboardFrame;
    cv::VideoCapture *  cap;
    RelativePosition *  relativePosition;

    int frameSkipping ,maxBufferSize, threasholdMultiplicator, frameDebuggingOutput;


    // SURF and FLANN properties
    bool surfAndFlannSetup = false;
    cv::Mat objectDescriptors, sceneDescriptors, H;
    cv::SurfFeatureDetector     detector;
    std::vector<cv::KeyPoint>   targetKeypoints, sceneKeypoints;
    cv::SurfDescriptorExtractor extractor;
    cv::FlannBasedMatcher       matcher;
    std::vector<cv::DMatch>     matches, goodMatches;
    std::vector<cv::Point2f>    targetVector, sceneVector, targetCorners;
    std::array<cv::Point2f, 4>  cornerPoints;

    cv::Mat     getNextFrameFromCamera(void);
    void        setUpSURFandFLANN();
    ObjectBox * processFrameUsingSURFandFLANN(cv::Mat currentFrame);
    void        drawFrameNumber(cv::Mat & frame);
};

#endif //VIDEOPROCESSOR_HPP
