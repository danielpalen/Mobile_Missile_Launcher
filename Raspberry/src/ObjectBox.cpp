/*
** @author Daniel Palenicek
** @version 0.1 / 23.09.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "ObjectBox.hpp"

int  ObjectBox::boxCounter = 0;
bool ObjectBox::debug      = false;

// MARK: Constructors

/**
 * This constructor constructs a new ObjectBox object from an array of sample
 * ObjectBoxes. The sample ObjectBoxes are a number of ObjectBoxes calculated
 * from different frames from the camera that are combined to one ObjectBox.
 * This approach wants to make the object detection more accurate by minimizing
 * the imact of mistakes that were made during the object recognition.
 */
ObjectBox::ObjectBox(std::vector<ObjectBox> sampleObjectBoxes)
{
    Logger::debug("ObjectBox Constructor 1");

    this->sample   = false;

    cv::Point2f a = cv::Point2f(0,0);
    cv::Point2f b = cv::Point2f(0,0);
    cv::Point2f c = cv::Point2f(0,0);
    cv::Point2f d = cv::Point2f(0,0);

    float relevantBoxes = 0;

    for (int i=0; i<sampleObjectBoxes.size(); i++) {

        std::array<cv::Point2f, 4> temp = sampleObjectBoxes[i].getObjectCornerPoints();

        // if is relevant Box. i.e. object was detected and it is a plausible box.
        if (sampleObjectBoxes[i].isRelevant()) {

            relevantBoxes += 1;
            a += temp[0];
            b += temp[1];
            c += temp[2];
            d += temp[3];
        }

        sampleObjectBoxes[i].printObjectBox();
    }

    if (relevantBoxes > 0) {
        a = cv::Point2f(a.x/relevantBoxes, a.y/relevantBoxes);
        b = cv::Point2f(b.x/relevantBoxes, b.y/relevantBoxes);
        c = cv::Point2f(c.x/relevantBoxes, c.y/relevantBoxes);
        d = cv::Point2f(d.x/relevantBoxes, d.y/relevantBoxes);
    }

    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;
    init();
    this->relevant = filter();


    this->printObjectBox();
}

/**
 * This constructor just makes a simple ObjectBox object using the object's four
 * corner points that the video frame analysis found.
 */
ObjectBox::ObjectBox(std::array<cv::Point2f, 4> objectCornerPoints)
{
    Logger::debug("ObjectBox Constructor 2");
    a = objectCornerPoints[0];
    b = objectCornerPoints[1];
    c = objectCornerPoints[2];
    d = objectCornerPoints[3];


    this->sample      = true;
    init();
    this->relevant    = filter();

}


// MARK: Getter

/**
 * This method returns the objectBox's corner points in the form of an array.
 *
 * @return an array of the four corner points.
 */
std::array<cv::Point2f, 4> ObjectBox::getObjectCornerPoints()
{
    return std::array<cv::Point2f, 4>{a,b,c,d};
}

/**
 * Simple getter function for the sample value.
 * @method ObjectBox::isSample
 * @return the ObjectBox's sample value.
 */
bool ObjectBox::isSample() { return sample; }

/**
 * Simple getter function for the relevant value.
 * A ObjectBox sample is only relevant if it is suspected that it represents
 * a detected object. This is determained via the filter functions.
 *
 * @method ObjectBox::isRelevant
 * @return relevant
 */
bool ObjectBox::isRelevant() { return relevant; }

/**
 * Getter for the objectCenter
 *
 * @method ObjectBox::getObjectCenter
 * @return the value of objectCenter
 */
cv::Point2f ObjectBox::getObjectCenter() { return objectCenter; }


// MARK: Object property calculations.

/**
 * This method returns whether this ObjectBox represents a detected object or not.
 * It therefore checks different properties of the objext box. Those could for
 * example be if the area the objext box represents is big enough or if the
 * corner points even form a square etc.
 *
 * @method ObjectBox::objectDetected
 * @return boolean if the object was detected.
 */
bool ObjectBox::objectDetected()
{
    return relevant;
}

/**
 * Calculates whether the ObjectBox (minus a certain threashold) intersects the
 * cameraCenter on the horizontal axis. This is important information to figure
 * out whether the roboter's projectiles would hit the target if fired.
 *
 * @return bool of whether the intersection is given or not.
 */
bool ObjectBox::cameraCenterIntersectsTargetHorizontaly()
{
    // TODO: threashold configuration.
    float area = getRelativeObjectArea();
    int threashold;

    if (area < 0.04) {
        threashold = -10;
    }
    else if  (area < 0.08) {
        threashold = 0;
    }
    else if  (area < 0.15) {
        threashold = 50;
    }
    else {
        threashold = 80;
    }

    return std::max(a.x, d.x) + threashold < cameraCenter.x && cameraCenter.x < std::min(b.x, c.x) - threashold;
}

/**
 * Calculates whether the ObjectBox (minus a certain threashold) intersects the
 * cameraCenter on the vertical axis. This is important information to figure
 * out whether the roboter's projectiles would hit the target if fired.
 *
 * @return bool of whether the intersection is given or not.
 */
bool ObjectBox::cameraCenterIntersectsTargetVerticaly()
{
    return std::max(a.y, b.y) < cameraCenter.y && cameraCenter.y < std::min(d.y, c.y);
}

/**
 * Represents the area the ObjectBox takes up in relation to the entire cameras
 * area. This can be used to verify if the object was even detected, because
 * the camera is not capable of detecting the object when it is to small. It
 * can also be used to estimate the camera's distance form the object.
 *
 * @method ObjectBox::getRelativeObjectArea
 * @return percentage of ObjectBox's area in relation to the screenArea.
 */
float ObjectBox::getRelativeObjectArea()
{
    // TODO: easy estimation for now.
    return (b.x - a.x) * std::min((d.y - a.y),(c.y-b.y)) / screenArea;
}

/**
 * Calculates the distance of the ObjectBox's center to the distance of the cameraCenter
 *
 * @method ObjectBox::distanceOfObjectToCameraCenter
 * @return distance as cv::Point2f
 */
cv::Point2f ObjectBox::distanceOfObjectToCameraCenter()
{
    return cv::Point2f(objectCenter.x - WEBCAM_WIDTH/2, objectCenter.y - WEBCAM_HEIGHT/2);
}

// MARK: Filters

/**
 * This function seperates the ObjectBoxes that actually detected the the target
 * object from the ones that are just garbage. Since the VideoProcessors processNextFrame()
 * function always returns an ObjectBox object it is the filter() functions duty
 * to seperate the meaningful ObjectBoxes from the garbage.
 *
 * @method ObjectBox::filter
 * @return does the ObjectBox really represent the target obejct in the scene?
 */
bool ObjectBox::filter()
{
    bool result = true;

    char area = '+', rot = '+', relH = '+', relV = '+';
    // is the ObjectBoxes area big enough?
    if (!(getRelativeObjectArea() > 0.01)) {
      area = '-';
      result = false;
    }

    // Check if the objects orientation is right.
    if (!(a.x < b.x && d.x < c.x && a.y < d.y && b.y < c.y))  {
      rot = '-';
      result = false;
    }

    if (!(((b.x - a.x) < 2*(c.x - d.x)) && ((c.x - d.x) < 2*(b.x - a.x))))  {
      relH = '-';
      result = false;
    }

    if (!(((d.y - a.y) < 2*(c.y - b.y)) && ((c.y - b.y) < 2*(d.y - a.y))))  {
      relV = '-';
      result = false;
    }

    if (isSample() && debug) printf("  filter %4d area:%c rotation:%c rel_H:%c rel_V:%c\n", boxID, area, rot, relH, relV);
    //printObjectBox();

    return result;
}


// MARK: Drawing functions

/**
 * Draws the ObjectBoxe's borders onto a frame.
 *
 * @method ObjectBox::drawBorders
 * @param  frame       to draw to
 * @param  origin      of the corridnate system within the frame.
 * @param  borderColor to draw in.
 */
void ObjectBox::drawBorders(cv::Mat &frame, cv::Point2f origin)
{
    cv::Scalar borderColor = sample ? (relevant ? blue : red) : green;
    float thickness = 1.5;
    cv::line( frame, a + origin, b + origin, borderColor, thickness );
    cv::line( frame, b + origin, c + origin, borderColor, thickness );
    cv::line( frame, c + origin, d + origin, borderColor, thickness );
    cv::line( frame, d + origin, a + origin, borderColor, thickness );

    //if (sample) {
    //    cv::putText(frame, std::to_string(boxID), a + origin, 1, 1, borderColor, 1 );
    //}
}

/**
 * Draws the ObjectBoxe's corners onto a frame. It also draws labels for the corners
 * and it optionaly draws vertical and horizontal heler lines.
 *
 * @method ObjectBox::drawCorners
 * @param  frame        to draw to.
 * @param  origin       of the coordinate system within the frame.
 * @param  borderColor  color to draw in.
 * @param  helpingLines should they be drawin ot not?
 */
void ObjectBox::drawCorners(cv::Mat &frame, cv::Point2f origin, cv::Scalar borderColor, bool helpingLines)
{

    cv::Scalar green = cv::Scalar(0, 255, 0), red = cv::Scalar(0, 0, 128);
    std::vector<cv::Scalar> colors = {cv::Scalar(255, 0, 0), cv::Scalar(255, 255, 0), cv::Scalar(255, 255, 255), cv::Scalar(255, 0, 255)};
    float letterSize = 2.0;

    cv::putText(frame, "a", a + origin, 1, letterSize, borderColor, 1 );
    cv::putText(frame, "b", b + origin, 1, letterSize, borderColor, 1 );
    cv::putText(frame, "c", c + origin, 1, letterSize, borderColor, 1 );
    cv::putText(frame, "d", d + origin, 1, letterSize, borderColor, 1 );

    if (helpingLines) {
        // draw corner helping lines
        cv::Scalar helpingBorderColor = cv::Scalar(255, 255, 0);
        float tmpLen = 120;
        cv::line( frame, a + origin, a + origin + cv::Point2f(0,tmpLen), helpingBorderColor, 2 );
        cv::line( frame, a + origin, a + origin + cv::Point2f(tmpLen,0), helpingBorderColor, 2 );
        cv::line( frame, b + origin, b + origin + cv::Point2f(0,tmpLen), helpingBorderColor, 2 );
        cv::line( frame, b + origin, b + origin - cv::Point2f(tmpLen,0), helpingBorderColor, 2 );
        cv::line( frame, c + origin, c + origin - cv::Point2f(0,tmpLen), helpingBorderColor, 2 );
        cv::line( frame, c + origin, c + origin - cv::Point2f(tmpLen,0), helpingBorderColor, 2 );
        cv::line( frame, d + origin, d + origin - cv::Point2f(0,tmpLen), helpingBorderColor, 2 );
        cv::line( frame, d + origin, d + origin + cv::Point2f(tmpLen,0), helpingBorderColor, 2 );
    }
}

/**
 * Draws lines that connect the ObjectBoxes center point with the cameraCenter point.
 * This helps to see the offset that still has to be corrected by the robot.
 *
 * @method ObjectBox::drawCenterOffset
 * @param  frame       to draw to.
 * @param  origin      of the coordinate system within the frame.
 * @param  borderColor color to draw in.
 */
void ObjectBox::drawCenterOffset(cv::Mat &frame, cv::Point2f origin, cv::Scalar borderColor)
{
    cv::Scalar green = cv::Scalar(0, 255, 0), red = cv::Scalar(0, 0, 128);
    std::vector<cv::Scalar> colors = {cv::Scalar(255, 0, 0), cv::Scalar(255, 255, 0), cv::Scalar(255, 255, 255), cv::Scalar(255, 0, 255)};
    float letterSize = 2.0;
    // draw center
    cv::circle(frame, getObjectCenter() + origin, 5.0, red, 2);
    // This line shows the offset on the x axis
    cv::line( frame, getObjectCenter() + origin, cv::Point((getObjectCenter() + origin).x, cameraCenter.y), red, 2 );
    // This line shows the offset in the y axis
    cv::line( frame, cv::Point((getObjectCenter() + origin).x, cameraCenter.y), cameraCenter + origin , red, 2 );
}

// MARK: PRIVATE

/**
 * This init method is part of the ObjectBox initialization process and sets
 * up some basic attributes.
 *
 * @method ObjectBox::init
 */
void ObjectBox::init()
{
    cameraCenter   = cv::Point(WEBCAM_WIDTH/2,WEBCAM_HEIGHT/2);
    screenArea     = WEBCAM_WIDTH * WEBCAM_HEIGHT;
    objectCenter   = cv::Point2f((a.x + b.x)/2, (a.y + d.y)/2);
    boxID = boxCounter++;
    //std::cout << "constructor: boxId = " << boxID << std::endl;
}

/**
 * Prints a one line discription of the ObjextBox.
 *
 * @method ObjectBox::printObjectBox
 */
void ObjectBox::printObjectBox()
{
    if (debug) {
        std::string type, prefix = " ", suffix = "";
        if (isSample()) {
            prefix = relevant ? "+" : "-";
            type = "sample";
        } else {
            type = "pred. ";
            suffix = "\n";
        }

        //std::cout << prefix << " " << type << " " << std::to_string(boxID) << " a: " << a << " b: " << b << " c: " << c << " d: " << d << suffix << std::endl;
        printf("%s %s %4d a[%6.2f, %6.2f]  b[%6.2f, %6.2f]  c[%6.2f, %6.2f]  d[%6.2f, %6.2f]%s\n", prefix.c_str(), type.c_str(), boxID, a.x, a.y, b.x, b.y, c.x, c.y, d.x, d.y, suffix.c_str());
    }
}
