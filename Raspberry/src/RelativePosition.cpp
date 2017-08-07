/*
** @author Daniel Palenicek
** @version 0.1 / 29.08.2016
**
** Copyright © 2016 Daniel. All rights reserved.
*/

#include "RelativePosition.hpp"


// MARK: Constructors

/**
 * Simple constructor that sets up the necessary constants
 */
RelativePosition::RelativePosition()
{
    Logger::debug("RelativePosition Constructor");
    cameraCenter  = cv::Point(WEBCAM_WIDTH/2,WEBCAM_HEIGHT/2);
    screenArea    = WEBCAM_WIDTH * WEBCAM_HEIGHT;
    //cv::Point2f x = cv::Point2f(0,0);
    //std::array<cv::Point2f, 4> temp = {x,x,x,x};
    //objectBox     = new ObjectBox(temp);
}


// MARK: Getter & Setter

/**
 * This function returns the objects area in relation to the entire screens
 * area. From that one can tell how close the object is to the camera.
 *
 * @return percentage of object area to screen area
 */
float RelativePosition::getRelativeObjectArea()
{
    return objectBox->getRelativeObjectArea();
}

/**
 * This function returns the objects center cooriantes in the camera coordinate system.
 *
 * @return coordinates of the objects center in the cameras coordinae system.
 */
cv::Point2f RelativePosition::getObjectCenter()
{
    return objectBox->getObjectCenter();
}

/**
 * Appends an ObjectBox to the sampleObjectBoxes array.
 * @param newSample is ObjectBox to append.
 */
void RelativePosition::addSampleBox(ObjectBox * newSampleBox)
{
    sampleObjectBoxes.push_back(ObjectBox(*newSampleBox));
}

/**
 * The sampleBoxes are processed by passing them to the ObjectBox constructor.
 * This returns the main ObjectBox that is used for further calculations.
 *
 * @method RelativePosition::processSampleBoxes
 */
void RelativePosition::processSampleBoxes()
{
    objectBox = new ObjectBox(sampleObjectBoxes);
}


// MARK: Other functions

/**
 * This method returns whether the object was detected in the scene or not.
 * The rules are based on experience and trial and error and may not be ideal.
 *
 * @return if the target object was detected in the scene
 */
bool RelativePosition::objectDetected()
{
    return objectBox->objectDetected();
}

/**
 * Calculates whether the ObjectBox (minus a certain threashold) intersects the
 * cameraCenter on the horizontal and the vertical axis. This is important information
 * to figure out whether the roboter's projectiles would hit the target if fired.
 *
 * @return bool of whether the intersection is given or not.
 */
bool RelativePosition::cameraCenterIntersectsTarget()
{
    return cameraCenterIntersectsTargetHorizontaly() && cameraCenterIntersectsTargetVerticaly();
}

/**
 * Returns whether the objectBox (minus a certain threashold) intersects the
 * cameraCenter on the horizontal axis. This is important information to figure
 * out whether the roboter's projectiles would hit the target if fired.
 *
 * @return bool of whether the intersection is given or not.
 */
bool RelativePosition::cameraCenterIntersectsTargetHorizontaly()
{
    return objectBox->cameraCenterIntersectsTargetHorizontaly();
}

/**
 * Returns whether the objectBox (minus a certain threashold) intersects the
 * cameraCenter on the horizontal axis. This is important information to figure
 * out whether the roboter's projectiles would hit the target if fired.
 *
 * @return bool of whether the intersection is given or not.
 */
bool RelativePosition::cameraCenterIntersectsTargetVerticaly()
{
    return objectBox->cameraCenterIntersectsTargetVerticaly();
}

/**
 * This funtion calculates the distance of the objects center toward the distance
 * of the camera center point in pixels. Both in x and y direction.
 *
 * @return contains both x and y distance of the object center to the camera center in pixels.
 */
cv::Point2f RelativePosition::distanceOfObjectToCameraCenter()
{
    return objectBox->distanceOfObjectToCameraCenter();
}


/**
 * This method calculates the relative position of the camera to the target object.
 * @return an enum that contains the relative position
 */
/*
RelativePosition::side RelativePosition::whichSideOfTheLauncherIsTheObjectLocatedAt()
{
    float leftSide = (d.y - a.y), rightSide = (c.y - b.y), threshhold = 1.01;

    if (false) {
        //printf("center");
        return RelativePosition::side::streightAhead;
    }
    else if (leftSide / rightSide < 0.9) {
        //printf("left");
        return RelativePosition::side::left;
    }
    else if (rightSide / leftSide < 0.9) {
        //printf("right");
        return RelativePosition::side::right;
    }
    else {
        //printf("none");
        return RelativePosition::side::none;
    }
}
*/


// MARK: Drawing functions

/**
 * This function draws its classes relevant Kexpoints onto the frame it is passed.
 * This helps when visually debugging.
 *
 * @param frame  the frame to draw on
 * @param origin the offset to draw everything to in the x axis so that it ende
 *               up in the right place.
 */
void RelativePosition::drawKeyPointsOntoCVMat(cv::Mat &frame, cv::Point2f origin)
{   // TODO: maybe make origin a pointer.
    float letterSize = 2.0;

    for (int i=0; i<sampleObjectBoxes.size(); i++) {
        sampleObjectBoxes[i].drawBorders(frame, origin);
    }

    if (objectDetected()) {
        objectBox->drawBorders(frame, origin);
        objectBox->drawCorners(frame, origin, green, false);
        //objectBox->drawCenterOffset(frame, origin, green);
    }

    drawText(frame, origin);
    clearSampleBoxes();
}

/**
 * This function draws statistics to a frame.
 *
 * @method RelativePosition::drawText
 * @param frame  the frame to draw on
 * @param origin the offset to draw everything to in the x axis so that it ende
 *               up in the right place.
 */
void RelativePosition::drawText(cv::Mat &frame, cv::Point2f origin)
{
    int letterThickness = 1;
    if (objectDetected()) {
        cv::putText(frame, "area  : " +  std::to_string(getRelativeObjectArea()),            cv::Point2f(5, 20) + origin, 1, 1.0, green, letterThickness );
        cv::putText(frame, "deltaX: " +  std::to_string(distanceOfObjectToCameraCenter().x), cv::Point2f(5, 40) + origin, 1, 1.0, green, letterThickness );
        cv::putText(frame, "deltaY: " +  std::to_string(distanceOfObjectToCameraCenter().y), cv::Point2f(5, 60) + origin, 1, 1.0, green, letterThickness );
    }

/*
    // This text label represents the relative position of the robot to the target object.
    std::string sideString = "none";
    if (objectDetected()) {
        RelativePosition::side tempSide = whichSideOfTheLauncherIsTheObjectLocatedAt();
        switch(tempSide) {
            case RelativePosition::side::streightAhead: sideString = "center"; break;
            case RelativePosition::side::left: sideString = "left"; break;
            case RelativePosition::side::right: sideString = "right"; break;
        }
    }
    cv::putText(frame, "side: " + sideString, cv::Point2f(5, 80) + origin, 1, 1.0, green, letterThickness );
    */
}


// MARK: PRIVATE

/**
 * Clears the sample boxes array. After processing and drawing the sample boxes
 * they are not needed anymore and need to be cleared before new sample boxes
 * get saved.
 */
void RelativePosition::clearSampleBoxes()
{
    sampleObjectBoxes.clear();
}
