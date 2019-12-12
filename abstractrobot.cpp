/**
 * @file abstractrobot.cpp
 * @brief C++ Implementation: abstractrobot
 * 
 * @details Holds the abstract class AbstractRobot which all the robots inherit.
 * 
 * Copyright: See COPYING file that comes with this distribution
 * 
 * @author Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
 * @author Mauno Pihelgas <mpihelga@itcollege.ee>, (C) 2010
 */
#include "abstractrobot.h"

#define MOTOR_ZERO_CMD 162
#define MOTION_CMD 300
#define THROWER_CMD 400

AbstractRobot::AbstractRobot()
{


  Image & imageobject = Image::getImage();
  image = & imageobject;
  View & viewobject = View::getView();
  view = & viewobject;
  Comm & _com = Comm::getComm();
  com = & _com;
  com->openSerial("");
  unsigned char digitalBitMaskValue=0;

}


AbstractRobot::~AbstractRobot()
{
}

int AbstractRobot::requestSensors()
{
   return requestSensors(0);
}
int AbstractRobot::requestSensors(int nr)
{
   return com->requestSerialMulti(44, nr);
}

int AbstractRobot::getSensorsResponse()
{
   return getSensorsResponse(0);
}

int AbstractRobot::getSensorsResponse(int nr)
{
   return com->serialMultiResponse(microcontrollerData, nr);
}

void AbstractRobot::setDcMotor(int motor, int speed)
{
  setDcMotor(motor, speed, 0);
}
void AbstractRobot::setDcMotor(int motor, int speed, int nr)
{
  if((motor<0 || motor>5) && (motor != 100 /* remote */) ) {
	fprintf(stderr,"ERROR: DC motor nr %d out of range\n",motor);
	return;
  }
  com->sendCommand(MOTOR_ZERO_CMD + motor, speed, nr);
}

void AbstractRobot::setOmni(int dirDeg, int velocityBody, int velocityAngular)
{
  com->sendCommand3(MOTION_CMD, dirDeg, velocityBody, velocityAngular);
}

void AbstractRobot::setThrowerCommand(int requestedRpm, int precision)
{
  com->sendCommand3(THROWER_CMD, requestedRpm, precision, 0);
}
