/*! \class ObjectBox ObjectBox.hpp "ObjectBox.hpp"
**
** The ObjectBox class represents some object that the VideoProcessor
** detected. It is the ObjectBoxe's task to figure out whether what was
** detected might actually be the target object ot migth just be wrong.
** Furthermore it is used to build a new ObjectBox from a number of
** sample ObjectBoxes. This means that the VideoProcessor records a number
** of frames, analyzes and makes ObjectBoxes for all of them and then
** passes an array of all the ObjectBoxes to the ObjectBox constructor to
** generate one new ObjectBox from all the samples. The idea behind this is
** to minimize and possibly eliminate mistakes that were made during the
** object detection by the VideoProcessor. This is acceived by throwing away
** samples that are obviously wrong and then mixing all the other plausible
** samples.
**
** @author Daniel Palenicek
** @version 0.1 / 23.09.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#ifndef OBJECTBOX_HPP
#define OBJECTBOX_HPP

#include <string>
#include "opencv2/features2d/features2d.hpp"
#include "VideoProcessor.hpp"
#include "Logger.hpp"

class ObjectBox {

public:

    // MARK: Constructors
    ObjectBox() {};
    ObjectBox(std::vector<ObjectBox> sampleObjectBoxes);
    ObjectBox(std::array<cv::Point2f, 4> objectCornerPoints);

    // MARK: Getter
    std::array<cv::Point2f, 4> getObjectCornerPoints();
    bool        isSample();
    bool        isRelevant();

    // MARK: Object property calculations
    bool        objectDetected();
    bool        cameraCenterIntersectsTargetHorizontaly();
    bool        cameraCenterIntersectsTargetVerticaly();
    float       getRelativeObjectArea();
    cv::Point2f getObjectCenter();
    cv::Point2f distanceOfObjectToCameraCenter();

    // MARK: Filters
    bool filter();

    // MARK: Drawing functions
    void drawBorders(cv::Mat &frame, cv::Point2f origin);
    void drawCorners(cv::Mat &frame, cv::Point2f origin, cv::Scalar borderColor, bool helpingLines);
    void drawCenterOffset(cv::Mat &frame, cv::Point2f origin, cv::Scalar borderColor);

private:
    /* points in coordinate space.
    axis   x→→
        a ##### b
    y   #       #
    ↓   #       #
    ↓   d ##### c
    */

    cv::Point2f a;
    cv::Point2f b;
    cv::Point2f c;
    cv::Point2f d;
    cv::Point2f cameraCenter;
    cv::Point2f objectCenter;
    cv::Scalar  white = cv::Scalar( 255,  255, 255);
    cv::Scalar  blue  = cv::Scalar( 255,    0,   0);
    cv::Scalar  green = cv::Scalar(   0, 255,    0);
    cv::Scalar  red   = cv::Scalar(   0,    0, 255);
    float       screenArea;

    bool sample;
    bool relevant;
    static int boxCounter;
    static bool debug;
    int boxID;

    void init();
    void printObjectBox();
};

#endif //OBJECTBOX_HPP
