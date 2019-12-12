//
// C++ Interface: camera
//
// Description: 
//
//
// Author: Valdur Kaldvee, Margus Ernits, Erik Kaju
//
//
#ifndef CAMERA_H
#define CAMERA_H
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <string>

#define CAMERA_FRONT "51208051"
#define CAMERA_LEFT "51209851"
#define CAMERA_RIGHT "41303651"

#ifdef XI
#warning "XIAPI defined"
#include <xiApi.h>
#define HandleResult(res,place) if (res!=XI_OK) {printf("Error after %s (%d)",place,res);exit(1);}
#endif

/**
This Class holds camera information

	@author Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>
*/
using namespace std;

class Camera{
public:
    Camera();
    ~Camera();
    void reOpen();
    void close();
    
    	// image buffer
    	

    /*!
        \fn Camera::getCapture()
     */
    CvCapture  * getCapture()
    {
       return capture;
    }
    
#ifdef XI
    static HANDLE xiH_front;
    static HANDLE xiH_left;
    static HANDLE xiH_right;

    static bool isCameraEnabled(const char* deviceIdOrSerial) {
        HANDLE checkCameraHandle = NULL;
        if (deviceIdOrSerial == CAMERA_FRONT) {
            checkCameraHandle = xiH_front;
        } else if (deviceIdOrSerial == CAMERA_LEFT) {
            checkCameraHandle = xiH_left;
        } else if (deviceIdOrSerial == CAMERA_RIGHT) {
            checkCameraHandle = xiH_right;
        } else return false;

        if (checkCameraHandle == NULL) return false;
        return true;
    }

	static XI_RETURN stat;
    HANDLE  getXI_CaptureFront() {
        return xiH_front;
    }

    HANDLE  getXI_CaptureLeft() {
        return xiH_left;
    }

    HANDLE  getXI_CaptureRight() {
        return xiH_right;
    }

    void setupXICamera(HANDLE* xiH, XI_RETURN* stat, const char* deviceIdOrSerial);
#endif

    IplImage* getImg()
    {
      if(img) cvReleaseImage(&img);
      img=cvLoadImage(pictureFilename);
      if(!img){
        printf("Could not load image file: %s\n",pictureFilename);
        exit(1);
      }
      return img;
    }
    
    IplImage* img; 

public:
  CvCapture  *  capture;
    char * pictureFilename;
  
};

#endif
