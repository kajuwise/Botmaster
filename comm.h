/**
 * @file comm.h
 * @brief C++ Interface: comm
 * 
 * @details This file holds the Comm class which creates communication channel to microcontroller, 
	polls events from controller, realizes communication protocol.
 * 
 * Copyright: See COPYING file that comes with this distribution
 * 
 * @author Valdur Kaldvee (C) 2008
 * @author Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
 * @author Mauno Pihelgas <mpihelga@itcollege.ee>, (C) 2010
 */
#ifndef COMM_H
#define COMM_H
#define MAX_DEVICES 16
#include <errno.h>
#include <termios.h>
#include <unistd.h> 
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <unistd.h>  
#include <fcntl.h>  
#include <errno.h> 
#include <termios.h>
#include <QMutex>
#include <QWaitCondition>

#include "config.h"

#define MSG_MC_DATA_NR_ELEMENTS 16
#define MSG_MC_DATA_HEADER '\a'
#define MSG_MC_DATA_CHECKSUM_DUMMY 1337


/**
 * @brief This class creates communication channel to microcontroller, polls events from controller, realizes communication protocol.
 * @author Valdur Kaldvee
 * @author Margus Ernits
 * @author Mauno Pihelgas
 * @author Erik Kaju
 */
class Comm{
public:
//     Comm();

    ~Comm();
    int fd; ///< File descriptor for the port.
	int fds[MAX_DEVICES]; ///< Array of file descriptors for the serial ports.
    
    /**
     * @brief Opens the serial device.
	 * @deprecated This function is deprecated. Use openSerial(const char *, const int) instead.
     * @param modemDevice - Serial device address
     * @return File descriptor for the port (fd)
     */
    int openSerial(const char *); 
	
	/**
     * @brief Opens the numbered (nr) serial device.
     * @param modemDevice - Serial device address (Not in use!)
	 * @param nr - Serial device number
     * @return File descriptor for the port (fd)
     */
	int openSerial(const char *, const int);

    /**
     * @brief Sends commands to the serial device.
	 * @deprecated This function is deprecated. Use sendCommand(char, int, const int) instead.
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @param value - digitalBitMaskValue
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
     
     //TODO viita dokumentatsioonile robo wikis.
    int sendCommand(char, int);
	
	/**
     * @brief Sends commands to the numbered (nr) serial device.
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @param value - digitalBitMaskValue
	 * @param nr - Serial device number
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
    int sendCommand(char, int, const int);

    /**
    * @brief Sends commands to the numbered (nr) serial device.
    * @param addr - ServoBasic address (multiple addresses on one controller)
    * @param value - digitalBitMaskValue
     * @param nr - Serial device number
    * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
    */
    int sendCommand3(int, int, int, int);
    
    /**
     * @brief Reads and returns an integer value from the serial device.
	 * @deprecated This function is deprecated. Use readSerial(char, const int) instead.
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @return Returns an integer value from the serial device.
     */
    int readSerial(char);
	
	/**
     * @brief Reads and returns an integer value from the numbered (nr) serial device.
     * @param addr - ServoBasic address (multiple addresses on one controller)
	 * @param nr - Serial device number
     * @return Returns an integer value from the serial device.
     */
    int readSerial(char, const int);

    /**
     * @brief Reads multiple values from the serial device and saves them in two arrays (analog & digital).
	 * @deprecated This function is deprecated. Use readSerialMulti(char, int *, int *, const int) instead.
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @param microControllerData - pointer to the array of microcontroller values
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
    int readSerialMulti(char, int *);
	
    /**
     * @brief Reads multiple values from the numbered (nr) serial device and saves them in two arrays (analog & digital).
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @param microControllerData - pointer to the array of microcontroller values
	 * @param nr - Serial device number
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
    int readSerialMulti(char, int *, const int);

    /**
     * @brief Requests a read start for multiple values from the serial device.
     * @deprecated This function is deprecated. Use readSerialMulti(char, const int) instead.
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
    int requestSerialMulti(char);

    /**
     * @brief Requests a read start for multiple values from the numbered (nr) serial device.
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @param analogs - pointer to the array of 'analog' values
     * @param digitals - pointer to the array of 'digital' values
	 * @param nr - Serial device number
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
    int requestSerialMulti(char, const int);

    /**
     * @brief Reads multiple values from the numbered (nr) serial device and saves them in two arrays (analog & digital).
     * @param addr - ServoBasic address (multiple addresses on one controller)
     * @param analogs - pointer to the array of 'analog' values
     * @param digitals - pointer to the array of 'digital' values
	 * @param nr - Serial device number
     * @return 0 | 1 (0 = SUCCESS, 1 = FAILURE)
     */
    int serialMultiResponse(int *, int);

   static Comm  & getComm()
    {
        static Comm comm;
        return  comm;
    }
    
  protected:
    Config * conf;    

private:
    QMutex mutex; //TODO Erinevatel serialitel võiks olla erinev "mutex", et saaks samaaegselt erinevatesse serialitesse kirjutada / lugeda.
    QWaitCondition isLocked;
    Comm();

    Comm(Comm const&);
    Comm& operator=(Comm const&);

    uint8_t fletcherChecksum(int *, int);
    
    /**
     * @brief Closes and tries to reopen the serial device.
	 * @deprecated This function is deprecated. Use reopen(const char *, const int) instead.
     * @param device - Serial device address
     */
    inline void reopen(const char *);
	
	/**
     * @brief Closes and tries to reopen the numbered (nr) serial device.
     * @param device - Serial device address
	 * @param nr - Serial device number
     */
    inline void reopen(const char *, const int); 

    int len;
    char buf[255];
};

#endif
