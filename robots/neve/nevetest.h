//
// C++ Interface: NeveTest
//
// Description: 
//
//
// Author: Valdur Kaldvee, Erik Kaju
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef NEVE_TEST_H
#define NEVE_TEST_H

#include<QtGui>
#include <QMainWindow>
#include "abstractrobot.h"

class NeveTest: public AbstractRobot
{
public:
	NeveTest();
	~NeveTest();
    void go();
    void msleep(long t) {usleep(t * 1000);}
	void motorTest();
	void solenoidTest();
    void instaTurn(int angle);
    void doThrowerTest();
    void justThrow();
    void temporaryCrap();
    void sideStep(int relativeAngle, int distance);
    void readRobotAndFieldSwitches();

    //Remote control
    void readRemoteCtrl();
    void waitActionSignalFromRemoteCtrl(char action);
    bool isSignalTargeting(char action);
    void distancePreserveTurn(int angle);

    int omniTestDirDeg = 0;
    int omniTestVelocityBody = 0;
    int omniTestVelocityAngular = 0;
    int throwerTestPwm = 0;
    int throwerTestRpm = 0;

	  
};
#endif
