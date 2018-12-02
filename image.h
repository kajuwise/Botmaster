/**
 * @file image.h
 * @brief C++ Interface: image
 *
 * @details This file holds image processing functionality.
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 * @author Valdur Kaldvee (C) 2008
 * @author Margus Ernits <margus.ernits@itcollege.ee>, (C) 2008
 * @author Mauno Pihelgas <mpihelga@itcollege.ee>, (C) 2010
 */
#ifndef IMAGE_H
#define IMAGE_H
#include <cv.h>
#include <highgui.h>
#include <QTime>
#include <QDebug>
#include "camera.h"
#include "config.h"
#include "objecttype.h"
#include "capturethread.h"

/* TODO to conf */
#define MIN_AREA 20

#define BALL            1
#define BASKET_PURPLE     2
#define BASKET_BLUE       3
#define FIELD_EDGE_R    4
#define FIELD_EDGE_L    5
#define IMAGE_WIDTH     1280

/**
 * @brief This class holds image processing functionality
 * @author Valdur Kaldvee
 * @author Margus Ernits
 * @author Erik Kaju
 */
class Image{
public:
    class Object {
    public:
        int type; ///< Object type (BALL, GOAL)
        CvPoint center;		 ///< Center coordiantes
        CvRect rect;		 ///< Bounding rectangle
        double area;		 ///< Area of the object
        int distanceH;		 ///< Horizontal distance from image center px
        int distanceHCm;		 ///< Horizontal distance from image center centimeters
        int distanceV;		 ///< Vertical distance from image center px
        int distanceVCm;     ///< Vertical distance from image center centimeters
        int distanceC;		 ///< Distance from image center
        void reset() {
            type = area = distanceH = distanceV = distanceC = 0;
        }
    };
public:
    ~Image();
    IplImage* getFrame(const char* cameraNr); ///< Gets the next image frame.
    static Image &  getImage(); ///< Returns static Image class instance. It cannot be accessed directly because Image is a singleton class.
    CvCapture* capture; ///< Capture object returned from Camera::getCapture()
#ifdef XI
    static HANDLE xiH_front;
    static HANDLE xiH_left;
    static HANDLE xiH_right;
#endif
    IplImage* getImagePtr(); ///< Returns IplImage class object.
    int fps; ///< FPS (Frames per second) value that is printed on the image.
protected:
    CvScalar colorBall;
    CvScalar colorBasketPurple;
    CvScalar colorBasketBlue;


private:
    CaptureThread captureThread;
    QTime time1;
    QTime time2;
    int fpsCounter; ///< Counts the number of calls to Image::getFrame() in 1 second. After 1 second it is defined as int fps;.
    int WB_old_R;
    int WB_old_G;
    int WB_old_B;
    char str[50]; ///< A universal string (char array) used to write on the image.
    int start;
    Config * conf; ///< Configuration object. In this class it is used to get color and light related values from the configuration module.
    CvSeq* objects; ///< @todo Not in use?

    Image();

#ifdef XI
    IplImage* xi_img_raw;
    static XI_RETURN stat;
    void TakeWhitebalance();
    void WhiteBalaceSlider();
    XI_IMG xi_image;


#endif

    IplImage* img; ///< Original unprocessed image.
    IplImage* work; ///< Processed image. @todo What exactly is it?
    IplImage* binary; ///< Processed image. @todo What exactly is it?
    //int height, width; ///< Image dimensions.
    CvMat* LUT; ///< LookUp Table (LUT) for RGB to YUV conversion.

    /**
   * @brief Generates LookUp Table (LUT) for RGB to YUV conversion.
   * @param LUT - Pointer to CvMat type object LUT.
   * @param code - indicates whether to look for cans (0 | 1 (0 = FALSE, 1 = TRUE))
   */
    void genLut(CvMat*, int);

    Camera cam; ///< Camera object
    void fastCvtColor(IplImage* , CvMat* );
    void fastCvtColor(IplImage*, IplImage*, CvMat* );
    void fastCvtColorFraction(IplImage*, IplImage*, CvMat*, int, int, bool);
    void fastCvtColorFraction(IplImage*, IplImage*, CvMat*, int, int);
    void fastCvtColorFractionWithBlackout(IplImage*, IplImage*, CvMat*, int, int);


    void findBall(IplImage*, IplImage*, IplImage*, ObjectType, int, CvSeq*, CvSeq*, CvSeq*, int*);
    void findGoal(IplImage*, IplImage*, IplImage*, ObjectType, int, CvSeq*, CvSeq*);
    void findField(IplImage*, IplImage*, IplImage*, ObjectType, CvSeq*, int *);
    void findFieldEdge(IplImage*, IplImage*, IplImage*, int*, ObjectType, int);
    int checkFieldEdge(IplImage*, int*, CvPoint, CvPoint, CvPoint*);
    void updateROI();
    void clearROI();

    CvMemStorage* storage; ///< Dynamicly growing memory storage for holding sequences, contours, etc


public:
    CvFont font; ///< Font which is used to write on the image
    CvSeq* found_objects; ///< Growable sequence of elements. Contains objects found by image processing

    /**
   * @brief Processes the image for objects and other related information.
   * @details This function still has the parameters from 2008, which are unused. However, currently the body of the function calls the
    findBall(IplImage* , IplImage* , IplImage* , ObjectType , int, int) function which holds the picture processing for 2009 football assignment.
   * @param minHeight -
   */
    void process(int * fieldTop);

    /**
     * @brief Camera reopen ...
     *
     */
    void reOpen();

    int frameCounter;
    void whiteBalaceSlider();
    void takeWhiteBalance();
};

#endif
