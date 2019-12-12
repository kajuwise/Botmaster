//
// C++ Implementation: NeveTest
//
// Description: 
//
//
// Author: Valdur Kaldvee, Erik Kaju, Indrek Tamm (C) 2010-2018
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "robottesttool.h"
#include "nevetest.h"
#include <QTime>
#include <QDebug>
#include <iostream>
#include <math.h>

#include <QMessageBox>
#include <QMenu>
#include <QAction>

#define DIGITAL_SIG_ROBOT_SELECTOR 0
#define DIGITAL_SIG_FIELD_SELECTOR 1
#define DIGITAL_SIG_START_BUTTON 2
#define DIGITAL_SIG_EZ_RC 3

using namespace std;
RobotTestTool *robotTestToolWindow;

NeveTest::NeveTest()
{
    robotTestToolWindow = new RobotTestTool(&omniTestDirDeg, &omniTestVelocityBody, &omniTestVelocityAngular, &throwerTestPwm, &throwerTestRpm);
    robotTestToolWindow->setWindowTitle(QString::fromUtf8("Robot test tool"));
}

NeveTest::~NeveTest()
{
}

void NeveTest::motorTest() {
    qDebug() << "Performing motor test";
	int i;
	
	for (i = 0; i < 3; i++) {
        setDcMotor(i, 50);
		msleep(2000);
        setDcMotor(i, -50);
		msleep(2000);
		setDcMotor(i, 0);
	}
}

void NeveTest::solenoidTest() {
    setDcMotor(4, 1);
    msleep(100);
    setDcMotor(4, 0);
}

void NeveTest::go() {
    char selection = '0';
	char str[50];
	int i;
	int height;
	
	qDebug("NeveTest GO...\n");
	
	Config  & conf  = Config::getConfig();
    int fieldTop[1280];
    temporaryCrap();
    const char* getFrameFromCameraNr = CAMERA_FRONT;

    conf.setSendCmdEnabled(1);
    setOmni(0,0,0);
    setDcMotor(3,0);
    setThrowerCommand(0,0);
    conf.setSendCmdEnabled(0);

    char hello[] = "1,2,-3,4,5";
    qDebug() << atoi(hello) << " must be 1";
    qDebug() << atoi(&hello[2]) << " must be 2";
    qDebug() << atoi(&hello[4]) << " must be -3";

    while(1) {
        requestSensors();
        readRobotAndFieldSwitches();
        readRemoteCtrl();
        _img = image->getFrame(getFrameFromCameraNr);
        image->process(fieldTop);
        getSensorsResponse();

        if (abs(omniTestDirDeg) > 0 || abs(omniTestVelocityBody) >> 0 || abs(omniTestVelocityAngular) > 0) {
            conf.setSendCmdEnabled(1);
            setOmni(omniTestDirDeg, omniTestVelocityBody, omniTestVelocityAngular);
            conf.setSendCmdEnabled(0);
        }

        /*
        if (throwerTestPwm > 0) {
            conf.setSendCmdEnabled(1);
                //setThrowerCommand(0,0); //set roller state to "collecting"
                setDcMotor(5, throwerTestPwm);
           //
           conf.setSendCmdEnabled(0);
        }*/

/*
        if (throwerTestRpm >= 1) {
            int rpm = throwerTestRpm < 250 ? 0 : throwerTestRpm;
            conf.setSendCmdEnabled(1);
            setDcMotor(4, rpm);
            conf.setSendCmdEnabled(0);
        }
*/

		selection = conf.keyS;
		switch(selection) {
            case '1':
				sprintf(str, "Motor test");
				if (conf.getSendCmdEnabled()) {
                    qDebug("MOTOR TEST;");
                    motorTest();
					conf.setSendCmdEnabled(0);
				}
				break;
            case '2':
				sprintf(str, "Solenoid test");
				if (conf.getSendCmdEnabled()) {
					solenoidTest();
					conf.setSendCmdEnabled(0);
				}
				break;
            case '3':
                sprintf(str, "Just throw");
                if (conf.getSendCmdEnabled()) {
                    justThrow();
                    conf.setSendCmdEnabled(0);
                }
                break;
            case '4':
                sprintf(str, "Instaturn left");
                if (conf.getSendCmdEnabled()) {
                    instaTurn(-90);
                    conf.setSendCmdEnabled(0);
                }
                break;
            case '5':
                sprintf(str, "Instaturn right");
                if (conf.getSendCmdEnabled()) {
                    instaTurn(90);
                    conf.setSendCmdEnabled(0);
                }
                break;
            case '6':
                sprintf(str, "Sidestep left");
                if (conf.getSendCmdEnabled()) {
                    sideStep(-100, 30);
                    conf.setSendCmdEnabled(0);
                }
                break;
            case '7':
                sprintf(str, "Sidestep right");
                if (conf.getSendCmdEnabled()) {
                    sideStep(100, 30);
                    conf.setSendCmdEnabled(0);
                }
                break;
            case '8':
                sprintf(str, "Thrower command");

                if (conf.getSendCmdEnabled()) {
                    qDebug("Sending thrower command for 4000 RPM with 5 percent precision.");
                    setThrowerCommand(throwerTestRpm, 5);
                    setDcMotor(4, throwerTestRpm);
                    conf.setSendCmdEnabled(0);
                }


                break;

            case '9':
                sprintf(str, "pwm thrower test");
                if (conf.getSendCmdEnabled()) {
                    setThrowerCommand(5, 5);
                    conf.setSendCmdEnabled(0);
                }

                conf.setSendCmdEnabled(1);
                setDcMotor(5, throwerTestPwm);
                conf.setSendCmdEnabled(0);
                break;
            case 'x':
                conf.setSendCmdEnabled(1);
                setOmni(0,0,0);
                //setDcMotor(4,0);
                setDcMotor(5,255);
                setThrowerCommand(-1,-1);
                exit(2);
                break;
            case 'l':
                sprintf(str, "Image from left cam");
                getFrameFromCameraNr = CAMERA_LEFT;
                break;
            case 'f':
                sprintf(str, "Image from central cam");
                getFrameFromCameraNr = CAMERA_FRONT;
                break;
            case 'r':
                sprintf(str, "Image from right cam");
                getFrameFromCameraNr = CAMERA_RIGHT;
                break;
            case 'a':
                sprintf(str, "Distronic turn (circling)");
                if (conf.getSendCmdEnabled()) {
//                    distancePreserveTurn(90);

//                    setOmni(-83, abs(30), 30);
                    //setOmni(-83, abs(60), 30);

                    setOmni(0,0,65);
                    msleep(400);
                    setOmni(0,0,0);
//                    distancePreserveTurn(-90);

                    conf.setSendCmdEnabled(0);
                }
                break;

            case 'm':
                sprintf(str, "Opened test tool menu");
                if (robotTestToolWindow->isVisible()) {
                    robotTestToolWindow->close();
                } else {
                    robotTestToolWindow->show();
                }
                conf.keyS = NULL;
                break;
            case 't':
                sprintf(str, "Thrower test");
                if (conf.getSendCmdEnabled()) {
                    qDebug() << "Will be performing thrower test";
                    doThrowerTest();
                    conf.setSendCmdEnabled(0);
                }
                break;

            default:
               sprintf(str, "No test selected");
				break;
		}		
	
		// Show selected test text
        cvPutText(_img, str, cvPoint(60, 20), &(image->font), CV_RGB(50,100,0));
		
		// Show analog values
                height = 120;

		for (i = 0; i < 8; i++) {
            sprintf(str, "A%d %d", i, microcontrollerData[i]);
            cvPutText(_img, str, cvPoint(10, height), &(image->font), CV_RGB(66, 226, 244));
			height += 20;
		}
		
		// Show digital values
                height = 120;
		
		for (i = 0; i < 8; i++) {
            sprintf(str, "D%d %d", i, microcontrollerData[i+8]);
            cvPutText(_img, str, cvPoint(250, height), &(image->font), CV_RGB(66, 226, 244));
			height += 20;
		}
		
		view->show(_img);
	}
}

void NeveTest::instaTurn(int angle) {
    int angular = 255;
    if(angle < 0) {
        angular *= -1;
    }

    int duration = 27.8555373569* (pow((float)abs(angle),0.4880905071));
    setOmni(0, 0, angular);

    msleep(duration);
    qDebug() << "ANGLE:" << angle << ", DURATION:" << duration;
    setOmni(0, 0, 0);
    msleep(10);
}

void NeveTest::temporaryCrap() {
    int fieldTop[1280];
    //First image should be taken from side camera with big height, without that next frame from cental cam will fail
    _img = image->getFrame(CAMERA_RIGHT);
    image->process(fieldTop);
}

void NeveTest::sideStep(int angle, int distance) {
    if(distance < 0 || distance > 50) return;

    int maxSideSpeed = 130;
    setOmni(0, 0, 0);

    setOmni(angle, maxSideSpeed, 0);
    //int duration = 72.9414522784*pow(distance,0.5532342559); //power
    int duration = 9.52*distance + 181.5111111111; //linear
    msleep(duration);
    setOmni(0, 0, 0);
}

QChar selectedRobot;
QChar selectedField;
void NeveTest::readRobotAndFieldSwitches() {
    if (microcontrollerData[DIGITAL_SIG_ROBOT_SELECTOR] == 0) {
        selectedRobot = QChar(RC_SIG_A);
    } else if (microcontrollerData[DIGITAL_SIG_ROBOT_SELECTOR] == 1) {
        selectedRobot = QChar(RC_SIG_B);
    }

    if (microcontrollerData[DIGITAL_SIG_FIELD_SELECTOR] == 0) {
        selectedField = RC_SIG_A;
    } else if (microcontrollerData[DIGITAL_SIG_FIELD_SELECTOR] == 1) {
        selectedField = RC_SIG_B;
    }
}

//Remote control
int lastRemoteSignal = 0;
void NeveTest::readRemoteCtrl() {
    if (isSignalTargeting(RC_SIG_STOP)) {
        waitActionSignalFromRemoteCtrl(RC_SIG_START);
    }
}

void NeveTest::waitActionSignalFromRemoteCtrl(char action) {
    while(1) {
        requestSensors();
        getSensorsResponse();
        if (isSignalTargeting(action) == true) break;
    }
}

bool NeveTest::isSignalTargeting(char action) {
    int currentRemoteSignal = microcontrollerData[MC_DATA_REMOTE_SIGNAL];
    if (currentRemoteSignal != lastRemoteSignal) {
        if (lastRemoteSignal != 0) {
            QString currentSignalQStr = QString::number(currentRemoteSignal);
            qDebug() << "New remote control signal:" << currentSignalQStr;
            if (selectedField == currentSignalQStr[RC_FIELD]
                    && (currentSignalQStr[RC_ROBOT] == RC_SIG_ALL_ROBOTS ||  currentSignalQStr[RC_ROBOT] == selectedRobot)
                    && currentSignalQStr[RC_ACTION] == action) {
                lastRemoteSignal = currentRemoteSignal;
                return true;
            }
        }
    }
    lastRemoteSignal = currentRemoteSignal;
    return false;
}

void NeveTest::distancePreserveTurn(int angle) {
    int turnSpeed = 110;

    int originalAngle = angle;
    if(angle<0){
        angle = -angle;
        turnSpeed = -turnSpeed;
    }
    angle -= 360*int(angle/360);

    if(angle>180){
        angle = angle-180;
    }

    int duration = 7.1369346734*angle+30.1344221106; //175.9303173589 * exp(0.0155098758 * angle);

//    setDcMotor(0, 15);
//    setDcMotor(1, 15);
//    setDcMotor(2, turnSpeed);

    qDebug() << "ORIGINAL ANGLE:" << originalAngle << ", TURN ANGLE:" << angle << ", DURATION:" << duration << ", turning" << (turnSpeed > 0 ? "right" : "left");
    msleep(duration);

    setOmni(0, 0, 0);
    msleep(10);
}

void NeveTest::doThrowerTest() {
    int maxThrowingTestSpeed = 150;

    for (int speed = 0; speed<maxThrowingTestSpeed; speed=speed+10) {
        qDebug() << "Setting dc motor 3 speed to" << speed;
        setDcMotor(3, speed);
        msleep(300);
    }

   for (int speed = maxThrowingTestSpeed; speed>=0; speed=speed-10) {
        setDcMotor(3, speed);
        msleep(300);
   }
}

void NeveTest::justThrow() {
    setDcMotor(3, 100);
    msleep(5000);
    setDcMotor(3, 0);
}



