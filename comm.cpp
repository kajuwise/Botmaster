/**
 * @file comm.cpp
 * @brief C++ Implementation: comm
 * 
 * @details This file holds the Comm class implementation which creates communication channel to microcontroller, 
	polls events from controller, realizes communication protocol.
 * 
 * Copyright: See COPYING file that comes with this distribution
 * 
 * @author Valdur Kaldvee (C) 2008
 * @author Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
 * @author Mauno Pihelgas <mpihelga@itcollege.ee>, (C) 2010
 * @author Erik Kaju
 */
#include "comm.h"
#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include <QDebug>
#include <iterator>

Comm::Comm()
{
   Config & temp =Config::getConfig();
   conf = & temp;
   for(int i=0; i<MAX_DEVICES; i++) {
	   fds[i]=-1;
   }
}


Comm::~Comm()
{
  close(fd);
}



/*!
    \fn Comm::openSerial()
 */
int Comm::openSerial(const char * modemDevice)
{
	return openSerial(modemDevice, 0);
}

/*!
    \fn Comm::openSerial()
 */
int Comm::openSerial(const char * modemDevice, const int nr)
{
    char serialDevice[150] = "/dev/ttyS0";
    serialDevice[11] = nr + '0';

	struct termios options;
    std::cout<<"Serial Open "<<serialDevice<<std::endl;
  /* open the port */
// if (modemDevice!=NULL){
   
   //TODO: getmodemDevice peaks ka nr ette saama
   
  fds[nr] = open(serialDevice, O_RDWR | O_NOCTTY | O_NDELAY); 
   
   /*if(0 && conf->getmodemDevice()!= NULL){
     fds[nr] = open(conf->getmodemDevice(), O_RDWR | O_NOCTTY | O_NDELAY);
   }  
  else{
    fds[nr] = open("/dev/usbserial", O_RDWR | O_NOCTTY | O_NDELAY);
  }*/
 

  if (fds[nr] == -1) {
        //Could not open the port.
    perror("open_port: Unable to open device - ");
  }
  else fcntl(fds[nr], F_SETFL, 0);

  /* get the current options */
  tcgetattr(fds[nr], &options);

  cfsetispeed(&options, B115200);
  cfsetospeed(&options, B115200);
    
  /* 8N1 */
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;
    
  /* set raw input, 1 second timeout */
  options.c_cflag     |= (CLOCAL | CREAD);
  options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG);
  options.c_oflag     &= ~OPOST;
  options.c_cc[VMIN]  = 0;
  options.c_cc[VTIME] = 10;

  /* set the options */
  tcsetattr(fds[nr], TCSANOW, &options);
    
	fd=fds[nr];
  return (fds[nr]);
}

/*!
    \fn Comm::sendCommand()
    @Author Valdur Kaldvee
 */
int Comm::sendCommand(char addr, int value)
{
	return sendCommand(addr, value, 0);
}
 
int Comm::sendCommand(char addr, int value, int nr)
{
//   isLocked.wait(&mutex);
  QMutexLocker locker(&mutex);
  if((conf->getSendCmdEnabled())==0) return 1;
    int len;
    unsigned char tmp = 0;
    char buf[50];

    buf[49] = 0;

    sprintf(buf, "\r");    
    len = write(fds[nr], buf, strlen(buf));
    tcflush(fds[nr], TCIFLUSH);

    if (len < 0) {
      perror("sendCommand write error - ");
      Comm::reopen("", nr);
      return 1;
    }

    len = 0;
    tmp = 128 | addr;
    snprintf(buf, sizeof buf, "%d\r%d\r", tmp, value);
    if (strlen(buf) < 50) {
      len = write(fds[nr], buf, strlen(buf));
      tcflush(fds[nr], TCIFLUSH);
    }

    if (len < 0) {
      perror("sendCommand write error - ");
      Comm::reopen("", nr);
      return 1;
    }
    
    return 0;
}

int Comm::sendCommand3(int addr, int value1, int value2, int value3)
{
  //   isLocked.wait(&mutex);
  QMutexLocker locker(&mutex);
  if((conf->getSendCmdEnabled())==0) return 1;
    int nr = 0;
    int len;
    char buf[50];

    buf[49] = 0;

    sprintf(buf, "\r");
    len = write(fds[nr], buf, strlen(buf));
    tcflush(fds[nr], TCIFLUSH);

    if (len < 0) {
      perror("sendCommand write error - ");
      Comm::reopen("", nr);
      return 1;
    }

    len = 0;
    snprintf(buf, sizeof buf, "%d\r%d,%d,%d\r", addr, value1, value2, value3);
    if (strlen(buf) < 50) {
      len = write(fds[nr], buf, strlen(buf));
      tcflush(fds[nr], TCIFLUSH);
    }

    if (len < 0) {
      perror("sendCommand write error - ");
      Comm::reopen("", nr);
      return 1;
    }

    return 0;
}

int Comm::readSerialMulti(char addr, int * microControllerData)
{
    return readSerialMulti(addr, microControllerData, 0);
}

int Comm::readSerialMulti(char addr, int * microControllerData, int nr)
{
//   isLocked.wait(&mutex);
  QMutexLocker locker(&mutex);
  len = 0;
  int buf_len = 0;

  memset(buf, 0, sizeof buf);

  sprintf(buf, "\r");
  len = write(fds[nr], buf, strlen(buf));
  tcflush(fds[nr], TCIFLUSH);
  
  if (len < 0) {
    perror("readSerialMulti write error - ");
    Comm::reopen("", nr);
    return 1;
  }

  len = 0;
  snprintf(buf, sizeof buf, "%d\r", addr);    
  if (strlen(buf) < 255) {
    len = write(fds[nr], buf, strlen(buf));
    tcflush(fds[nr], TCIFLUSH);
  }

  if (len < 0) {
    perror("readSerialMulti write error - ");
    Comm::reopen("", nr);
    return 1;
  }

  do {
    len = read(fds[nr], &buf[buf_len], 255 - buf_len);
    buf_len += len;
    if (len < 1 || buf_len > 254) {
        return 1;
    }
  } while (buf[buf_len-1] != '\n');

  int i, j;
  int start = -1;
  j = 0;

  for (i = 0; i < buf_len; i++) {
    if (isdigit(buf[i]) && start == -1) start = 0;
    
    if ((buf[i] == ',' || buf[i] == '\n') && start > -1) {
      microControllerData[j] = atoi(&buf[start]);
      j++;
      start = i+1;
    }

    if (j > 15 ) break;
  }


  if (j != 16) {
	std::cout<<"Values: "<<j<<" Buf: "<<buf<<" Len: "<<strlen(buf)<<std::endl;
    std::cout<<microControllerData[0]<<std::endl;
    std::cout<<microControllerData[1]<<std::endl;
    std::cout<<microControllerData[2]<<std::endl;
    std::cout<<microControllerData[3]<<std::endl;
    std::cout<<microControllerData[4]<<std::endl;
    std::cout<<microControllerData[5]<<std::endl;
    std::cout<<microControllerData[6]<<std::endl;
    std::cout<<microControllerData[7]<<std::endl;
    std::cout<<microControllerData[8]<<std::endl;
    std::cout<<microControllerData[9]<<std::endl;
    std::cout<<microControllerData[10]<<std::endl;
    std::cout<<microControllerData[11]<<std::endl;
    std::cout<<microControllerData[12]<<std::endl;
    std::cout<<microControllerData[13]<<std::endl;
    std::cout<<microControllerData[14]<<std::endl;
    std::cout<<microControllerData[15]<<std::endl;
  }
  
  return 1;
}

int Comm::requestSerialMulti(char addr)
{
	return requestSerialMulti(addr, 0);
}

int Comm::requestSerialMulti(char addr, int nr)
{
//   isLocked.wait(&mutex);
  len = 0;
  QMutexLocker locker(&mutex);
  //if((conf->getSendCmdEnabled())==0) return 1;
  memset(buf, 0, sizeof buf);

  sprintf(buf, "\r");
  len = write(fds[nr], buf, strlen(buf));
  tcflush(fds[nr], TCIFLUSH);
  
  if (len < 0) {
    perror("readSerialMulti write error - ");
    Comm::reopen("", nr);
    return 1;
  }

  len = 0;
  snprintf(buf, sizeof buf, "%d\r", addr);    
  if (strlen(buf) < 255) {
    len = write(fds[nr], buf, strlen(buf));
    tcflush(fds[nr], TCIFLUSH);
  }

  if (len < 0) {
    perror("readSerialMulti write error - ");
    Comm::reopen("", nr);
    return 1;
  } 
  return 1;
}


int Comm::serialMultiResponse(int * microcontrollerData, int nr){
  int buf_len = 0;
  do {
    len = read(fds[nr], &buf[buf_len], 255 - buf_len);
    buf_len += len;
    if (len < 1 || buf_len > 254) {
        return 1;
    }
  } while (buf[buf_len-1] != '\n');

  int serialBufferIndex, microControllerDataIndex;
  microControllerDataIndex = 0;

  int data[MSG_MC_DATA_NR_ELEMENTS];
  bool checksumValid = false;

  bool readingNextIntChar = false;
  for (serialBufferIndex = 0; serialBufferIndex < buf_len; serialBufferIndex++) {
    if (serialBufferIndex == 0) {
        if (buf[serialBufferIndex] == MSG_MC_DATA_HEADER) {
            readingNextIntChar = true;
            continue;
        } else {
            qDebug() << "Reading microcontroller data failed. Wrong message header:" << buf[serialBufferIndex];
            return 1;
        }
    }

    if (buf[serialBufferIndex] == ',') {
        readingNextIntChar = true;
        continue;
    } else if (isdigit(buf[serialBufferIndex]) || buf[serialBufferIndex] == '-'){
        if (readingNextIntChar) {
            if (microControllerDataIndex < MSG_MC_DATA_NR_ELEMENTS) {
                data[microControllerDataIndex] = atoi(&buf[serialBufferIndex]);
                microControllerDataIndex++;
                readingNextIntChar = false;
            } else {
                int checksumInTheMessage = atoi(&buf[serialBufferIndex]);
                checksumValid = checksumInTheMessage == fletcherChecksum(data,MSG_MC_DATA_NR_ELEMENTS);
                if (!checksumValid) qDebug() << "Reading microcontroller data failed. Checksum was not right: " << checksumInTheMessage;
                break;
            }
        }
    } else {
        qDebug() << "Reading microcontroller data failed. Unexpected character at index:" << serialBufferIndex << "=" << buf[serialBufferIndex];
        return 1;
    }
  }

  if (checksumValid) {
      for (int i=0; i < MSG_MC_DATA_NR_ELEMENTS; i++) microcontrollerData[i] = data[i];
      return 0;
  } else {
    qDebug() << "Reading microcontroller data failed. Checksum was not right";
    return 1;
  }
}

uint8_t Comm::fletcherChecksum(int * microcontrollerData, int dataLength) {
    uint8_t sum1 = 0;
    uint8_t sum2 = 0;
    while (dataLength--){
        sum1 += *microcontrollerData++;
        sum2 += sum1;
    }
    return (sum1 & 0xF) | (sum2 << 4);
}

/*!
    \fn Comm::readSerial(char)
 */
int Comm::readSerial(char addr)
{
	return readSerial(addr, 0);
}
 
int Comm::readSerial(char addr, int nr)
{
  int len;
  int buf_len = 0;
  char buf[10];
//   isLocked.wait(&mutex);
  QMutexLocker locker(&mutex);
  
  sprintf(buf, "\r");    
  len = write(fds[nr], buf, strlen(buf));
  tcflush(fds[nr], TCIFLUSH);
  
  if (len < 0) {
    perror("readSerial write error - ");
      Comm::reopen("", nr);
    return 1;
  }

  sprintf(buf, "%d\r", addr);
  len = write(fds[nr], buf, strlen(buf));
  tcflush(fds[nr], TCIFLUSH);
  
  if (len < 0) {
    perror("readSerial write error - ");
      Comm::reopen("", nr);
    return 1;
  }

  do {
    len = read(fds[nr], &buf[buf_len], 10);
    buf_len += len;
    if (len == 0 || buf_len > 10) return 2;
  } while (buf[buf_len-1] != '\n');

  

  return atoi(buf);
}


//TODO Siin ka korda seada õige serial device kasutamine.
inline void Comm::reopen(const char * device)
{
	reopen(device, 0);
	return;
}

inline void Comm::reopen(const char * device, int nr)
{
    close(fds[nr]);
    Comm::openSerial(device, nr);
    
    if(device!=""){
       Comm::openSerial(device, nr);
      return;
    }
     Comm::openSerial(device, nr);
    return;
}
