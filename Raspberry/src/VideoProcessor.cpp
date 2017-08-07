/*
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "VideoProcessor.hpp"

std::string vehicleTurnPath;

/**
 * The video processor constructor sets up all the necessary properties to use
 * the class. It also
 */
VideoProcessor::VideoProcessor(RelativePosition * relativePosition)
{
    Logger::debug("VideoProcessor Constructor");
    frameNumber = 0;

    Properties * properties = Properties::getInstance();
    targetImagePath         = properties->getStringPropertyWithName("vp_target_image_path");
    testScenceImagePath     = properties->getStringPropertyWithName("vp_test_scene_image_path");
    minHessian              = properties->getNumberPropertyWithName("vp_min_Hessian");
    sampleSize              = properties->getNumberPropertyWithName("vp_sample_size");
    vehicleTurnPath         = properties->getStringPropertyWithName("vehicle_turn_calibration");
    frameSkipping           = properties->getNumberPropertyWithName("frame_skipping");
    maxBufferSize           = properties->getNumberPropertyWithName("max_buffer_size");
    threasholdMultiplicator = properties->getNumberPropertyWithName("threashold_multiplicator");
    frameDebuggingOutput    = properties->getNumberPropertyWithName("frame_debugging_output");


    targetImage = cv::imread(targetImagePath, CV_LOAD_IMAGE_GRAYSCALE);
    //cv::Mat sceneFrame = cv::imread(testScenceImagePath, CV_LOAD_IMAGE_GRAYSCALE);;

    windowName     = properties->getStringPropertyWithName("webcam_window_name");
    webcamIdentifier= properties->getNumberPropertyWithName("webcam_device_name");
    this->relativePosition = relativePosition;

    cap = new cv::VideoCapture(webcamIdentifier);
    if(!cap->isOpened()) {
        throw DeviceNotFoundException("Webcam", std::to_string(webcamIdentifier));
    }

    //time(&timeLastFrameCaptured); // TODO: this can probably go.
    //*cap >> frame;
    getNextFrameFromCamera();
    cv::namedWindow(windowName, 1);
}

/**
 * This functions opens a camera and a window and shows the
 * camera feed in that window until the user hits the esc key.
 */
int VideoProcessor::startCapturing(void)
{
    capturing = true;

    while (capturing) {
        //usleep(2000000);
        //showNextFrame();
        processNextFrame();
        //if (cv::waitKey(10) == 27) capturing = false;
        switch (cv::waitKey(10)) {
            case 27 : capturing = false; break;
            //case
        }
    }
    return 0;
}

/**
 * Reads the next frame from the camera and presents it in a window.
 */
void VideoProcessor::showNextFrame(void)
{
    getNextFrameFromCamera();

    cv::line(frame, cv::Point2f(WEBCAM_WIDTH/2,0), cv::Point2f(WEBCAM_WIDTH/2,WEBCAM_HEIGHT), cv::Scalar(0, 255, 0), 2 );

    cv::imshow(windowName, frame);
    cv::waitKey(10);
}

/**
 * This function generates the next relative position object by processing frames as many frames as samples are needed. It then presents the frame.
 *
 * @method VideoProcessor::processNextFrame
 */
void VideoProcessor::processNextFrame()
{
    for (int i = 0; i<sampleSize; i++) {
        relativePosition->addSampleBox(processFrameUsingSURFandFLANN(getNextFrameFromCamera()));
    }

    relativePosition->processSampleBoxes();
    relativePosition->drawKeyPointsOntoCVMat(frame, cv::Point2f(0, 0));
    //relativePosition->drawKeyPointsOntoCVMat(dashboardFrame, cv::Point2f( targetImage.cols, 0));

    cv::imshow(windowName, frame);
    cv::waitKey(10);
}

/**
 * This function retruns the current frame from the camera. OpenCV Capture
 * uses a buffer which in this case is not desireble, because the next frame from
 * the buffer is not the frame that represents the sceneVector currently in front of the
 * camera but a sceneVector that is sometimes even 5 to 10 seconds behind. To solve this
 * problem, the time that OpenCV uses to get the next frame is measured. We assume that the first frame that is rad is from the buffer. The time it takes to read that frame is used as the threashold. Since
 * reading a frame directly from the camera takes significantly longer than reading
 * it from the buffer (the purpose of a buffer) we can throw away all the frames
 * that take less time than the threshold multiplied by a specified factor. If the frame takes long enough to read, we will assume that is read from the camera directly. In order to prevent infinite looping, a maximum buffer size is specified. It is assumed that the buffer is not bigger than said constant. Therefore after maxBufferSize iterations the loop can also be terminated.
 */
cv::Mat VideoProcessor::getNextFrameFromCamera(void)
{
    // The delay is necessary so the camera image is not blury because the roboterState
    // was not completely standing still yet.
    usleep(500000);

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    *cap >> frame;
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto durationThreashold = std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();

    if (frameDebuggingOutput == 1) {
        printf("Threashold              .: %2i -> %5ld microseconds\n", frameNumber, durationThreashold);
    }

    int i = 0;

    while (frameSkipping == 1 && i < maxBufferSize) {

        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
        *cap >> frame;
        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count();

        if (frameDebuggingOutput == 1) {
            printf("Time to receive frame No.: %2i -> %5ld microseconds\n", frameNumber, duration);
        }

        i++;
        if (duration > threasholdMultiplicator*durationThreashold) break;
    }

    frameNumber++;

    return frame;
}

/**
 * This method sets up surf and flann properties that only have to be calculated once.
 */
void VideoProcessor::setUpSURFandFLANN()
{
    if( !targetImage.data )  throw FileNotFoundException(targetImagePath);
    detector = cv::SurfFeatureDetector( minHessian );
    detector.detect( targetImage, targetKeypoints );
    extractor.compute( targetImage, targetKeypoints, objectDescriptors );
    if (objectDescriptors.empty()) std::cout << "object discriptor empty" << std::endl;
    surfAndFlannSetup = true;
}

/**
 * This function analyzes a frame using the surf and flann algorithm. It looks for the target, identifies it's corner points and returns an ObjectBox pointer.
 *
 * @param currentFrame the frame to be processed
 * @return              The object box pointer
 */
ObjectBox * VideoProcessor::processFrameUsingSURFandFLANN(cv::Mat currentFrame)
{
    try {

        if (!surfAndFlannSetup) {
            setUpSURFandFLANN();
        }

        cv::cvtColor(currentFrame, sceneFrame, CV_BGRA2GRAY); // currentFrame;

        if( !sceneFrame.data )   throw FileNotFoundException(" --(!) Error reading frame ");

        // Detect the keypoints using SURF Detector
        detector.detect( sceneFrame, sceneKeypoints );

        // Calculate descriptors (feature vectors)
        extractor.compute( sceneFrame, sceneKeypoints, sceneDescriptors );

        if (sceneDescriptors.empty())  std::cout << "sceneVector discriptor empty"  << std::endl;

        matches = std::vector< cv::DMatch >{};
        matcher.match( objectDescriptors, sceneDescriptors, matches );

        maxDistance = 0;
        minDistance = 100;

        // Quick calculation of max and min distances between keypoints
        for( int i = 0; i < objectDescriptors.rows; i++ ) {
            double distance = matches[i].distance;
            if( distance < minDistance ) minDistance = distance;
            if( distance > maxDistance ) maxDistance = distance;
        }

        goodMatches = std::vector< cv::DMatch >{};

        for( int i = 0; i < objectDescriptors.rows; i++ ) {
            if( matches[i].distance < 3*minDistance ) {
                goodMatches.push_back( matches[i]); }
        }

        cv::drawMatches( targetImage, targetKeypoints, sceneFrame, sceneKeypoints,
                     goodMatches, dashboardFrame, cv::Scalar::all(-1), cv::Scalar::all(-1),
                     cv::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

        // Localize the object
        targetVector   = std::vector<cv::Point2f>{};
        sceneVector = std::vector<cv::Point2f>{};

        for( int i = 0; i < goodMatches.size(); i++ )
        {
          // Get the keypoints from the good matches
          targetVector.push_back( targetKeypoints[ goodMatches[i].queryIdx ].pt );
          sceneVector.push_back( sceneKeypoints[ goodMatches[i].trainIdx ].pt );
        }

        H = findHomography( targetVector, sceneVector, CV_RANSAC );

        // Get the corners from the image_1 ( the object to be "detected" )
        std::vector<cv::Point2f> targetCorners(4);
        targetCorners[0] = cvPoint(                0,                0 );
        targetCorners[1] = cvPoint( targetImage.cols,                0 );
        targetCorners[2] = cvPoint( targetImage.cols, targetImage.rows );
        targetCorners[3] = cvPoint(                0, targetImage.rows );
        std::vector<cv::Point2f> sceneCorners(4);

        cv::perspectiveTransform( targetCorners, sceneCorners, H);

        // Draw lines between the corners (the mapped object in the sceneVector - image_2 )
        cornerPoints = {sceneCorners[0], sceneCorners[1], sceneCorners[2],sceneCorners[3]};

        return new ObjectBox(cornerPoints);
    }
    catch (Exception &e) {
        std::cout << "Exception analyizing frame.\n" << e.what() << std::endl;
    }

    cornerPoints = {cv::Point2f(0,0), cv::Point2f(0,0), cv::Point2f(0,0), cv::Point2f(0,0)};
    return new ObjectBox(cornerPoints);
}

/**
 * Returns the number of the frame that was last received from the camera.
 *
 * @return frame number
 */
int VideoProcessor::getFrameNumber()
{
    return frameNumber;
}

/* property that is needed for the mouse callback */
bool waitingForMouseEvent = true, initialClick = true;

/**
 * This function is called when a mouse event occourse in the OpenCV window.
 * It is only used for capturing training data for the vehicle callibration.
 *
 * @param event    the event itself
 * @param x        the cursor's x position within the window
 * @param y        the cursor's y position within the window
 * @param flags
 * @param userdata
 */
void callback(int event, int x, int y, int flags, void * userdata)
{
    if (event == cv::EVENT_LBUTTONDOWN) {

        if (initialClick) {
            initialClick = false;

        } else {

            waitingForMouseEvent = false;

            int distance = x - WEBCAM_WIDTH / 2;
            std::fstream outfile;
        	outfile.open(vehicleTurnPath, std::fstream::out | std::fstream::app );
            outfile << "pixel: " << distance << "\n";
    		outfile.flush();
            outfile.close();
        }
    }
}

/**
 * This function sets up necessary things for the vehicle callibration training.
 */
void VideoProcessor::startTrainingLoop()
{
    cv::setMouseCallback(windowName, callback, NULL);
    showNextFrame();

    waitForMouseEvent();
}

/**
 * This function simply waits until a mouse event occurse and blocks the rest
 * of the program until then.
 */
void VideoProcessor::waitForMouseEvent()
{
    while (waitingForMouseEvent) {
        showNextFrame();
        usleep(10000);
    }

    waitingForMouseEvent = true;
}

/**
 * This function draws the frame number to a cv::Mat.
 *
 * @param frame the cv::Mat to draw to
 */
void VideoProcessor::drawFrameNumber(cv::Mat & frame)
{
    const cv::string text = "frame: " + std::to_string(getFrameNumber());
    cv::putText(frame, text, cv::Point2f(20, 300), 1, 1.0, cv::Scalar( 0, 255, 0), 1 );
}
