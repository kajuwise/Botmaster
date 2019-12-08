//
// C++ Interface: Neve
//
// Description: 
//
//
// Author: Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef Neve_H
#define Neve_H
#include "abstractrobot.h"
#include "objecttype.h"
#include "comm.h"
/**
	@author Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>
*/

class StateHolder;

class Neve: public AbstractRobot {
public:
    enum BallState {
        BallInSocket,
        BallAlmostInSocket,
        BallNotInSocket
    };


    enum State {
        WAITING_START,                            //          "WAITING_START"; //1
        FIND_BALL,                                //      "FIND_BALL"; //2
        STAND_AND_CENTER_ON_BALL,                //                      "IMPROVE_CONTACT_WITH_BALL"; //3
        ESCAPE_BLACK_LINE,                        //              "ESCAPE_BLACK_LINE";
        START_ESCAPE_BLACK_LINE,                  //                    "START_ESCAPE_BLACK_LINE";
        DISTRONIC_TURN,                           //           "DISTRONIC_TURN";
        ESCAPE_GOAL_TOO_CLOSE,
        GRAB_BALL,
        AIM_TO_SHOOT,
        SHOOT,
        SHOOTING_IN_PROGRESS
    };


public:
        Neve();

        ~Neve();

	void go();
    void setState(State newState);
	void msleep(long t) {usleep(t * 1000);}
	int round(float);
	void resetRemoteCtrl();
    void printGoalInfo();
    void getObjectCalc(Image::Object* ball, float* distance, float* angle, const char* cameraDevice);
    void getCvPointCalc(CvPoint* inputCvPoint, float* distance, float* angle, const char* cameraDevice);

    int centerOnGoal(Image::Object goal, int turnSpeed);
    void getGoalAimDirection(int* basketSpinDirection, float* targetGoalArea, float* targetGoalAngle);
    void squareDriveTest();
    
    void nearGoalSpeedLimiter(Image::Object* goalOwn, Image::Object* goalTarget,int* forwardSpeed);
    void instaTurn(int angle);
    void findClosestBall(const char* cameraDevice);
    void temporaryCrap();
    void checkKeyPressAction();
    void printBallStats(Image::Object * obj, float* angle, float* distance, bool targetBall);
    void printBasketStats(Image::Object * obj, float* angle, float* distance);
    void decideInstaTurnToGetBall(bool oneEightyIfNowhereFound);
    void printCameraSource(const char* camera, bool blind);
    void showCurrentImage();
    void findGoals();

    //Right switches
    void readRobotAndFieldSwitches();

    //Remote control
    void readRemoteCtrl();
    void waitActionSignalFromRemoteCtrl(char action);
    bool isRCSignalTargetingCommand(char action);

    void sideStep(int angle, int distance);
    bool decideIfAvoidCloseToGoal();

private:
    void drawCircleOnGoal();
    void distancePreserveTurn(int angle);
    bool doDistonicTurnIfNeeded();
    bool doDistronicTurnBasedOnGoalPosition(const char* cameraDevice);
    BallState getBallSocketState() const;
    bool isCloseToGoal() const;
    void setThrowerRpmIfBasketIsSeen();
    void setThrowerRpm(int rpm);
    int calculateThrowerRpm(int distance);
    float getThrowAngleCorrectionRelativeToField();


private:
    int lastTargetedRCCommand;
    QChar selectedRobot;
    QChar selectedField;
};


#endif
