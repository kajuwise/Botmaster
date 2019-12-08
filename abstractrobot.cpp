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



/*!
    \fn AbstractRobot::getServo(int servo)
 */
int AbstractRobot::getServo(int servo)
{
  return getServo(servo, 0);
}
int AbstractRobot::getServo(int servo, int nr)
{
  if(servo<0||servo>9) fprintf(stderr,"ERROR: Servo value %d out of range\n",servo);
	return com->readSerial((char)(servo+1), nr);
}

/*!
	\fn AbstractRobot::getAnalog(int analog_no)
 */
int AbstractRobot::getAnalog(int analog_no)
{
	return getAnalog(analog_no, 0);
}
int AbstractRobot::getAnalog(int analog_no, int nr)
{
	return com->readSerial(20 + analog_no, nr);
}

int AbstractRobot::getAnalogs()
{
   return getAnalogs(0);
}
int AbstractRobot::getAnalogs(int nr)
{
   return com->readSerialMulti(44, analog, digital, nr);
}

int AbstractRobot::getSensors()
{
   return getSensors(0);
}
int AbstractRobot::getSensors(int nr)
{
   return com->readSerialMulti(44, analog, digital, nr);
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
   return com->serialMultiResponse(analog, digital, nr);
}

int AbstractRobot::getAnalogsAvg()
{
   return getAnalogsAvg(0);
}
int AbstractRobot::getAnalogsAvg(int nr)
{
   return com->readSerialMulti(42, analog, digital, nr);
}




/*!
    \fn AbstractRobot::setServo(int servo, int speed)
 */
void AbstractRobot::setServo(int servo, unsigned char speed)
{
  setServo(servo, speed, 0);
}
void AbstractRobot::setServo(int servo, unsigned char speed, int nr)
{
  if(servo<0||servo>9) fprintf(stderr,"ERROR: Servo value %d out of range\n",servo);
  com->sendCommand(129 + servo, speed, nr);
}

void AbstractRobot::setServoOff()
{
  setServoOff(0);
}
void AbstractRobot::setServoOff(int nr)
{
  com->sendCommand(231, 2, nr);
}

void AbstractRobot::setServoOn()
{
  setServoOn(0);
}
void AbstractRobot::setServoOn(int nr)
{
  com->sendCommand(231, 1, nr);
}

// void AbstractRobot::saveServos()
// {
//     com.sendCommand(228, 0);
// }

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

/**
 233 in protocol will send command to PORT B
*/
void AbstractRobot::setDigital(unsigned char pin_nr)
{
	setDigital(pin_nr, 0);
}
void AbstractRobot::setDigital(unsigned char pin_nr, int nr)
{
	if(pin_nr > 7) {
	   fprintf(stderr,"ERROR: Digital nr %d out of range\n",pin_nr);
	  return;
	}
//      fprintf(stderr,"1 Digital bitmaskvalue %d\n", digitalBitMaskValue);
	 digitalBitMaskValue |=  (1 << pin_nr);
	 com->sendCommand(233, digitalBitMaskValue, nr);

//     fprintf(stderr,"2 Digital bitmaskvalue %d\n", digitalBitMaskValue);
}

void AbstractRobot::clearDigital(unsigned char pin_nr)
{
	clearDigital(pin_nr, 0);
}
void AbstractRobot::clearDigital(unsigned char pin_nr, int nr)
{
	if(pin_nr > 7) {
	   fprintf(stderr,"ERROR: Digital nr %d out of range\n",pin_nr);
	  return;
	}
//      fprintf(stderr,"1 Digital bitmaskvalue %d\n", digitalBitMaskValue);
	 digitalBitMaskValue &= ~(1 << pin_nr);
	 com->sendCommand(233, digitalBitMaskValue, nr);
//     fprintf(stderr,"2 Digital bitmaskvalue %d\n", digitalBitMaskValue);
}

/*!
    \fn AbstractRobot::sendImage()
 */
void AbstractRobot::sendImage()
{
  //thread.setImg(_img);
  //thread.start(QThread::TimeCriticalPriority);
}
