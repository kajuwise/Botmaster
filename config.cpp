/**
 * @file config.cpp
 * @brief C++ Implementation: config
 *
 * @details Configuration module for Robot Vision System.
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 * @author Valdur Kaldvee (C) 2008
 * @author Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
 * @author Mauno Pihelgas <mpihelga@itcollege.ee>, (C) 2010
 */
#include "config.h"
#include <string>
#include <iostream>
#include <QtCore>
#include <QDir>

Config::Config(/*int argn, char ** argv*/)
{

    immediateStart=false;
    colorConfFile = QDir::currentPath().append("/configs/calibration_tool/colors.ini");
    QSettings settings(colorConfFile, QSettings::IniFormat);

    /** nÃ¤itab silumisinfot */
showDebugInfo=0;
sendCmdEnabled=0;
brightness=0;
contrast=0;
blackPercent = 50;
colorPercent = 70;
topCut=0;
sideCut=0;
img_width=0;
img_height_front_cam=0;
img_height_side_cam=0;
img_offset_x=0;
img_offset_y=130;
img_flip=0;
img_exposure=0;
img_gain=0;
// 360 kaamera ainuult
squareCut=-1; // -1 do disable

  //Kasutatakse Image klassis pildi tÃ¶Ã¶tlemise juures (.h failis realiseeritud!).
   orange.hMin = settings.value("orange.hMin").toInt();
   orange.hMax = settings.value("orange.hMax").toInt();
   orange.sMin = settings.value("orange.sMin").toInt();
   orange.sMax = settings.value("orange.sMax").toInt();
   orange.lMin = settings.value("orange.lMin").toInt();
   orange.lMax = settings.value("orange.lMax").toInt();

   yellow.hMin = settings.value("yellow.hMin").toInt();
   yellow.hMax = settings.value("yellow.hMax").toInt();
   yellow.sMin = settings.value("yellow.sMin").toInt();
   yellow.sMax = settings.value("yellow.sMax").toInt();
   yellow.lMin = settings.value("yellow.lMin").toInt();
   yellow.lMax = settings.value("yellow.lMax").toInt();

   blue.hMin = settings.value("blue.hMin").toInt();
   blue.hMax = settings.value("blue.hMax").toInt();
   blue.sMin = settings.value("blue.sMin").toInt();
   blue.sMax = settings.value("blue.sMax").toInt();
   blue.lMin = settings.value("blue.lMin").toInt();
   blue.lMax = settings.value("blue.lMax").toInt();

   green.hMin = settings.value("green.hMin").toInt();
   green.hMax = settings.value("green.hMax").toInt();
   green.sMin = settings.value("green.sMin").toInt();
   green.sMax = settings.value("green.sMax").toInt();
   green.lMin = settings.value("green.lMin").toInt();
   green.lMax = settings.value("green.lMax").toInt();

   black.hMin = settings.value("black.hMin").toInt();
   black.hMax = settings.value("black.hMax").toInt();
   black.sMin = settings.value("black.sMin").toInt();
   black.sMax = settings.value("black.sMax").toInt();
   black.lMin = settings.value("black.lMin").toInt();
   black.lMax = settings.value("black.lMax").toInt();

   WB_kb = settings.value("WB_kb").toInt();
   WB_kg = settings.value("WB_kg").toInt();
   WB_kr = settings.value("WB_kr").toInt();

   blackout.throwerXFrom = settings.value("blackout.thrower.x.from").toInt();
   blackout.throwerXTo = settings.value("blackout.thrower.x.to").toInt();
   blackout.throwerY = settings.value("blackout.thrower.y").toInt();
   blackout.cornerLeftX = settings.value("blackout.corner.left.x").toInt();
   blackout.cornerLeftY = settings.value("blackout.corner.left.y").toInt();
   blackout.cornerRightX = settings.value("blackout.corner.right.x").toInt();
   blackout.cornerRightY = settings.value("blackout.corner.right.y").toInt();

    keyS = '-';
    showImage=0;
    saveImage=0;
}


Config::~Config()
{
}

void Config::usage(char** argv)
{
    printf("Usage: %s -r robot\n\n"
           "Options:\n"
           " -h, --help\n"
           "\tPrint detailed help screen.\n"
           " -r, --robot\n"
           "\tRobot name. Case sensitive. Required argument.\n"
           " -s, --sdev\n"
           "\tSerial device name. Case sensitive.\n"
           " -v, --videodev\n"
           "\tVideo device name. Case sensitive.\n"
           " -f, --file\n"
           "\tPicture filename. Case sensitive.\n"
           " -c, --contrast\n"
           "\tContrast value.\n"
           " -p, --params\n"
           "\tAdditional robot parameters (as string).\n"
           " -b, --brightness\n"
           "\tBrightness value.\n\n", argv[0]);
}

const char * Config::getmodemDevice()
{

    if (serial_device) {
        //	  std::cout<<"testib serial: "<<serial_device<<"\n";
        return serial_device;
    }

    return "/dev/ttyUSB0";
}

/*!
    \fn Config::getRobotName()
 */
const char * Config::getRobotName()
{
    if (robot_name) return robot_name;
    return "UNSPECIFIED";
}


const char* Config::getRobotParams()
{
    if (robot_params) return robot_params;
    return "UNSPECIFIED";
}


Config & Config::getConfig()
{
    static Config instance;
    return  instance;
}

/*!
    \fn Config::setConfig(int argn, const char ** argv)
 */
void Config::setConfig(int argn, char ** argv)
{
    struct option longopts[] =
    {
    { "robot",      required_argument, NULL, 'r'}, /* Robot name */
    { "sdev",       required_argument, NULL, 's'}, /* Serial device name */
    { "videodev",   required_argument, NULL, 'v'}, /* video device name */
    { "file",       required_argument, NULL, 'f'}, /* picture filename */
    { "go",         no_argument, NULL, 'g'}, /* Starti immediately */
    { "contrast",   required_argument, NULL, 'c'}, /* contrast value */
    { "brightness", required_argument, NULL, 'b'}, /* brightness value */
    { "help",       no_argument,       NULL, 'h'}, /* Displays detailed help */
    { "params",     no_argument,       NULL, 'p'}, /* Parameters passed over to robots */
    { 0, 0, 0, 0 }
};

    if(argn==1)
    {
        usage(argv);
        exit(0);
    }

    serial_device=NULL;
    robot_name=NULL;
    robot_params=NULL;
    char c;
    while ((c = getopt_long(argn, argv, "hogs:b:c:v:f:r:p:", longopts, NULL)) != -1)
    {
        switch (c)
        {
        case 'b':
            if (optarg != NULL){
                brightness = atoi(optarg);
            }else{
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 'c':
            if (optarg != NULL){
                contrast = atoi(optarg);
            }else{
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 's':
            if (optarg != NULL)
                serial_device = optarg;
            else
            {
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 'r':
            if (optarg != NULL)
                robot_name = optarg;
            else
            {
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 'f':
            if (optarg != NULL){
                bmp_filename = optarg;
            }
            else
            {
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 'g':
            qDebug() << "SET TO GO IMMEDIATELY!";
            sendCmdEnabled = 1;
            immediateStart = true;
            break;
        case 'v':
            if (optarg != NULL)
                video_device = optarg;
            else
            {
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 'h':
            usage(argv);
            exit(0);
            break;
        case 'p':
            if (optarg != NULL)
                robot_params = optarg;
            else
            {
                printf("Error!!!\n");
                exit(1);
            }
            break;
        case 0:
            break;
        case ':':
            fprintf(stderr, "%s: option `-%c' requires an argument\n",
                    argv[0], optopt);
            break;
        case '?':
        default:
            fprintf(stderr, "%s: option `-%c' is invalid:\nExiting...\n",
                    argv[0], optopt);
            exit(1);
            break;
        }
    }
    readIni();
}


/**
 * Reads robot configuration from ini file
 * @return int 0 - success
 */
int Config::readIni()
{

    if (robot_name == NULL)
    {
        fprintf(stderr, "Robot name not specified!");
        exit(1);
    }
    std::string confFileName("configs/");
    confFileName.append(robot_name);
    confFileName.append(".ini");

    CSimpleIniA iniConf(true, true, true);
    if (iniConf.LoadFile(confFileName.c_str()) < 0)
    {
        fprintf(stderr, "Failed to load robot configuration file!");
        exit(1);
    }

    const char * niceName
            = iniConf.GetValue("robot", "robot_name", NULL);


    const char * tmp = iniConf.GetValue("robot", "brightness", 0);
    if(tmp && brightness==0) brightness = atoi(tmp);


    tmp = NULL;
    tmp = iniConf.GetValue("robot", "contrast", 0);
    if(tmp && contrast==0) contrast = atoi(tmp);
    //if (!contrast) contrast =255; <-- max value?

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "topCut", 0);
    if(tmp && topCut==0) topCut = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_width", 0);
    if(tmp && img_width==0) img_width = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_height_front_cam", 0);
    if(tmp && img_height_front_cam==0) img_height_front_cam = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_height_side_cam", 0);
    if(tmp && img_height_side_cam==0) img_height_side_cam = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_offset_x", 0);
    if(tmp && img_offset_x==0) img_offset_x = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_offset_y", 0);
    if(tmp && img_offset_y==0) img_offset_y = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_flip", 0);
    if(tmp && img_flip==0) img_flip = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_exposure", 0);
    if(tmp && img_exposure==0) img_exposure = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "img_gain", 0);
    if(tmp && img_gain==0) img_gain = atoi(tmp);

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "serial_device", NULL);
    if(tmp && serial_device==0) serial_device = tmp;

    tmp = NULL;
    tmp = iniConf.GetValue("robot", "video_device", NULL);
    if(tmp && video_device==0) video_device = tmp;

    char message[255];
    sprintf(message, "Robot \"%s\" configuration loaded from \"%s\" ...\n", niceName, confFileName.c_str());
    qDebug()<< message;

    // Debugimiseks
    /*
  std::cout<<"Brightness: "<<brightness<<std::endl;
  std::cout<<"Contrast: "<<contrast<<std::endl;
  std::cout<<"Serial: "<<serial_device<<std::endl;
  std::cout<<"Video: "<<video_device<<std::endl<<std::endl;
  */

    return 0;
}

void Config::setShowDebugInfo ( int theValue )
{
    showDebugInfo = theValue;
}


int Config::getShowDebugInfo()
{
    return showDebugInfo;
}

void Config::setSendCmdEnabled ( int theValue )
{
    sendCmdEnabled = theValue;
}

int Config::getSendCmdEnabled()
{
    return sendCmdEnabled;
}

void Config::setTopCut ( int theValue )
{
    topCut = theValue;
}

int Config::getTopCut()
{
    return topCut;
}

void Config::setSideCut ( int theValue )
{
    sideCut = theValue;
}

int Config::getSideCut()
{
    return sideCut;
}

void Config::setImgWidth ( int theValue )
{
    img_width = theValue;
}

int Config::getImgWidth()
{
    return img_width;
}

void Config::setImgHeightFrontCam ( int theValue )
{
    img_height_front_cam = theValue;
}

int Config::getImgHeightFrontCam ()
{
    return img_height_front_cam;
}

void Config::setImgHeightSideCam ( int theValue )
{
    img_height_side_cam = theValue;
}

int Config::getImgHeightSideCam ()
{
    return img_height_side_cam;
}

void Config::setImgOffsetX ( int theValue )
{
    img_offset_x = theValue;
}

int Config::getImgOffsetX()
{
    return img_offset_x;
}

void Config::setImgOffsetY ( int theValue )
{
    img_offset_y = theValue;
}

int Config::getImgOffsetY()
{
    return img_offset_y;
}

void Config::setImgFlip ( int theValue )
{
    img_flip = theValue;
}

int Config::getImgFlip()
{
    return img_flip;
}

void Config::setImgExposure ( int theValue )
{
    img_exposure = theValue;
}

int Config::getImgExposure()
{
    return img_exposure;
}

void Config::setImgGain ( int theValue )
{
    img_gain = theValue;
}

int Config::getImgGain()
{
    return img_gain;
}

/* 360 kaamera ainult*/
int Config::getSquareCut()
{
    return squareCut;
}
void Config::setSquareCut(int theValue)
{
    squareCut = theValue;
}
int *Config::getWbKb(){
    return & WB_kb;
}
int *Config::getWbKg(){
    return & WB_kg;
}
int *Config::getWbKr(){
    return & WB_kr;
}
bool Config::getImmediateStart(){
    return immediateStart;
}

