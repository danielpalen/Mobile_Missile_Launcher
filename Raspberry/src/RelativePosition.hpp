/*! \class RelativePosition RelativePosition.hpp "RelativePosition.hpp"
**
** The RelativePosition class saves the four corner points of the object in the
** form of it's ObjectBox property in the relative to the camera's corrdinate
** system and provices helper functions to determain the distane, angle etc. to
** the object. This is needed in other classes to make decisions on how to move
** the robot to get into a better position.
**
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef RELATIVEPOSITION_HPP
#define RELATIVEPOSITION_HPP

#include <array>
#include <stdio.h>
#include <iostream>
#include "opencv2/features2d/features2d.hpp"
#include "VideoProcessor.hpp"
#include "ObjectBox.hpp"
#include "Logger.hpp"

class ObjectBox;

class RelativePosition {

public:

    //enum side {streightAhead, right, left, none};

    // MARK: Constructors
    RelativePosition();
    //RelativePosition(std::array<cv::Point2f, 4> objectCornerPoints);

    // MARK: Getter & Setter
    float       getRelativeObjectArea();
    cv::Point2f getObjectCenter();
    void        addSampleBox(ObjectBox * newSample);
    void        processSampleBoxes();

    // MARK: Other functions
    bool objectDetected();
    bool cameraCenterIntersectsTarget();
    bool cameraCenterIntersectsTargetHorizontaly();
    bool cameraCenterIntersectsTargetVerticaly();
    cv::Point2f distanceOfObjectToCameraCenter();

    // MARK: Drawing functions
    void  drawKeyPointsOntoCVMat(cv::Mat &frame, cv::Point2f origin);
    void  drawText(cv::Mat &frame, cv::Point2f origin);

    // MARK: Destructor
    ~RelativePosition() {};

private:
    // MARK: PRIVATE
    std::vector<ObjectBox> sampleObjectBoxes;
    ObjectBox * objectBox;
    cv::Point2f cameraCenter;
    float       screenArea;

    void  clearSampleBoxes();

    // MARK: colors
    cv::Scalar green = cv::Scalar(0, 255, 0);
    cv::Scalar red   = cv::Scalar(0, 0, 128);
    std::vector<cv::Scalar> colors = {cv::Scalar(255, 0, 0), cv::Scalar(255, 255, 0), cv::Scalar(255, 255, 255), cv::Scalar(255, 0, 255)};

};

#endif //RELATIVEPOSITION_HPP
