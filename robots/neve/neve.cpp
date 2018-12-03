//
// C++ Implementation: Neve
//
// Description: 
//
//
// Author: Valdur Kaldvee, Erik Kaju, Indrek Tamm (C) 2010-2018
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "neve.h"
#include <QTime>
#include <QDebug>
#include <math.h>
#include <iostream>

#include "stateholder.h"

//#define _GLIBCXX_USE_CXX11_ABI 0 //indrek, what is it?

#define BEAM_VALUE_BALL_IN_SOCKET 250

#define GOAL_CLOSE 270
#define BLACK_LINE 850

#include "robottesttool.h"


/* Omni cheatsheet
 * dirDeg - direction of movement (0 is forward, 90 is sideways to the right)
 * velocityBody: drive speed
 * velocityAngular: spinning speed
*/


//Target ball
float ballDistanceFromRobot = 0.0;
float ballMinAngle = 0.0;
float minBallDistanceFromEdgePoint = 0.0;

Image::Object ball;
Image::Object goalTarget;
Image::Object goalOwn;
Image::Object * obj;
int fieldTop[1280];
char str[50];

int ballTotal = 0;
int basketSpinDirection = 1;
int leftCamAngleDiff = -80;
int rightCamAngleDiff = 82;

int goalTargetColor = BASKET_PURPLE;
int goalOwnColor = BASKET_BLUE;

Config & conf = Config::getConfig();

StateHolder state("current", Neve::WAITING_START);
StateHolder statePrev("previous", Neve::WAITING_START);

bool infoMode = true;

// For competition, these should be initalized to false
bool waitingSecondClick = false;
bool clearToKick = false;
bool listeningToRemote = false;
bool mustRun = false;
bool isDisplayImageRefreshedDuringRun = false; //set to false if you want to disable image refreshing
//

// Is true until state is not WAITING_START anymore
bool displayImageRefresh = true;

const char* getFrameFromCameraNr = CAMERA_FRONT;
//    const char* getFrameFromCameraNr = CAMERA_LEFT;
//    const char* getFrameFromCameraNr = CAMERA_RIGHT;

bool testToolEnabled = false;
int pwmTestValue = 30;

QTime timerLastDistronicTurn;
QTime inCurrentStateTime;
bool allowedtoGetThrowAngleCorrectionRelativeToField;

//
//
// Neve class
//
//

QTime timerUnableCenterBallAndBasket;
float angleCorrectionDueToFieldPosition;


Neve::Neve()
 : lastTargetedRCCommand(0)
{ } // constructor
Neve::~Neve() { } // destructor

void Neve::go() {

    if (testToolEnabled) {
        int testPar1 = 0;
        int testPar2 = 0;
        int testPar3 = 0;
        RobotTestTool * robotTestTool = new RobotTestTool(&testPar1, &testPar2, &testPar3, &pwmTestValue);
        robotTestTool->show();
    }

    setOmni(0, 0, 0);
    int turnSpeed = 0;
    int forwardSpeed = 0;
    int angle = 0;
    int drivingForward = 0;
    int error = 0;
    int random = 0;
    int elapsed = 0;
    int kickCount = 0;
    int noBallTurnDir = 1;
    int lastRemoteSignal = 0;

    bool isNearBall = 0;

    ball.distanceH = 0;

    QTime timer_roundTimeElapsed;
    QTime timer_ballKickTime;
    QTime timer9;
    QTime timer12;
    QTime timer13;
    QTime timerSinceLastThrow;



    //Start thrower right away
    conf.setSendCmdEnabled(1);
    resetRemoteCtrl();
    setDcMotor(3,45);
    conf.setSendCmdEnabled(0);

    qDebug("Neve GO...\n");
    timer12.start();

    while (0) {
        conf.setSendCmdEnabled(1);
        squareDriveTest();
    }

    temporaryCrap(); //TODO FIX AND REMOVE THAT CRAP

    if (conf.getImmediateStart() == true) {
        qDebug("entering getImmediateStart");
        state = FIND_BALL;
        mustRun = true;
    }


    while (1) {

        //uncomment to print fielddifs
//        if(1) {
//            getThrowAngleCorrectionRelativeToField();
//        }

        if (testToolEnabled) {
            conf.setSendCmdEnabled(1);
            setDcMotor(3, pwmTestValue);
            conf.setSendCmdEnabled(0);
        }

        requestSensors();
        _img = image->getFrame(getFrameFromCameraNr);
        printCameraSource(getFrameFromCameraNr, false);
        image->process(fieldTop);
        printGoalInfo();
        checkKeyPressAction();
        getSensorsResponse();
        readRobotAndFieldSwitches();
        readRemoteCtrl();
        clearToKick = true;

        if (image->found_objects != NULL) {
            //findGoal
            findGoals();

            // Find closest ball						
            findClosestBall(getFrameFromCameraNr);
        }

        // Robot gets start command
        if (state == WAITING_START && mustRun == true) {
            timer_roundTimeElapsed.start();
            timer_ballKickTime.start();

//            TODO: ALWAYS PUT IT BACK
            //Start-speed-stair
            setOmni(0, 100, 0);
            msleep(10);
            setOmni(0, 200, 0);
            msleep(100);
            setOmni(0, 254, 5);

            int blindDelay =
//                    1000 //diagonal to center
//                    750 //via black line to midpoint
                    1500 //via black line to other end
//                    200 // almost no delay
                    ;
            msleep(blindDelay);

            requestSensors();
            getSensorsResponse();

            statePrev = state;
            state = FIND_BALL;
        }

        // Robot is waiting start
        if (state == WAITING_START) {
            if (digital[6] == 0 && listeningToRemote == false) {
                qDebug() << "Manual button pressed down. Setting listening to remote to true;";
                listeningToRemote = true;
                continue;
            } else if (listeningToRemote == true && digital[6] == 1) {
//                qDebug() << "Manual button key up. Starting to wait for second click.";
                waitingSecondClick = true;
                conf.setSendCmdEnabled(1);
            } else if (digital[6] == 0 && waitingSecondClick == true) {
                qDebug() << "Received second click and setting mustRun to true!";
                mustRun = true;
                continue;
            }

            drawCircleOnGoal(); //todo put back

//            if(conf.getImmediateStart()){
//                qDebug() << "I must start now!";
//                state = FIND_BALL;
//            }

        }

        // Robot is running
        if (state != WAITING_START) {
            elapsed = timer_roundTimeElapsed.elapsed() / 1000;
            sprintf(str, "Time %d", elapsed);
            cvPutText(_img, str, cvPoint(10, 210), &(image->font), CV_RGB(0, 0, 0));

            // Do not show images anymore
            if(!isDisplayImageRefreshedDuringRun) {
                displayImageRefresh = false;
            }
        }

        if (state == ESCAPE_GOAL_TOO_CLOSE) {
            qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz") << "AVOID GOAL TOO CLOSE !!!!!!!!";
            setOmni(0,0,0);
            msleep(250);
            sideStep(180,25);
            instaTurn(160);
            msleep(100);
            decideInstaTurnToGetBall(false);
            state = FIND_BALL;
            continue;
        }

        // Robot is searching the ball.
        if (state == FIND_BALL) {

            if(decideIfAvoidCloseToGoal()){
                continue;
            }

            if (state != statePrev) {
                statePrev = state;
                inCurrentStateTime.start();
                timer9.start();
                timer13.start();
            }

            if (getBallSocketState() == BallInSocket) { // TODO improve readability - this block here is h dealing with ball socket sensor, greater value emans more light form other side of roller
                state = IMPROVE_CONTACT_WITH_BALL;
            } else {
                if (isNearBall && (ball.area == 0 || ballDistanceFromRobot > 20.0)) {
                    state = IMPROVE_CONTACT_WITH_BALL;
                } else {
                    if (ball.area != 0) {
                        setThrowerSpeedIfBasketIsSeen();
                        timer9.start();

                        angle = 0;
                        forwardSpeed = 200;
                        turnSpeed = ballMinAngle * 0.7;

                        if (ballDistanceFromRobot < 150.0) {
                            forwardSpeed = 200;
                            if(abs(ballMinAngle) > 20 ){forwardSpeed = 205; } //polnud
                            turnSpeed = ballMinAngle * 0.7;
                        }

                        if (ballDistanceFromRobot < 120.0) {
                            forwardSpeed = 190; //180
                            if(abs(ballMinAngle) > 20 ){forwardSpeed = 205; } //200
                            turnSpeed = ballMinAngle * 0.7;
                        }

                        if (ballDistanceFromRobot < 100.0) {
                            forwardSpeed = 180; //160
                            if(abs(ballMinAngle) > 20 ){forwardSpeed = 200; } //180
                            turnSpeed = ballMinAngle * 0.7;
                        }

                        if (ballDistanceFromRobot < 90.0) {
                            forwardSpeed = 160; //140
                            if(abs(ballMinAngle) > 20 ){forwardSpeed = 170; } //160
                            turnSpeed = ballMinAngle * 0.7;
                        }

                        if (ballDistanceFromRobot < 80.0) {
                            forwardSpeed = 140; //120
                            if(abs(ballMinAngle) > 20 ){forwardSpeed = 160; } //140
                            turnSpeed = ballMinAngle * 0.7;
                        }

                        if (ballDistanceFromRobot < 60.0) {
                            forwardSpeed = 120; //100
                            if(abs(ballMinAngle) > 20 ){forwardSpeed = 140; } //110
                            turnSpeed = ballMinAngle * 0.75;
                        }

                        if (ballDistanceFromRobot < 30.0) {
//                            qDebug() << ballDistanceMinAngle;
                            //If angle is big, take it delicately

                            if (abs(ballMinAngle) > 20 ){
                                forwardSpeed = 50;
                                turnSpeed = ballMinAngle * 0.95;
                            } else {
                                forwardSpeed = 55;
                                turnSpeed = ballMinAngle * 0.8;
                            }
                        }

                        if (ballDistanceFromRobot < 20.0) {
                            setOmni(0,0,0);
                            msleep(100);
                            setState(DISTRONIC_TURN);
                            continue;
                        }

                        bool ballDebug = false;
                        bool ballTests = false;

                        if(ballDebug){
                            qDebug() << "Pall - nurk: " << ballMinAngle << ", distanceHCm: " << ball.distanceHCm << ", distanceVCm: " << ball.distanceVCm << ", ballDistanceMin:" << ballDistanceFromRobot;
                        }

                        if(ballTests && ballMinAngle < 45.0 && ballMinAngle > 20.0 && ball.distanceVCm > 30 && ball.distanceVCm < 50 ){
                            turnSpeed = 0;
                            forwardSpeed = 50;
                            angle = 70;
                        }


                        //TRIKID TRIKID
                        /* */
                        //qDebug() << "Holy angel:" << ballDistanceMinAngle << ", ball distance" << ballDistanceMin;
                        int maxSupportedBallAngle = 25;
                        int maxSupportedBallDistance = 105;
                        bool autoAimEnabled = true;
                        if(autoAimEnabled
                                && goalTarget.area != 0
                                && abs(ballMinAngle) <= maxSupportedBallAngle
                                && ballDistanceFromRobot <= maxSupportedBallDistance
                                && minBallDistanceFromEdgePoint > 200 /*magic*/) {
                            //qDebug() << minBallDistanceFromEdgePoint;
                            forwardSpeed *= 0.75;
                            //float goalDistance, goalAngle;
                            //calculateGoal(&goalTarget, &goalDistance, &goalAngle);
                            //qDebug() << "Pall - nurk: " << ballDistanceMinAngle << ", distanceHCm: " << ball.distanceHCm << ", distanceVCm: " << ball.distanceVCm << ", ballDistanceMin:" << ballDistanceMin;
                            //qDebug() << "angle:" << goalAngle << ", distance:" << goalDistance << ", distanceH:" << goalTarget.distanceH << ", distanceVcm:" << goalTarget.distanceVCm << ", distanceHcm:" << goalTarget.distanceHCm;
                            //forwardSpeed = 60 ; // forwardSpeed*0,5;

                            int newTurnSpeed;
                            float magicConstant = 0.15;
                            float goalAngle = 0.0; //kirjutatakse yle, kastutan
                            float goalDistance = 0.0; //kirjutatakse yle, ei kasuta
                            bool forceRight = false;

                            getObjectCalc(&goalTarget, &goalDistance, &goalAngle, getFrameFromCameraNr);

                            if(goalAngle > 0 && ballMinAngle > 0){
                                if(goalAngle > ballMinAngle){
                                    newTurnSpeed = abs(goalAngle - ballMinAngle) * magicConstant;
                                    //qDebug() << "paremale - v2r-pos > pall-pos";
                                }else{
                                    magicConstant = magicConstant * (-1);
                                    newTurnSpeed = abs(ballMinAngle - goalAngle) * magicConstant;
                                    //qDebug() << "vasakule - pall-pos > v2r-pos";
                                }

                            }
                            if((goalAngle > 0 && ballMinAngle < 0) || (goalAngle < 0 && ballMinAngle > 0)){
                                if(ballMinAngle > 0){
                                    magicConstant = magicConstant * (-1);
                                    forceRight = true;
                                    //qDebug() << "vasakule  - pall-pos : v2r-neg";
                                }else{
                                    //qDebug() << "paremale - v2r-pos : pall-neg";
                                }
                                newTurnSpeed = (abs(goalAngle)+abs(ballMinAngle)) * magicConstant;
                                //qDebug() << "pxxrde suund" << newTurnSpeed << " ja maagiline konstant: " << magicConstant;
                            }

                            if(goalAngle < 0 && ballMinAngle < 0){
                                if(goalAngle > ballMinAngle){
                                    newTurnSpeed = abs(ballMinAngle - goalAngle) * magicConstant;
                                    //qDebug() << "paremale - v2r-neg > pall-neg";
                                }else{
                                    magicConstant = magicConstant * (-1);
                                    newTurnSpeed = abs(goalAngle - ballMinAngle) * magicConstant;
                                    //qDebug() << "vasakule - pall-neg > v2r-neg";
                                }

                            }

                            turnSpeed = newTurnSpeed;
                            float angleAmplifier;
                            if(ballMinAngle <= 7) {  // TODO testing and optimization
                                angleAmplifier = 2.5;
                            } else if(ballMinAngle <= 15) {
                                angleAmplifier = 4;
                            } else if (ballMinAngle <= 25) {
                                angleAmplifier = 4.5;
                            } else if (ballMinAngle <= 30) {
                                angleAmplifier = 6;
                            } else if (ballMinAngle <= 35) {
                                angleAmplifier = 7;
                            } else if (ballMinAngle <= 40) {
                                angleAmplifier = 8;
                            }

                            if(turnSpeed > 0){
                                    angle = ballMinAngle * angleAmplifier;// - 60;
                            }else{
                                    angle = ballMinAngle * angleAmplifier;
                            }
                        }
                    } else if (timerSinceLastThrow.elapsed() < 2200) {
                        decideInstaTurnToGetBall(false);
                        setOmni(0,5,0);
                        showCurrentImage();
                        continue;
                    } else {
                        qDebug() << "No balls seen, insta turn";
                        decideInstaTurnToGetBall(true);
                        setOmni(0,0,0);
                        msleep(100);
                        showCurrentImage();
                        continue;
                    }
                    setOmni(angle, forwardSpeed, turnSpeed); //Prev else if block has set up angle, forwardSpeed and turnSpeed params
                }
            }
        }

        if (state == DISTRONIC_TURN) {

            setThrowerSpeedIfBasketIsSeen();

            if(ballTotal == 0 || ballDistanceFromRobot > 20) {
                setState(FIND_BALL);
            }

            int direction = basketSpinDirection;

            // initial full speed values
            float ballDistanceMultiplier = 2.0;
            float turnSpeedMultiplier = 2.0;
            float velocity = 50.0;
            float angleCorrection = 15.0;
            float circlingDistance = 7.0;

            if (goalTarget.area > 0) {

                if (allowedtoGetThrowAngleCorrectionRelativeToField == true) {
                    angleCorrectionDueToFieldPosition = getThrowAngleCorrectionRelativeToField();
                    allowedtoGetThrowAngleCorrectionRelativeToField = false;
                }

                float basketAngle = 0.0;
                float basketDistance = 0.0;
                getObjectCalc(&goalTarget, &basketDistance, &basketAngle, getFrameFromCameraNr);

                basketAngle -= 2.9; //default angle shifter
                basketAngle += 4.1 * angleCorrectionDueToFieldPosition; //field position driven angle shifter


                if (abs(basketAngle) <= 40) {
                    velocity = 10.0; //0.28 * abs(basketAngle) + 3.57; //40degrees -> 15.0, 5degrees -> 5
                    angleCorrection = 7.0;
                    ballDistanceMultiplier = 1.2;
                    turnSpeedMultiplier = 1.0;
                }

                if (basketAngle > 0) {
                    direction = 1;
                } else {
                    direction = -1;
                }

               float allowedAngleMistake = 1.0;
               if (basketDistance > 200) {
                allowedAngleMistake = 3.0;
               }

               if ((abs(basketAngle) <= allowedAngleMistake && abs(ballMinAngle) <= allowedAngleMistake) ||
                        (abs(basketAngle) <= 5.0 && abs(ballMinAngle) <= 5.0 && timerUnableCenterBallAndBasket.elapsed() > 3500) ) {
                    setOmni(0,0,0);
                    msleep(50);
                    setState(IMPROVE_CONTACT_WITH_BALL);
                    continue;
                }
            }

            angle = (-direction * 90) + (direction * (ballDistanceFromRobot - circlingDistance) * ballDistanceMultiplier);
            turnSpeed = (ballMinAngle + direction*angleCorrection) * turnSpeedMultiplier;
            setOmni(angle, velocity, turnSpeed);
        }

        // Make solid contact with the ball.
        // Robot drives forward some time.
        if (state == IMPROVE_CONTACT_WITH_BALL) {

            setThrowerSpeedIfBasketIsSeen();

            if(ballDistanceFromRobot > 20 || abs(ballMinAngle) > 15) {
                setState(FIND_BALL);
            }

            requestSensors();
            getSensorsResponse();
            if (getBallSocketState() == BallInSocket) {
                setState(THROW);
            }

            turnSpeed = 0;
            forwardSpeed = 30;

            setOmni(0, forwardSpeed, turnSpeed);
        }

        if (state == THROW) {
            qDebug() << "THROWING";
            setThrowerSpeedIfBasketIsSeen();

            requestSensors();
            getSensorsResponse();
            if (getBallSocketState() == BallNotInSocket) {
                qDebug() << "I HAVE THROWN! STARTING THE TIMER!";
                //very likely has thrown the ball
                timerSinceLastThrow.start();
                setState(FIND_BALL);
            }
            setOmni(0,10,0);
        }

        // Escapes black line by driving toward small goal
        if (state == ESCAPE_BLACK_LINE) {
            if (state != statePrev) {
                statePrev = state;
            }

            turnSpeed = -30; //-30
            angle = 0;
            forwardSpeed = 0;

            setOmni(angle, forwardSpeed, turnSpeed);

            if (goalTarget.area != 0 && goalTarget.area < 10000) {
                if (centerOnGoal(goalTarget, turnSpeed)) {
                    state = START_ESCAPE_BLACK_LINE;
                }
            }

            if (goalOwn.area != 0 && goalOwn.area < 10000) {
                if (centerOnGoal(goalOwn, turnSpeed)) {
                    state = START_ESCAPE_BLACK_LINE;
                }
            }
        }

        if (state == START_ESCAPE_BLACK_LINE) {
            //if (state != statePrev) {
                statePrev = state;

                angle = 0;
                forwardSpeed = 205; //100
                turnSpeed = 0;
                setOmni(angle, forwardSpeed, turnSpeed);
                msleep(300); //400

                getGoalAimDirection(state.state());

                //setOmni(0,0,0);
                //exit(0);

                state = FIND_BALL;
                //timer14.start();


            //}
            /*
            angle = 0;
            forwardSpeed = 110; //80
            turnSpeed = 0;

            setOmni(angle, forwardSpeed, turnSpeed);

            if (goalTarget.area == 0 && goalOwn.area == 0) {
                state = ESCAPE_BLACK_LINE;
            }


            if (timer14.elapsed() > 350) {
                state = FIND_BALL;
            }
            */
        }

        // Stateless actions
        if (state != WAITING_START) {
            // Avoid driving over black line
            if (analog[3] > BLACK_LINE || analog[4] > BLACK_LINE) {
                //state = ESCAPE_BLACK_LINE;
                state=FIND_BALL;
            }
        }

        //float goalDistanceFocus, goalAngle;
        //calculateGoal(&goalTarget, &goalDistanceFocus, &goalAngle);
        //qDebug() << "angle:" << goalAngle << ", distance:" << goalDistanceFocus << ", distanceH:" << goalTarget.distanceH << ", distanceVcm:" << goalTarget.distanceVCm << ", distanceHcm:" << goalTarget.distanceHCm;

        // Show image
        showCurrentImage();
    }
}

int Neve::round(float number) {
    return (number >= 0) ? ((int) (number + 0.5)) : ((int) (number - 0.5));
}

void Neve::squareDriveTest() {
    int speed = 75;
    int time = 1;

    setOmni(0, speed, 0);
    sleep(time);
    setOmni(90, speed, 0);
    sleep(time);
    setOmni(180, speed, 0);
    sleep(time);
    setOmni(270, speed, 0);
    sleep(time);
}

void Neve::resetRemoteCtrl() {
    setDcMotor(4, 1); //last night hack. 166 binded to reset remoteCommand in microcontroller
}

void Neve::printGoalInfo() {
    if (!infoMode) return;

    cvPutText(_img, "Target basket:", cvPoint(_img->width/2-425, _img->height-60), &(image->font), CV_RGB(255,255,255));
    cvPutText(_img, "Own basket:", cvPoint(_img->width/2-400, _img->height-30), &(image->font), CV_RGB(255,255,255));

    //in case of logical error - visible immediately
    if (goalTargetColor == BASKET_PURPLE) {
        cvPutText(_img, "PURPLE", cvPoint(_img->width/2-245, _img->height-60), &(image->font), CV_RGB(255,0,255));
    } else if (goalTargetColor = BASKET_BLUE) {
        cvPutText(_img, "BLUE", cvPoint(_img->width/2-245, _img->height-60), &(image->font), CV_RGB(30,144,255));
    }

    if (goalOwnColor == BASKET_PURPLE) {
        cvPutText(_img, "PURPLE", cvPoint(_img->width/2-245, _img->height-30), &(image->font), CV_RGB(255,0,255));
    } else if (goalOwnColor = BASKET_BLUE) {
        cvPutText(_img, "BLUE", cvPoint(_img->width/2-245, _img->height-30), &(image->font), CV_RGB(30,144,255));
    }

//    bool ok =  isCloseToGoal();
//    QString buffer;
//    buffer.sprintf("%i x=%i Y=%i width=%i height=%i area=%f %s", analog[2], goalTarget.rect.x, goalTarget.rect.y,
//            goalTarget.rect.width, goalTarget.rect.height, goalTarget.area,
//                   ok ? "TRUE" : "FALSE");
//    cvPutText(_img, buffer.toStdString().c_str(),
//              cvPoint(_img->width/2-245, _img->height-90), &(image->font), CV_RGB(64,220,244));

//    CvPoint leftUpper;
//    leftUpper.x = goalTarget.rect.x;
//    leftUpper.y = goalTarget.rect.y;

//    CvPoint rightLower;
//    rightLower.x = goalTarget.rect.x + goalTarget.rect.width;
//    rightLower.y = goalTarget.rect.y + goalTarget.rect.height;

//    cvDrawRect(_img, leftUpper, rightLower, CV_RGB(64,220,244), 5);


}

int Neve::centerOnGoal(Image::Object goal, int turnSpeed) {
    int goalLeft, goalRight, goalCenter;
    int center;
    int offset;

    offset = goal.rect.width / 20;  // 5%

    goalLeft = goal.rect.x + offset;
    goalRight = goal.rect.x + goal.rect.width - offset;
    goalCenter = goalLeft + (goal.rect.width / 2);
    center = _img->roi->width / 2;

    if (turnSpeed > 0) {
        if (center > goalLeft && center < goalCenter) return 1;
    } else {
        if (center < goalRight && center > goalCenter) return 1;
    }

    return 0;
}

void Neve::getGoalAimDirection(State state) {

    //Kilplase code below
    Image::Object frontVisibleTargetGoal = goalTarget;
    Image::Object frontVisibleOwnGoal = goalOwn;

    _img = image->getFrame(CAMERA_RIGHT);
    printCameraSource(CAMERA_RIGHT, true);
    image->process(fieldTop);
    findGoals();
    if (goalTarget.area != 0) {
        basketSpinDirection = 1;
        return;
    }
    if (goalOwn.area != 0) {
        basketSpinDirection = -1;
        return;
    }

    _img = image->getFrame(CAMERA_LEFT);
    printCameraSource(CAMERA_LEFT, true);
    image->process(fieldTop);
    findGoals();
    if (goalTarget.area != 0) {
        basketSpinDirection = -1;
        return;
    }
    if (goalOwn.area != 0) {
        basketSpinDirection = 1;
        return;
    }


    if (frontVisibleTargetGoal.area != 0) {
        if (frontVisibleTargetGoal.distanceH > 0) {
            basketSpinDirection = 1;
        } else {
            basketSpinDirection = -1;
        }
        return;
    }

    if (frontVisibleOwnGoal.area != 0) {
        if (frontVisibleOwnGoal.distanceH > 0) {
            basketSpinDirection = -1;
        } else {
            basketSpinDirection = 1;
        }
        return;
    }

    cvPutText(_img, "AIM GOAL LEFT! - saw nothing", cvPoint(_img->width/2, _img->height-30), &(image->font), CV_RGB(255,255,0));
    basketSpinDirection = -1;
}

void Neve::getObjectCalc(Image::Object* object, float* distance, float* angle, const char* cameraDevice) {
    float b, distanceY, distanceX;
    char str[50];

    // See calculation_formulas.odc
    // 212129.24887416 X ^ -1.5745456297
    distanceY = 212129.24887416 * pow((float) object->rect.y+object->rect.height, -1.5745456297);

    float tanHalfCameraAngle = 0.0;
    float increaseAngle = 0.0;
    if (cameraDevice == CAMERA_FRONT) {
        //tan(93.5/2)
        tanHalfCameraAngle = 1.063031307;
    } else if (cameraDevice == CAMERA_LEFT) {
        //tan(60/2)
        tanHalfCameraAngle = 0.577350269;
        increaseAngle = leftCamAngleDiff;
    } else if (cameraDevice == CAMERA_RIGHT) {
        //tan(60/2)
        tanHalfCameraAngle = 0.577350269;
        increaseAngle = rightCamAngleDiff;
    } else {
        qDebug() << "Mistake in getObjectCalc - image from unsupported camera. Killing botmaster.";
        exit(1);
    }

    b = tanHalfCameraAngle * distanceY;

    float magicConstant = 1.05;
    // resolution width 1280
    distanceX = (object->distanceH / 640.0) * b * magicConstant;

    *distance = sqrt(pow(fabs(distanceX), 2.0) + pow(fabs(distanceY), 2.0));

    if(cameraDevice == CAMERA_LEFT || cameraDevice == CAMERA_RIGHT) {
        //morphing front cam specific distance calculation into side cam
        *distance = (35.0769383755*log(*distance))-50.8714990166;
        //Finetuning (18 is ideally ok)
        if (*distance > 80) {
            *distance += 10;
        } else if (*distance > 25) {
            *distance -= 10;
        } else if (*distance <=25 && *distance >= 19) {
            *distance -= 3;
        } else if (*distance < 18 && *distance > 15) {
            *distance += 1;
        } else if (*distance < 15) {
            *distance += 3;
        }
    }

    *angle = (atan(distanceX / distanceY) * 180 / M_PI) + increaseAngle;

    object->distanceHCm = distanceX;
    object->distanceVCm = distanceY;
}

void Neve::getCvPointCalc(CvPoint* inputCvPoint, float* distance, float* angle, const char* cameraDevice) {
    float b, distanceY, distanceX;
    char str[50];

    distanceY = 47024.33 * pow((float) inputCvPoint->y, -1.33) * 1.4;

    float tanHalfCameraAngle = 0.0;
    float increaseAngle = 0.0;
    if (cameraDevice == CAMERA_FRONT) {
        //tan(93.5/2)
        tanHalfCameraAngle = 1.063031307;
    } else if (cameraDevice == CAMERA_LEFT) {
        //tan(60/2)
        tanHalfCameraAngle = 0.577350269;
        increaseAngle = leftCamAngleDiff;
    } else if (cameraDevice == CAMERA_RIGHT) {
        //tan(60/2)
        tanHalfCameraAngle = 0.577350269;
        increaseAngle = rightCamAngleDiff;
    } else {
        qDebug() << "Mistake in getObjectCalc - image from unsupported camera. Killing botmaster.";
        exit(1);
    }

    b = tanHalfCameraAngle * distanceY;

    int distanceH = inputCvPoint->x - (_img->roi->width / 2);
    // resolution width 1280
    distanceX = (distanceH / 640.0) * b;

    *distance = sqrt(pow(fabs(distanceX), 2.0) + pow(fabs(distanceY), 2.0));
    *angle = atan(distanceX / distanceY) * 180 / M_PI;
}

//Slowdown if near goal
void Neve::nearGoalSpeedLimiter(Image::Object* goalOwn, Image::Object* goalTarget,int* forwardSpeed) {
    bool slowDownNearGoalEnabled = true;
    int slowDownNearGoalSpeedLimit = 90;
    int slowDownNearGoalFromHeight = 200;

    if(goalOwn->rect.height > slowDownNearGoalFromHeight || goalTarget->rect.height > slowDownNearGoalFromHeight) {
        forwardSpeed = &slowDownNearGoalSpeedLimit;
    }
}

void Neve::instaTurn(int angle) {
    int angular = 255;
    if(angle < 0) {
        angular *= -1;
    }

    setOmni(0, 0, 0);
    msleep(75);
    int duration = 27.8555373569* (pow((float)abs(angle),0.4880905071));
    //qDebug() << "ANGLE:" << angle << ", DURATION:" << duration;
    setOmni(0, 0, angular);
    msleep(duration);

    setOmni(0, 0, 0);
    msleep(100);
    if(abs(angle)>90) {
        msleep(100);
    }

    if(abs(angle)>=160) {
        msleep(90);
    }
}

void Neve::findClosestBall(const char* cameraDevice) {
    ball.area = 0;
    ball.rect.y = 0;
    ballTotal = 0;

    float ballDistance = 0.0;
    float ballAngle = 0.0;   // degrees
    int maxBallAllowedDistance = 5000;
    ballDistanceFromRobot = FLT_MAX;
    int preferenceChoiceAmplitude = 22; //cm, goes both ways (-22 0 22), half value also used - paarisarv >P recommended
    for (int i = 0; i < image->found_objects->total; i++) {
        obj = (Image::Object*) cvGetSeqElem((image->found_objects), i);

        if (obj->type == BALL) {
            getObjectCalc(obj, &ballDistance, &ballAngle, cameraDevice);

            CvPoint minEdgeDistPoint;
            int d;
            int minD = INT_MAX;
            for(int i = 0; i < _img->width; i++){
                d = sqrt(pow(obj->center.x - i,2.0)+pow(obj->center.y - fieldTop[i],2.0));
                if(d < minD){
                    minD = d;
                    minEdgeDistPoint.y = fieldTop[i];
                    minEdgeDistPoint.x = i;
                }
            }

            cvLine(_img, obj->center, minEdgeDistPoint, CV_RGB(255,0,255), 1, 8, 0);

            float fieldTopDistance, fieldTopPointAngle;
            getCvPointCalc(&minEdgeDistPoint, &fieldTopDistance, &fieldTopPointAngle, cameraDevice);
            float ballDistanceFromEdgeDistPoint = fieldTopDistance - ballDistance;

            if(abs(ballAngle) > 40) {
                ballDistanceFromEdgeDistPoint *= 1.5;
            }

            if (abs(ballDistance) > maxBallAllowedDistance) {
                if (infoMode) {
                    sprintf(str, "Ignoring, out of bounds: %.1f", ballDistance);
                    cvPutText(_img, str, cvPoint(obj->center.x, obj->center.y + 15), &(image->font), CV_RGB(0,0,0));
                }
            }
            else if ((ballDistance < 100 && ballDistanceFromEdgeDistPoint < 40) /* || (ballDistance < 200 && ballDistanceFromEdgeDistPoint < 55) */) {
                if (infoMode) {
                    sprintf(str, "Ignoring, fed: %.1f", ballDistanceFromEdgeDistPoint);
                    cvPutText(_img, str, cvPoint(obj->center.x, obj->center.y + 15), &(image->font), CV_RGB(0,0,0));
                }
            }
            else {
                if (ballTotal==0  // if no balls selected
                        ||  // or
                        (   (   (ballDistanceFromRobot - ballDistance >= preferenceChoiceAmplitude)
                                ||  // or
                                (abs(ballDistance-ballDistanceFromRobot) < preferenceChoiceAmplitude)
                            )
                            &&  // and
                            abs(ballAngle) < abs(ballMinAngle)  // if current ball angle smaller than minimal ball angle
                        )
                   )

                {
                    printBallStats(obj, &ballAngle, &ballDistance, false);
                    ballDistanceFromRobot = ballDistance;
                    ballMinAngle = ballAngle;
                    minBallDistanceFromEdgePoint = ballDistanceFromEdgeDistPoint;  // kaugus v2ljaku servast (mustast joonest)
                    ball = *obj;
                }
            }
            if (abs(obj->distanceHCm) < 6) clearToKick = false;
            ballTotal++;
        }
    }

    if (ballTotal > 0) {
        printBallStats(&ball, &ballMinAngle, &ballDistanceFromRobot, true);
    }
}

void Neve::printBallStats(Image::Object * obj, float* angle, float* distance, bool targetBall) {

    CvScalar color = CV_RGB(70,70,70);
    if (targetBall) {
        color = CV_RGB(0, 0, 0);

        cvCircle(_img, obj->center, 3, CV_RGB(0, 0, 0), 5, CV_AA, 0);
        sprintf(str, "[Ball locked]");
        cvPutText(_img, str, cvPoint(obj->center.x-obj->rect.width/2-30, obj->rect.y - 10), &(image->font), CV_RGB(255,255,0));
    }

    //Print out stats
    if (obj->area != 0) {
        if (infoMode) {
            sprintf(str, "Angl: %.1f*", *angle);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 7), &(image->font), color);

            sprintf(str, "Dist: %.0fcm", *distance);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 27), &(image->font), color);

            sprintf(str, "Dis center Y: %dpx", obj->center.y);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 47), &(image->font), color);

            sprintf(str, "Dis lowest rect Y: %dpx", obj->rect.y+obj->rect.height);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 67), &(image->font), color);

            sprintf(str, "Dis center X: %dpx", obj->center.x);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 87), &(image->font), color);


        }
    }
}

void Neve::printBasketStats(Image::Object * obj, float* angle, float* distance) {
    if (obj->area != 0) {
        CvScalar color = CV_RGB(0, 0, 0);
        if (infoMode) {
            sprintf(str, "Angl: %.1f*", *angle);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 7), &(image->font), color);

            sprintf(str, "Dist: %.0fcm", *distance);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 27), &(image->font), color);

            sprintf(str, "Dis center Y: %dpx", obj->center.y);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 47), &(image->font), color);

            sprintf(str, "Dis lowest rect Y: %dpx", obj->rect.y+obj->rect.height);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 67), &(image->font), color);

            sprintf(str, "Dis center X: %dpx", obj->center.x);
            cvPutText(_img, str, cvPoint(obj->rect.x, obj->rect.y + 87), &(image->font), color);


        }
    }
}

void Neve::temporaryCrap() {
    int fieldTop[1280];
    //First image should be taken from side camera with big height, without that next frame from cental cam will fail
    _img = image->getFrame(CAMERA_RIGHT);
    image->process(fieldTop);
}

int infoModeBlinkerCount = 0;
void Neve::checkKeyPressAction() {
    infoModeBlinkerCount++;
    char selection = '0';
    selection = conf.keyS;
    switch(selection) {
        case 'l':
            getFrameFromCameraNr = CAMERA_LEFT;
            break;
        case 'f':
            getFrameFromCameraNr = CAMERA_FRONT;
            break;
        case 'r':
            getFrameFromCameraNr = CAMERA_RIGHT;
            break;
        case 'p':
            goalTargetColor = BASKET_PURPLE;
            goalOwnColor = BASKET_BLUE;
            break;
        case 'b':
            goalTargetColor = BASKET_BLUE;
            goalOwnColor = BASKET_PURPLE;
            break;
        case 'g': {
            int oldTargetColor = goalTargetColor;
            goalTargetColor = goalOwnColor;
            goalOwnColor = oldTargetColor;
            break;
        }
        case 'x': {
            conf.setSendCmdEnabled(1);
            setOmni(0,0,0);
            setDcMotor(3,0);
            exit(2);
            break;
        }
        case 'q': {
            qDebug() << "Pressed q and setting mustRun to true!";
            mustRun = true;
            break;
        }
        case 'i':
            infoMode = !infoMode;
            break;
        default:
            break;
    }

    if (infoMode && infoModeBlinkerCount < 16) {
        sprintf(str, "@InfoMode!");
        cvPutText(_img, str, cvPoint(_img->width-150, 30), &(image->font), CV_RGB(255,0,0));
    }

    if (infoModeBlinkerCount == 32) infoModeBlinkerCount = 0;
    conf.keyS = NULL;
}

float Neve::getThrowAngleCorrectionRelativeToField() {
    _img = image->getFrame(CAMERA_RIGHT);
    printCameraSource(CAMERA_RIGHT, true);
    image->process(fieldTop);
    showCurrentImage();
    float fieldPointsOnTheRight = image->totalFieldPoints;

    _img = image->getFrame(CAMERA_LEFT);
    printCameraSource(CAMERA_LEFT, true);
    image->process(fieldTop);
    showCurrentImage();
    float fieldAreaOnTheLeft = image->totalFieldPoints;

//    qDebug() << fieldAreaOnTheLeft << "vs" << fieldPointsOnTheRight;
    float ratio = abs(1.0-(fieldAreaOnTheLeft/fieldPointsOnTheRight));
//    qDebug() << ratio;

    // more or less equal
    if (ratio < 0.2 || ratio > 0.5) {
        return 0.0;
    }

    float offsetMultiplier = 1.0;
    if (fieldAreaOnTheLeft < fieldPointsOnTheRight) {
        offsetMultiplier *= -1.0;
    }

    return offsetMultiplier;
}

void Neve::decideInstaTurnToGetBall(bool oneEightyIfNowhereFound) {

    if (oneEightyIfNowhereFound == FALSE) {
        _img = image->getFrame(CAMERA_FRONT);
        printCameraSource(CAMERA_FRONT, true);
        image->process(fieldTop);
        findClosestBall(CAMERA_FRONT);
        findGoals();
        showCurrentImage();
    }

    int ballsAtFront = ballTotal;
    int frontBallAngle = ballMinAngle;
    if(ballDistanceFromRobot < 10) return; //take front ball when distance is very small

    _img = image->getFrame(CAMERA_RIGHT);
    printCameraSource(CAMERA_RIGHT, true);
    image->process(fieldTop);
    findClosestBall(CAMERA_RIGHT);
    int ballsOnTheRight = ballTotal;
    int rightBallAngle = ballMinAngle;
    int rightBallDistance = ballDistanceFromRobot;
    //showCurrentImage();
    //msleep(750);

    _img = image->getFrame(CAMERA_LEFT);
    printCameraSource(CAMERA_LEFT, true);
    image->process(fieldTop);
    findClosestBall(CAMERA_LEFT);
    int ballsOnTheLeft = ballTotal;
    int leftBallAngle = ballMinAngle;
    int leftBallDistance = ballDistanceFromRobot;
    //showCurrentImage();
    //msleep(750);



    if (ballsOnTheLeft > 0 || ballsOnTheRight >0) {
            if (ballsOnTheLeft >= ballsOnTheRight) {

                if ((-60 < leftBallAngle && leftBallAngle < -80) && (25 < leftBallDistance && leftBallDistance < 40) && goalTarget.area > 0) {
                    sideStep(-95, leftBallDistance);
                } else {
                    instaTurn(leftBallAngle);
                }
            } else {
                if ((60 < rightBallAngle && rightBallAngle < 80) && (25 < rightBallDistance && rightBallDistance < 40) && goalTarget.area > 0) {
                    sideStep(95, rightBallDistance);
                } else {
                    instaTurn(rightBallAngle);
                }
            }
    } else {
        if (ballsAtFront > 0) {
            instaTurn(frontBallAngle);
        } else if(oneEightyIfNowhereFound) {
            instaTurn(180);
            decideInstaTurnToGetBall(false);
        }
    }
}

void Neve::printCameraSource(const char* camera, bool blind) {
    if(!infoMode) return;

    CvScalar color = CV_RGB(57,255,20);
    if (blind) {
        color = CV_RGB(255,0,0);
    }

    if (camera == CAMERA_FRONT) {
        sprintf(str, "FRONT CAM");
//        qDebug() << "FRONT CAM";
    } else if (camera == CAMERA_LEFT) {
        sprintf(str, "LEFT CAM");
        qDebug() << "LEFT CAM";
    } else if (camera == CAMERA_RIGHT) {
        sprintf(str, "RIGHT CAM");
        qDebug() << "RIGHT CAM";
    } else {
        sprintf(str, "UNKNOWN CAM");
    }
    cvPutText(_img, str, cvPoint(_img->width/2, 30), &(image->font), color);
}

void Neve::showCurrentImage() {
    if(displayImageRefresh){
        if (infoMode) {
            sprintf(str, "State: %s", state.toString());
            cvPutText(_img, str, cvPoint(_img->width/2-40, _img->height-30), &(image->font), CV_RGB(57,255,40));

            if (!listeningToRemote) {
                cvPutText(_img, "Not listening to remote!", cvPoint(_img->width/2+290, _img->height-30), &(image->font), CV_RGB(255,0,0));
            } else {
                cvPutText(_img, "Listening to remote!", cvPoint(_img->width/2+290, _img->height-30), &(image->font), CV_RGB(0,255,0));
            }
        }
        view->show(_img);
    } else {
        view->show(0);
    }
}

void Neve::setState(State newState) {
    if (newState == DISTRONIC_TURN) {
        allowedtoGetThrowAngleCorrectionRelativeToField = true;
        if (state == IMPROVE_CONTACT_WITH_BALL || state == FIND_BALL) {
            getGoalAimDirection(state.state());
        }
        timerUnableCenterBallAndBasket.start();
    }
    state = newState;
}

void Neve::findGoals() {
    goalTarget.area = 0;
    goalOwn.area = 0;

    CvMemStorage* potentialTargetGoalsStorage = cvCreateMemStorage(0);
    CvMemStorage* potentialOwnGoalsStorage = cvCreateMemStorage(0);
    CvSeq* potentialTargetGoals = cvCreateSeq(0, sizeof(CvSeq), sizeof(Image::Object), potentialTargetGoalsStorage);
    CvSeq* potentialOwnGoals = cvCreateSeq(0, sizeof(CvSeq), sizeof(Image::Object), potentialOwnGoalsStorage);

    for (int i = 0; i < image->found_objects->total; i++) {
        obj = (Image::Object*) cvGetSeqElem((image->found_objects), i);

        if (obj->type == goalTargetColor) {
            cvSeqPush(potentialTargetGoals, obj);
        }

        if (obj->type == goalOwnColor) {
            cvSeqPush(potentialOwnGoals, obj);
        }
    }

// TODO remove and commit "opponent goal color marking detection"
//    Image::Object * potentialTargetGoal; //todo make global
//    Image::Object * potentialOwnGoal; //todo make global
//    for (int targetIdx = 0; targetIdx < potentialTargetGoals->total; targetIdx++) {
//        potentialTargetGoal = (Image::Object*) cvGetSeqElem(potentialTargetGoals, targetIdx);

//        for (int ownIdx = 0; ownIdx < potentialOwnGoals->total; ownIdx++) {
//            potentialOwnGoal = (Image::Object*) cvGetSeqElem(potentialOwnGoals, ownIdx);

//            if(potentialOwnGoal->center.x > potentialTargetGoal->rect.x && potentialOwnGoal->center.x < (potentialTargetGoal->rect.x + potentialTargetGoal->rect.width)) {
//                try {
//                    if (potentialOwnGoals->total>0 && ownIdx >= 0) { //fix for opencv crashes
//                        cvSeqRemove(potentialOwnGoals, ownIdx);
//                        ownIdx--;
//                    }
//                    if (potentialTargetGoals->total>0 && targetIdx >= 0) { //fix for opencv crashes
//                        cvSeqRemove(potentialTargetGoals, targetIdx);
//                        targetIdx--;
//                    }
//                } catch (int e) {
//                    qDebug() << "Goal discovery fault - ignored. OwnIdx:" << ownIdx << " TargetIdx:" << targetIdx;
//                }
//            }
//        }
//    }

    //Find biggest target goal
    for (int i = 0; i < potentialTargetGoals->total; i++) {
        obj = (Image::Object*) cvGetSeqElem(potentialTargetGoals, i);

        if (obj->area > goalTarget.area) {
            goalTarget = *obj;
        }
    }

// Uncomment to print basket stats
    float basketDistance = 0.0;
    float basketAngle = 0.0;
    getObjectCalc(&goalTarget, &basketDistance, &basketAngle, CAMERA_FRONT);
    printBasketStats(&goalTarget, &basketAngle, &basketDistance);

    //Find biggest own goal
    for (int i = 0; i < potentialOwnGoals->total; i++) {
        obj = (Image::Object*) cvGetSeqElem(potentialOwnGoals, i);

        if (obj->area > goalTarget.area) {
            goalOwn = *obj;
        }
    }
}

void Neve::readRobotAndFieldSwitches() {
    if (digital[4] == 0) {
        selectedRobot = QChar(RC_SIG_A);
    } else if (digital[4] == 1) {
        selectedRobot = QChar(RC_SIG_B);
    }

    if (digital[5] == 0) {
        selectedField = RC_SIG_A;
    } else if (digital[5] == 1) {
        selectedField = RC_SIG_B;
    }
}

//Remote control
void Neve::readRemoteCtrl() {
    if (listeningToRemote == false) return;

    if (isRCSignalTargetingCommand(RC_SIG_START) == true) {
        qDebug() << "setting mustRun to true";
        mustRun = true;
    }
    if (isRCSignalTargetingCommand(RC_SIG_STOP) == true) {
        qDebug() << "stop signal received, falling to wait loop";
        setOmni(0,0,0);
        waitActionSignalFromRemoteCtrl(RC_SIG_START); // TODO separate state?
    }
}

void Neve::waitActionSignalFromRemoteCtrl(char actionStart) {
    while(1) {
        requestSensors();
        getSensorsResponse();
        checkKeyPressAction();
        showCurrentImage();
        if (isRCSignalTargetingCommand(actionStart) == true) {
            bool notSureIfBallInSocket = getBallSocketState() == BallNotInSocket || getBallSocketState() == BallAlmostInSocket;
            if (notSureIfBallInSocket) {
                decideInstaTurnToGetBall(true);
            }
            inCurrentStateTime.start();
            break;
        }

    }
}

bool Neve::isRCSignalTargetingCommand(char action) { // TODO timer to reset last command to 0 after few seconds
    int currentRCSignal = analog[1];
    if (currentRCSignal != 0) {
//        qDebug() << lastTargetedRCCommand << "<- Last | Current:" << currentRCSignal;
        if (currentRCSignal != lastTargetedRCCommand) {
            QString currentSignalQStr = QString::number(currentRCSignal);
//            qDebug() << "New targeted remote control signal:" << currentSignalQStr << selectedRobot;
            if (
                selectedField == currentSignalQStr[RC_FIELD]
                  && (currentSignalQStr[RC_ROBOT] == RC_SIG_ALL_ROBOTS
                        ||  currentSignalQStr[RC_ROBOT] == selectedRobot)
                  && currentSignalQStr[RC_ACTION] == action
               )
            {
                lastTargetedRCCommand = currentRCSignal;
                return true;
            }
            if (
                selectedField == currentSignalQStr[RC_FIELD]
                  && (currentSignalQStr[RC_ROBOT] == RC_SIG_ALL_ROBOTS
                        ||  currentSignalQStr[RC_ROBOT] == selectedRobot)
                  && currentSignalQStr[RC_ACTION] == RC_SIG_PING
               )
            {
                lastTargetedRCCommand = currentRCSignal;
                qDebug() << "PINGED! " << currentSignalQStr << selectedRobot;
                return false;
            }
        }
    }
    return false;
}

void Neve::sideStep(int angle, int distance) {
    if(distance < 0 || distance > 50) return;

    int maxSideSpeed = 130;
    setOmni(0, 0, 0);

    setOmni(angle, maxSideSpeed, 0);
    //int duration = 72.9414522784*pow(distance,0.5532342559); //power
    int duration = 9.52*distance + 181.5111111111; //linear
    msleep(duration);
    setOmni(0, 0, 0);
}

bool Neve::decideIfAvoidCloseToGoal() {
    // Avoid driving into goal
//    qDebug() << QTime::currentTime().toString("hh:mm:ss.zzz")
//            << " analog[2]:" << analog[2]
//             << " goalTarget.area:" << goalTarget.area
//             << " goalTarget.rect.height"<< goalTarget.rect.height
//             << " goalOwn.area:" << goalOwn.area
//             << " goalOwn.rect.height"<< goalOwn.rect.height;
    if (isCloseToGoal()) {
        state = ESCAPE_GOAL_TOO_CLOSE;
        return true;
    }
    return false;
}

void Neve::drawCircleOnGoal() {
    if (goalTarget.area != 0) {
        cvCircle(_img, cvPoint(goalTarget.rect.x + (goalTarget.rect.width / 2), goalTarget.rect.y+15), 3, CV_RGB(255, 0, 0), 15, CV_AA, 0);
        sprintf(str, "[Target locked]");
        cvPutText(_img, str, cvPoint(goalTarget.center.x-75, goalTarget.center.y), &(image->font), CV_RGB(254,255,234));
    }
}


void Neve::distancePreserveTurn(int angle) {
    timerLastDistronicTurn.start();
    int turnSpeed = 110;

    int originalAngle = angle;
    if(angle<0){
        angle = -angle;
        turnSpeed = -turnSpeed;
    }
    angle -= 360*int(angle/360);

//    if(angle>180){
//        angle = angle-180;
//    }

    int duration = 7.1369346734*angle+30.1344221106; //175.9303173589 * exp(0.0155098758 * angle);

    setDcMotor(0, 0);
    setDcMotor(1, 0);
    setDcMotor(2, turnSpeed);
    qDebug() << "ORIGINAL ANGLE:" << originalAngle << ", TURN ANGLE:" << angle << ", DURATION:" << duration << ", turning" << (turnSpeed > 0 ? "right" : "left");
    msleep(duration);

    setOmni(0, 0, 0);
    msleep(10);
}

Neve::BallState Neve::getBallSocketState() const {
    if (analog[0] > BEAM_VALUE_BALL_IN_SOCKET) {
        return BallInSocket;
    } else {
        return BallNotInSocket;
    }
}


bool Neve::isCloseToGoal() const
{
    bool ownGoalLooksClose = //goalOwn.area >= 150000 || goalOwn.rect.height >= 475;
        goalOwn.area >= 250000
        || (goalOwn.rect.height >= 420 && goalOwn.rect.y <= 2)
        || goalOwn.rect.width >= 1200;


    bool targetGoalLooksClose = //goalTarget.area >= 150000 || goalTarget.rect.height >= 475);
        goalTarget.area >= 250000
        || (goalTarget.rect.height >= 420 && goalTarget.rect.y <= 2)
        || goalTarget.rect.width >= 1200;


    return analog[2] > GOAL_CLOSE && (ownGoalLooksClose || targetGoalLooksClose);

}

void Neve::setThrowerSpeedIfBasketIsSeen() {
    if (goalTarget.area > 0) {
        float basketAngle = 0.0;
        float basketDistance = 0.0;
        getObjectCalc(&goalTarget, &basketDistance, &basketAngle, getFrameFromCameraNr);
        float throwerPwm = 0.1433074527 * basketDistance + 45.6274396764 + 1.0;
        if (basketDistance > 135) {
            throwerPwm += 2.0;
        }
        if (basketDistance > 150) {
            throwerPwm += 1.0;
        }
        setDcMotor(3, throwerPwm);
    }
}
