//
// C++ Implementation: view
//
// Description: 
//
//
// Author: Valdur Kaldvee, Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "view.h"
#include <iostream>
#include "objecttype.h"


View::View()
{
    Config & temp =Config::getConfig();
    conf = & temp;

    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, hScale, vScale, 0, lineWidth, 8);

    // Create a window in which the captured images will be presented
    //cvNamedWindow("Robovision" /*TODO getRobotName from config */, CV_WINDOW_NORMAL);
    cvNamedWindow("Robovision" /*TODO getRobotName from config */, CV_WINDOW_AUTOSIZE);

//    cvCreateTrackbar("H min", "Robovision", &((conf->getYellow())->hMin), 255, NULL);
//    cvCreateTrackbar("H max", "Robovision", &((conf->getYellow())->hMax), 255, NULL);

//    cvCreateTrackbar("S min", "Robovision", &((conf->getYellow())->sMin), 255, NULL);
//    cvCreateTrackbar("S max", "Robovision", &((conf->getYellow())->sMax), 255, NULL);

//    cvCreateTrackbar("L min", "Robovision", &((conf->getYellow())->lMin), 255, NULL);
//    cvCreateTrackbar("L max", "Robovision", &((conf->getYellow())->lMax), 255, NULL);


//      cvCreateTrackbar("H min", "Robovision", &((conf->getBlue())->hMin), 255, NULL);
//      cvCreateTrackbar("H max", "Robovision", &((conf->getBlue())->hMax), 255, NULL);

    //  cvCreateTrackbar("S min", "Robovision", &((conf->getBlue())->sMin), 255, NULL);
    //  cvCreateTrackbar("S max", "Robovision", &((conf->getBlue())->sMax), 255, NULL);

    //  cvCreateTrackbar("L min", "Robovision", &((conf->getBlue())->lMin), 255, NULL);
    //  cvCreateTrackbar("L max", "Robovision", &((conf->getBlue())->lMax), 255, NULL);

//        cvCreateTrackbar("kr", "Robovision", &(conf->WB_kr), 255, NULL);
//        cvCreateTrackbar("kg", "Robovision", &(conf->WB_kg), 255, NULL);
//        cvCreateTrackbar("kb", "Robovision", &(conf->WB_kb), 255, NULL);
}


View::~View()
{
}

View & View::getView()
{
    static  View instance;
    return  instance;
}




/*!
    \fn View::show()
 */
void View::show(IplImage * image)
{
    int precrnt = 65;

    //IplImage *showImag = cvCreateImage(cvSize((int)((image->width*precrnt)/100),(int)((image->height*precrnt)/100) ), image->depth,image->nChannels);
    //cvResize(image,showImag);


    static char key='0';

    static int frame = 0;
    
    if(1 || frame%10 == 0 /*conf->getShowImage()==1*/){
        cvShowImage( "Robovision", image);
    }
    frame++;
    key = cvWaitKey(2);
    if(key == 27) {
        qDebug() << "Escape pressed";
        conf->keyS = 'x';
    }

    switch(key){
    case 'd':
        if (conf->getShowDebugInfo()) {
            conf->setShowDebugInfo(0);
            qDebug("Debug OFF\n");
        } else {
            conf->setShowDebugInfo(1);
            qDebug("Debug ON\n");
        }
        break;
    case ' ': //Space
        conf->setSendCmdEnabled(1);
        qDebug("Command Send Enabled \n");
        break;
    case '0':
        conf->keyS = key;
        break;
    case '1':
        conf->keyS = key;
        break;
    case '2':
        conf->keyS = key;
        break;
    case '3':
        conf->keyS = key;
        break;
    case '4':
        conf->keyS = key;
        break;
    case '5':
        conf->keyS = key;
        break;
    case '6':
        conf->keyS = key;
        break;
    case '7':
        conf->keyS = key;
        break;
    case 'l':
        conf->keyS = key;
        qDebug() << "Pressed L";
        break;
    case 'f':
        conf->keyS = key;
        qDebug() << "Pressed F";
        break;
    case 'i':
        conf->keyS = key;
        qDebug() << "Pressed I";
        break;
    case 'c':
        conf->keyS = 'f';
        qDebug() << "Pressed C";
        break;
    case 'r':
        conf->keyS = key;
        qDebug() << "Pressed R";
        break;
    case 'y':
        conf->keyS = key;
        qDebug() << "Pressed Y";
        break;
    case 'b':
        conf->keyS = key;
        qDebug() << "Pressed B";
        break;
    case 'g':
        conf->keyS = key;
        qDebug() << "Pressed G";
        break;
    case 'a':
        conf->keyS = key;
        qDebug() << "Pressed A";
        break;
    case 'q':
        conf->keyS = key;
        qDebug() << "Pressed q";
        break;
    case 'p':
        cvSaveImage("frame.bmp", image);
        break;
    case 'x':
        qDebug() << "Pressed X, ignoring.";
        break;
    case 't':
        conf->takeWB = true;
    }

}




