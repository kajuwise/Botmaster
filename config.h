/**
 * @file config.h
 * @brief C++ Interface: config
 *
 * @details Configuration module for Robot Vision System.
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 * @author Valdur Kaldvee
 * @author Margus Ernits
 * @author Mauno Pihelgas
 * @author Erik Kaju
 */
#ifndef CONFIG_H
#define CONFIG_H
#include <getopt.h> //longopts
#include "simpleini/SimpleIni.h"
#include "objecttype.h"
#include "blackoutconf.h"
#include <cv.h>
#include <highgui.h>
#include <QSettings>
#include <QDir>
#include <string>

#define RC_FIELD 0
#define RC_ROBOT 1
#define RC_ACTION 2
#define RC_SIG_A '1'
#define RC_SIG_B '2'
#define RC_SIG_ALL_ROBOTS '3'
#define RC_SIG_START '1'
#define RC_SIG_STOP '2'
#define RC_SIG_PING '3'

/**
 * Configuration model for Robot Vision System
 *
 * @author Valdur Kaldvee
 * @author Margus Ernits <margus.ernits@itcollege.ee>
 * @author Mauno Pihelgas <mpihelga@itcollege.ee>
*/

using namespace std;

class Config{
public:

    //Config(int argn, char ** argv);
    ~Config();


private:

    //TODO mis parameetreid vÃµiks veel command linest ette anda? (Margus)
     bool immediateStart;
     int brightness;
     int contrast;
     char * bmp_filename;
     const char * video_device;
     const char * serial_device;

     const char * robot_name;
     const char * robot_params;
     Config();
     int readIni();
     int showDebugInfo;
     int sendCmdEnabled;
     int blackPercent;
     int colorPercent;
     int topCut;
     int sideCut;
     int squareCut;
     int img_width, img_height_front_cam, img_height_side_cam, img_offset_x, img_offset_y;
     int img_flip;
     int img_exposure;
     int img_gain;
     ObjectType blue, yellow, orange, black, green;
     BlackoutConf blackout;


public:
    QString colorConfFile;
    int  WB_kr,WB_kb,WB_kg;
    const char *  getmodemDevice();
    const char * getRobotName();
    const char * getRobotParams();
    void setConfig(int argn, char ** argv);
    static Config& getConfig();
    void setSendCmdEnabled(int);
    void setShowDebugInfo( int);
    void usage(char**);
    int getShowDebugInfo();
    int getSendCmdEnabled();
    int getTopCut();
    void setTopCut(int);
    int getSideCut();
    void setSideCut(int);
    void setImgWidth(int);
    int getImgWidth();
    void setImgHeightFrontCam(int);
    int getImgHeightFrontCam();
    void setImgHeightSideCam(int);
    int getImgHeightSideCam();
    void setImgOffsetX(int);
    int getImgOffsetX();
    void setImgOffsetY(int);
    int getImgOffsetY();
    bool getImmediateStart();
    void setImgFlip(int);
    int getImgFlip();
    void setImgExposure(int);
    int getImgExposure();
    void setImgGain(int);
    int getImgGain();
    char keyS;
    // 360 kaamera ainult
    int getSquareCut();
    void setSquareCut(int);
    bool takeWB;
    bool setSlider;
    int *getWbKr();
    int *getWbKb();
    int *getWbKg();


    ObjectType  * getBlue()
    {
        return  & blue;
    }
    ObjectType *  getYellow()
    {
        return  & yellow;
    }
    ObjectType * getOrange()
    {
        return & orange;
    }
    ObjectType  * getBlack()
    {
        return  & black;
    }
    ObjectType  * getGreen()
    {
      return  & green;
    }
    void setShowImage ( int theValue )
    {
        showImage = theValue;
    }


    int getShowImage() const
    {
        return showImage;
    }

    void setUseImageFile ( int theValue )
    {
        useImageFile = theValue;
    }


    int getUseImageFile() const
    {
        return useImageFile;
    }
    char * getImageFileName()
    {
        return bmp_filename;
    }

    void setSaveImage ( int theValue )
    {
        saveImage = theValue;
    }


    int getSaveImage() const
    {
        return saveImage;
    }


    BlackoutConf * getBlackout() {
        return & blackout;
    }

protected:
    int showImage;
    int useImageFile;
    int saveImage;
};

#endif
