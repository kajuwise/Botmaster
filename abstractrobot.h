/**
 * @file abstractrobot.h
 * @brief C++ Interface: abstractrobot
 * 
 * @details Holds the abstract class AbstractRobot which all the robots inherit.
 * 
 * @author Margus Ernits
 * @author Mauno Pihelgas
 * @author Erik Kaju
 */
#ifndef ABSTRACTROBOT_H
#define ABSTRACTROBOT_H
//#include "log.h"
#include "camera.h"
#include "config.h"
#include "image.h"
#include "comm.h"
#include "view.h"

//todo move to abstract neve - once we have it
#define MC_DATA_BALL_SENSOR 0
#define MC_DATA_REMOTE_SIGNAL 1
#define MC_DATA_ROBOT_SELECTOR 8
#define MC_DATA_FIELD_SELECTOR 9
#define MC_DATA_START_BUTTON 10
#define MC_DATA_EZ_RC 11
#define MC_DATA_MYXA_RPM 12
#define MC_DATA_LIDAR_DISTANCE 13
#define MC_DATA_ROLLER_STATE 14

/**
 * @brief This abstract class holds functions that are common to all robots.
 * @author Margus Ernits, Mauno Pihelgas, Erik Kaju
 */

//TODO Aadresside ja kommunikatsiooni vaheline vastavustabel
class AbstractRobot{
public:
    AbstractRobot();

    ~AbstractRobot();
    
	/// @brief Virtual function which can be over-ridden within an inhereting class (individual robots).
    virtual void go()=0;


	/// @deprecated This function is deprecated. Use requestSensors(int nr) instead.
    int requestSensors();

	/**
	 * @brief Sends a read request for all sensor values from the numbered (@a nr) serial device
	 * @param nr - Serial device number
	 * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
	 * @see Equivalent to getAnalogs(int nr)
	 */
    int requestSensors(int nr);


	/// @deprecated This function is deprecated. Use getSensorsResponse(int nr) instead.
    int getSensorsResponse();

	/**
	 * @brief Writes serial device responce into two arrays: analog[8] and digital[8].
	 * @param nr - Serial device number
	 * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
	 * @see Equivalent to getAnalogs(int nr)
	 */
    int getSensorsResponse(int nr);
	
	/// @deprecated This function is deprecated. Use setDcMotor(int motor, int speed, int nr) instead.
    void setDcMotor(int motor, int speed);
	/**
	 * @brief Sends the new @a speed value of a given @a motor to the numbered (@a nr) serial device.
	 * @param motor - Number of the DC motor on the serial device (valid input: 0-4)
	 * @param speed - Motor speed value
	 * @param nr - Serial device number
	 * @see For more detailed information see: http://robot.itcollege.ee/wiki/index.php/ServoBasic_kontroller
	 */
    void setDcMotor(int motor, int speed, int nr);
    void setOmni(int dirDeg, int velocityBody, int velocityAngular);
    void setThrowerCommand(int requestedRpm, int precision);

    int microcontrollerData[16]; ///< Holds all the analog sensor values retrieved by Comm::readSerialMulti(char addr, int * microcontrollerData, int nr)
    

protected:
  //Camera camera;
  Comm *  com;
  Image * image;
  View * view;
public:
  IplImage* _img;
private:  
};

#endif
