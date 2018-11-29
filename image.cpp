/**
 * @file image.cpp
 * @brief C++ Implementation: image
 *
 * @details This file holds image processing functionality.
 *
 * Copyright: See COPYING file that comes with this distribution
 *
 * @author Valdur Kaldvee
 * @author Margus Ernits
 * @author Mauno Pihelgas
 * @author Erik Kaju
 */
#include "image.h"
#include <stdio.h>
#include <iostream>
#include "visiontime.h"
#include "capturethread.h"
#include "QtConcurrent/QtConcurrentRun"

Image::Image()
{
    fpsCounter = 0;
    fps = 0;
    start = 1;
    Config & tmp = Config::getConfig();
    conf = & tmp;

    VisionTime::DO()->markBegin("generate_LUT_table");
    LUT = cvCreateMat(16777216, 1, CV_8UC3);
    genLut(LUT,CV_BGR2HLS);
    VisionTime::DO()->markEnd("generate_LUT_table");
    if (conf->getShowImage() == 0) {
#ifdef XI
        if(Camera::isCameraEnabled(CAMERA_FRONT)) {
            xiH_front=cam.getXI_CaptureFront();
        } else {
            qDebug("Image.cpp: Front camera not enabled, will abort initialisation!");
            exit(1);
        }
        if(Camera::isCameraEnabled(CAMERA_LEFT)) xiH_left=cam.getXI_CaptureLeft();
        if(Camera::isCameraEnabled(CAMERA_RIGHT)) xiH_right=cam.getXI_CaptureRight();
        xi_image.size = sizeof(XI_IMG);
        xi_image.bp = NULL;
        xi_image.bp_size = 0;
#else
        capture = cam.getCapture();
        captureThread.startAcquisition(capture);
#endif
    }
    colorBall.val[0] = 0;
    colorBall.val[1] = 255;
    colorBall.val[2] = 255;
    colorBall.val[3] = 0;

    colorBasketPurple = CV_RGB(255,0,255);
    colorBasketBlue = CV_RGB(0,255,255);

    double hScale = 1.0;
    double vScale = 1.0;
    int lineWidth = 2;
    cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX_SMALL, hScale, vScale, 0, lineWidth, 8);
    storage = cvCreateMemStorage(0);
    if (conf->getShowImage() == 0) {
#ifdef XI
        xi_img_raw = cvCreateImage(cvSize(1280, 1024), IPL_DEPTH_8U, 1);    // TODO: not used but removing breaks updateROI and clearROI
        img = cvCreateImage(cvSize(conf->getImgWidth(), conf->getImgHeightSideCam()), IPL_DEPTH_8U, 3);
        xiStartAcquisition(xiH_front);
        if (Camera::isCameraEnabled(CAMERA_LEFT)) xiStartAcquisition(xiH_left);
        if (Camera::isCameraEnabled(CAMERA_RIGHT)) xiStartAcquisition(xiH_right);
#endif
    }
}

Image::~Image()
{
    if(work) cvReleaseImage(&work);

    if(binary) cvReleaseImage(&binary);

    if(LUT)  cvReleaseMat(&LUT);

    if(capture) cvReleaseCapture(&capture);

#ifdef XI
    // Close devices
    if (xiH_front)
        xiCloseDevice(xiH_front);
    if (xiH_left)
        xiCloseDevice(xiH_left);
    if (xiH_right)
        xiCloseDevice(xiH_right);
#endif
}


void Image::genLut(CvMat* LUT, int code) {
    int   step  = LUT->step/sizeof(uchar);
    uchar *data = LUT->data.ptr;

    int i;
    for(i = 0; i < 16777216; i++) {
        data[i*step+0] = (uchar) i;
        data[i*step+1] = (uchar) (i >> 8);
        data[i*step+2] = (uchar) (i >> 16);
    }

    cvCvtColor(LUT, LUT, code);
}

void Image::updateROI() {
    int x, y, width, height;

    if (conf->getSquareCut() >= 0){

        y = conf->getSquareCut();
        height = width = img->height - (conf->getSquareCut() * 2);
        x = (img->width - height)/2;
    }
    else {
        if (conf->getTopCut() >= 0) {
            y = conf->getTopCut();
            height = img->height - conf->getTopCut();
        } else {
            y = 0;
            height = -conf->getTopCut();
        }
        x = conf->getSideCut();
        width = img->width - (conf->getSideCut() * 2);
    }
    if (conf->getShowImage() == 0) {
#ifdef XI
        cvSetImageROI(xi_img_raw, cvRect(x, y, width, height));
#endif
    }
    cvSetImageROI(img, cvRect(x, y, width, height));
    cvSetImageROI(work, cvRect(x, y, width, height));
    cvSetImageROI(binary, cvRect(x, y, width, height));

}

void Image::clearROI() {
#ifdef XI
    cvResetImageROI(xi_img_raw);
#endif
    cvResetImageROI(img);
    cvResetImageROI(work);
    cvResetImageROI(binary);
}

/*!
    \fn Image::cvQueryFrame(CvCapture*);
 */
const char* lastCam = "undefined";
IplImage* Image::getFrame(const char* cameraDevice)
{
    VisionTime::DO()->newFrame();

    /// @todo implement me
    if (conf->getShowImage() == 0) {

#ifdef XI
        HANDLE useXIHandle = NULL;

        if(Camera::isCameraEnabled(CAMERA_LEFT) == false) cameraDevice = CAMERA_FRONT;
        if(Camera::isCameraEnabled(CAMERA_RIGHT) == false) cameraDevice = CAMERA_FRONT;

        if (cameraDevice == CAMERA_LEFT) {
            useXIHandle = xiH_left;
            if(lastCam != CAMERA_LEFT) {
                img = cvCreateImage(cvSize(conf->getImgWidth(), conf->getImgHeightSideCam()), IPL_DEPTH_8U, 3);       
            }
        } else if (cameraDevice == CAMERA_RIGHT) {
            useXIHandle = xiH_right;
            if(lastCam != CAMERA_RIGHT) {
                img = cvCreateImage(cvSize(conf->getImgWidth(), conf->getImgHeightSideCam()), IPL_DEPTH_8U, 3);
            }
        } else {
            useXIHandle = xiH_front;
            if(lastCam != CAMERA_FRONT) {
                img = cvCreateImage(cvSize(conf->getImgWidth(), conf->getImgHeightFrontCam()), IPL_DEPTH_8U, 3);
            }
        }
        lastCam = cameraDevice;

        VisionTime::DO()->markBegin("create_image_objects");
        if (start == 1) {
            work = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3);
            work->origin = img->origin;
            binary = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1);
            binary->origin = img->origin;

            time2.start();
            start = 0;
        }
        VisionTime::DO()->markEnd("create_image_objects");
        updateROI();
        VisionTime::DO()->markBegin("get_img_and_cpy");
        takeWhiteBalance();
        whiteBalaceSlider();

        stat = xiGetImage(useXIHandle, 5000, &xi_image);
        HandleResult(stat,"xiGetImage");

        VisionTime::DO()->markEnd("get_img_and_cpy");

        VisionTime::DO()->markBegin("bayer_bgr_conversion");

        memcpy(img->imageData, xi_image.bp, xi_image.width*xi_image.height*3);

        if (conf->getImgFlip()) {
            cvFlip(img, img, -1);   // Flip image on both axis
        }

        if (conf->getSaveImage()) {
            frameCounter++;
            char pildinr[100];
            sprintf(pildinr, "frame%d.bmp", frameCounter);
            cvSaveImage(pildinr, img);
            conf->setSaveImage(0);
        }

        VisionTime::DO()->markEnd("bayer_bgr_conversion");
#else
        VisionTime::DO()->markBegin("get_frame_from_cam_imagecpp");
        //img = cvQueryFrame(capture);
        img = captureThread.getLatestFrame();
        VisionTime::DO()->markEnd("get_frame_from_cam_imagecpp");
        // std::cout<<"Camera Frame\n";

        VisionTime::DO()->markBegin("create_image_objects");
        if (start == 1) {
            // get the image data
            //height = img->height;
            //width  = img->width;

            work = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3);
            work->origin = img->origin;
            binary = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1);
            binary->origin = img->origin;

            start = 0;
        }
        VisionTime::DO()->markEnd("create_image_objects");

        updateROI();
#endif
    } else {
        VisionTime::DO()->markBegin("get_image_from_cam");
        img=cam.getImg();
        if (start == 1) {
            work = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3);
            work->origin = img->origin;
            binary = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 1);
            binary->origin = img->origin;

            start = 0;
        }
        updateROI();
        VisionTime::DO()->markEnd("get_image_from_cam");
        // std::cout<<"Image Frame\n";
    }

    fpsCounter++;
    if (time2.elapsed() >= 1000) {
        time2.restart();
        fps = fpsCounter;
        fpsCounter = 0;
    }

    time1.start();

    if(start == 0){
        time2.start();
        start = 2; //midnight HACK
    }

    VisionTime::DO()->markBegin("LUT_bgr_hsl");

//  BGR -> HSL Konverteerimine
//      Vana single-threaded RGB-HSL konversioon, ~9.05ms @ 996x996px
//  fastCvtColor(img, work, LUT);

//      Uus variant, ~7.65ms @ 996x996px
    QFuture<void> future1 = QtConcurrent::run(this, &Image::fastCvtColorFraction, img, work, LUT, 1,2);
    QFuture<void> future2 = QtConcurrent::run(this, &Image::fastCvtColorFraction, img, work, LUT, 2,2);

    future1.waitForFinished();
    future2.waitForFinished();

// qDebug() << QThread::idealThreadCount();
    VisionTime::DO()->markEnd("LUT_bgr_hsl");

    // draw center line
    cvLine(img, cvPoint(img->roi->width/2, 0), cvPoint(img->roi->width/2, img->height), CV_RGB(0,0,127), 2, 8, 0);
    cvLine(img, cvPoint(0,img->roi->height/2), cvPoint(img->width,img->roi->height/2), CV_RGB(0,0,127), 2, 8, 0);

    VisionTime::DO()->markBegin("print_frame_statistics");
    sprintf(str, "%d", time1.elapsed());
    cvPutText(img, str, cvPoint(10, 50), &font, CV_RGB(255,0,0));

    sprintf(str, "FPS %d", fps);
    cvPutText(img, str, cvPoint(10, 80), &font, CV_RGB(255,0,0));
    VisionTime::DO()->markEnd("print_frame_statistics");

    return img;
}


/*!
    \fn Image::getImage()
 */
Image &  Image::getImage()
{
    static Image instance;
    return instance;
}

IplImage* Image::getImagePtr()
{
    return img;
}

void Image::reOpen(){
    cam.close();

    cam.reOpen();
    capture=cam.getCapture();
}

void Image::findBall(IplImage* img, IplImage* work, IplImage* binary, ObjectType type, int itemType,
                     CvSeq* field, CvSeq* goalYellowHull, CvSeq* goalBlueHull, int* fieldEdge) {

    CvSeq* contours = NULL;
    CvMemStorage* storage = NULL;
    CvScalar color;
    Image::Object obj;

    int height, width, step , step2, channels;
    int heightStartIndex, heightEndIndex;
    int widthStartIndex, widthEndIndex;
    uchar *data1;
    uchar *data2;
    int i, j;
    uchar H, S, L;
    int match = 0;
    int nrOfMatches = 0;

    float radius;
    CvPoint2D32f center;
    float circleArea;
    float percent;
    int fieldDist;
    int goalYellowDist;
    int goalBlueDist;

    // get the image data
    height    = work->height;
    width     = work->width;
    step      = work->widthStep;
    channels  = work->nChannels;
    data1     = (uchar *)work->imageData;

    // get the image data
    step2     = binary->widthStep;
    data2     = (uchar *)binary->imageData;

    heightStartIndex = work->roi->yOffset;
    heightEndIndex = work->roi->yOffset + work->roi->height;

    widthStartIndex = work->roi->xOffset;
    widthEndIndex = work->roi->xOffset + work->roi->width;

    for(i = heightStartIndex; i < heightEndIndex; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
        match = 0;

        H = data1[i*step+j*channels+0];
        L = data1[i*step+j*channels+1];
        S = data1[i*step+j*channels+2];

        // if pixel is ball color
        if (H > type.hMin && H < type.hMax &&
                L > type.lMin && L < type.lMax &&
                S > type.sMin) match = 1;

        if (match) {
            nrOfMatches++;
            data2[i*step2+j] = 1;
        } else {
            data2[i*step2+j] = 0;
        }
    }

    if (nrOfMatches < MIN_AREA) return;

    storage = cvCreateMemStorage(0);
    i = cvFindContours(binary, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    //fprintf(stderr,"Contours: %d\n", i);

    if (i == 0) {
        cvReleaseMemStorage(&storage);
        return;
    }

    while (contours) {
        if (conf->getShowDebugInfo()) {
            cvDrawContours(img, contours, CV_RGB(255,0,0), CV_RGB(0,255,0),0, 2, CV_AA, cvPoint(0,0));
        }

        obj.area = fabs(cvContourArea(contours, CV_WHOLE_SEQ));

        if (obj.area >= MIN_AREA) {
            obj.type = itemType;
            color = colorBall;

            cvMinEnclosingCircle(contours, &center, &radius);

            obj.center.x = center.x;
            obj.center.y = center.y;

            obj.rect.x = center.x - radius;
            obj.rect.y = center.y - radius;
            obj.rect.width = radius * 2;
            obj.rect.height = radius * 2;


            // Find distance to field edge
            fieldDist = cvPointPolygonTest(field, center, true);

            if (fieldDist > -10) {
                // Check if no black line between robot and ball
                if (true || checkFieldEdge(img, fieldEdge, cvPoint(work->roi->width/2, work->roi->height), obj.center, NULL)) {

                    obj.distanceH = obj.center.x - (work->roi->width / 2);
                    obj.distanceV = (obj.center.y - (work->roi->height / 2))/-1;

                    circleArea = 3.14159265 * radius * radius;
                    percent = obj.area / circleArea * 100.0;

                    obj.area = circleArea;

                    if (conf->getShowDebugInfo()) {
                        sprintf(str, "%d", (int) obj.area);
                        //sprintf(str, "%d", (int) obj.center.y);
                        cvPutText(img, str, cvPoint(obj.center.x + 20, obj.center.y), &font, CV_RGB(0,0,0));
                    }

                    cvCircle(img, cvPoint(center.x, center.y), radius, color, 2, CV_AA, 0);
                    cvSeqPush( found_objects, &obj );
                }
            }
        }
        // take the next contour
        contours = contours->h_next;
    }

    // cleanup
    cvReleaseMemStorage(&storage);
    return;
}

void Image::findGoal(IplImage* img, IplImage* work, IplImage* binary, ObjectType type, int itemType, CvSeq* field, CvSeq* goalHull) {
    CvSeq* contours = NULL;
    CvMemStorage* storage = NULL;
    CvScalar color;
    Image::Object obj;

    CvSeq* goalHullPoints = NULL;
    CvMemStorage* goalHullPointsStorage = NULL;

    CvSeq* hull = NULL;

    int step, step2, channels;
    int heightStartIndex, heightEndIndex;
    int widthStartIndex, widthEndIndex;
    uchar *data1;
    uchar *data2;
    int i, j;
    uchar H, S, L;
    int match;
    int nrOfMatches = 0;

    int fieldDist;
    int fieldDistMin;

    CvSeqReader reader;
    CvPoint p;

    // get the image data
    step      = work->widthStep;
    channels  = work->nChannels;
    data1     = (uchar *)work->imageData;

    // get the image data
    step2     = binary->widthStep;
    data2     = (uchar *)binary->imageData;

    heightStartIndex = work->roi->yOffset;
    heightEndIndex = work->roi->yOffset + work->roi->height;

    widthStartIndex = work->roi->xOffset;
    widthEndIndex = work->roi->xOffset + work->roi->width;

    for(i = heightStartIndex; i < heightEndIndex; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
        match = 0;

        H = data1[i*step+j*channels+0];
        L = data1[i*step+j*channels+1];
        S = data1[i*step+j*channels+2];

        // if pixel is goal color
        if (H > type.hMin && H < type.hMax &&
                L > type.lMin && L < type.lMax &&
                S > type.sMin) match = 1;

        if (match) {
            nrOfMatches++;
            data2[i*step2+j] = 1;
        } else {
            data2[i*step2+j] = 0;
        }
    }

    if (nrOfMatches < MIN_AREA) return;

    storage = cvCreateMemStorage(0);
    i = cvFindContours(binary, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    //fprintf(stderr,"Contours: %d\n", i);

    if (i == 0) {
        cvReleaseMemStorage(&storage);
        return;
    }

    goalHullPointsStorage = cvCreateMemStorage(0);
    goalHullPoints = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), goalHullPointsStorage);

    while (contours) {
        if (conf->getShowDebugInfo()) {
            cvDrawContours(img, contours, CV_RGB(255,127,0), CV_RGB(0,255,0),0, 2, CV_AA, cvPoint(0,0));
        }

        obj.area = fabs(cvContourArea(contours));
        // qDebug() << "area " << obj.area;
        if (obj.area >= 300/*MIN_AREA*/) {
            // Find minimal distance between goal and field
            fieldDistMin = INT_MAX;
            cvStartReadSeq(contours, &reader, 0);

            for (i = 0; i < contours->total; i++) {
                CV_READ_SEQ_ELEM(p, reader);
                fieldDist = fabs(cvPointPolygonTest(field, cvPoint2D32f(p.x,p.y), true));

                if (fieldDist < fieldDistMin) fieldDistMin = fieldDist;
            }

            if (fieldDistMin < 50) {
                obj.rect = cvBoundingRect(contours, 0);

                obj.type = itemType;
                color = itemType == BASKET_PURPLE ? colorBasketPurple : colorBasketBlue;

                obj.center.x = obj.rect.x+obj.rect.width / 2;
                obj.center.y = obj.rect.y+obj.rect.height / 2;

                obj.distanceH = obj.center.x - (work->roi->width / 2);

                if (conf->getShowDebugInfo()) {
                    //sprintf(str, "%d", obj.rect.width);
                    //sprintf(str, "%d", (int)obj.area);
                    //sprintf(str, "%d", fieldDistMin);
                    cvPutText(img, str, cvPoint(obj.rect.x, obj.rect.y - 10), &font, CV_RGB(0,255,0));

                    sprintf(str, "%d", (int)obj.area);
                    cvPutText(img, str, cvPoint(obj.rect.x, obj.rect.y + 100), &font, CV_RGB(0,0,255));
                }

                CvBox2D box = cvMinAreaRect2(contours, 0);
                CvPoint2D32f pt[4];
                cvBoxPoints(box, pt);

                cvLine(img, cvPoint(pt[0].x, pt[0].y), cvPoint(pt[1].x, pt[1].y), color, 2, CV_AA, 0);
                cvLine(img, cvPoint(pt[1].x, pt[1].y), cvPoint(pt[2].x, pt[2].y), color, 2, CV_AA, 0);
                cvLine(img, cvPoint(pt[2].x, pt[2].y), cvPoint(pt[3].x, pt[3].y), color, 2, CV_AA, 0);
                cvLine(img, cvPoint(pt[3].x, pt[3].y), cvPoint(pt[0].x, pt[0].y), color, 2, CV_AA, 0);

                cvSeqPush(found_objects, &obj);

                cvStartReadSeq(contours, &reader, 0);

                // Add goal hull points
                for (i = 0; i < contours->total; i++) {
                    CV_READ_SEQ_ELEM(p, reader);
                    cvSeqPush(goalHullPoints, &p);
                }
            }
        }

        // take the next contour
        contours = contours->h_next;
    }

    // Create goal hull form goal hull points
    if (goalHullPoints->total > 0) {
        hull = cvConvexHull2(goalHullPoints, 0, CV_CLOCKWISE, 1);
        //cvDrawContours(img, hull, CV_RGB(255,0,0), CV_RGB(0,255,0),0, 2, CV_AA, cvPoint(0,0));

        cvStartReadSeq(hull, &reader, 0);

        for (i = 0; i < hull->total; i++) {
            CV_READ_SEQ_ELEM(p, reader);
            cvSeqPush(goalHull, &p);
        }
    }

    // cleanup
    cvReleaseMemStorage(&storage);
    cvReleaseMemStorage(&goalHullPointsStorage);
    return;
}

void Image::findField(IplImage* img, IplImage* work, IplImage* binary, ObjectType type, CvSeq* field, int* fieldTop) {
    CvSeq* contours = NULL;
    CvMemStorage* storage = NULL;
    Image::Object obj;

    CvSeq* fieldPoints = NULL;
    CvMemStorage* fieldPointsStorage = NULL;

    CvSeq* hull = NULL;

    int step , step2, channels;
    int heightStartIndex, heightEndIndex;
    int widthStartIndex, widthEndIndex;
    uchar *data1;
    uchar *data2;
    int i, j;
    uchar H, S, L;
    int match;
    int nrOfMatches = 0;

    CvSeqReader reader;
    CvPoint p;

    // get the image data
    step      = work->widthStep;
    channels  = work->nChannels;
    data1     = (uchar *)work->imageData;

    // get the image data
    step2     = binary->widthStep;
    data2     = (uchar *)binary->imageData;

    heightStartIndex = work->roi->yOffset;
    heightEndIndex = work->roi->yOffset + work->roi->height;

    widthStartIndex = work->roi->xOffset;
    widthEndIndex = work->roi->xOffset + work->roi->width;

    for(i = heightStartIndex; i < heightEndIndex; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
        match = 0;

        H = data1[i*step+j*channels+0];
        L = data1[i*step+j*channels+1];
        S = data1[i*step+j*channels+2];

        // if pixel is goal color
        if (H > type.hMin && H < type.hMax &&
                L > type.lMin && L < type.lMax &&
                S > type.sMin) match = 1;

        if (match) {
            nrOfMatches++;
            data2[i*step2+j] = 1;
        } else {
            data2[i*step2+j] = 0;
        }
    }

    if (nrOfMatches < MIN_AREA) return;

    storage = cvCreateMemStorage(0);
    i = cvFindContours(binary, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
    //fprintf(stderr,"Contours: %d\n", i);

    if (i == 0) {
        cvReleaseMemStorage(&storage);
        return;
    }

    fieldPointsStorage = cvCreateMemStorage(0);
    fieldPoints = cvCreateSeq(CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), fieldPointsStorage);

    while (contours) {
        if (conf->getShowDebugInfo()) {
            cvDrawContours(img, contours, CV_RGB(127,127,127), CV_RGB(0,255,0),0, 2, CV_AA, cvPoint(0,0));
        }

        obj.area = fabs(cvContourArea(contours));
        //qDebug() << "area " << obj.area;
        if (obj.area >= 1000) {
            cvStartReadSeq(contours, &reader, 0);

            for (i = 0; i < contours->total; i++) {
                CV_READ_SEQ_ELEM(p, reader);
                cvSeqPush(fieldPoints, &p);
            }
        }

        // take the next contour
        contours = contours->h_next;
    }

    if (fieldPoints->total > 0) {
        hull = cvConvexHull2(fieldPoints, 0, CV_CLOCKWISE, 1);
        cvDrawContours(img, hull, CV_RGB(255,0,0), CV_RGB(0,255,0),0, 2, CV_AA, cvPoint(0,0));

        cvStartReadSeq(hull, &reader, 0);

        for (i = 0; i < hull->total; i++) {
            CV_READ_SEQ_ELEM(p, reader);
            cvSeqPush(field, &p);
        }
    }


    /*
    cvStartReadSeq(fieldPoints, &reader, 0);
    memset(fieldTop, -1, (sizeof(int) * img->width));

    // find image maximum y for every x
    for (i = 0; i < fieldPoints->total; i++) { //i += 10
            CV_READ_SEQ_ELEM(p, reader);

            if (fieldTop[p.x] == -1 || p.y < fieldTop[p.x]) {
                fieldTop[p.x] = p.y;
            }
     } */
    if (fieldPoints->total > 0) {
        cvStartReadSeq(hull, &reader, 0);
        memset(fieldTop, -1, (sizeof(int) * img->width));

        // find image maximum y for every x
        for (i = 0; i < hull->total; i++) { //i += 10
                CV_READ_SEQ_ELEM(p, reader);

                if (fieldTop[p.x] == -1 || p.y < fieldTop[p.x]) {
                    if(p.y < (img->height/2)) {
                    fieldTop[p.x] = p.y;
                    }
                }
         }


        for(int i = 0; i < img->width; i++) {
            cvDrawLine(img, cvPoint(i, fieldTop[i]), cvPoint(i, fieldTop[i]),CV_RGB(0, 143, 255), 2, 1, 0);
        }
    }
    /* */

    // cleanup
    cvReleaseMemStorage(&storage);
    cvReleaseMemStorage(&fieldPointsStorage);

    return;
}

void Image::findFieldEdge(IplImage* img, IplImage* work, IplImage* binary, int* fieldEdge, ObjectType type, int hor) {
        CvSeq* contours = NULL;
        CvMemStorage* storage = NULL;
        Image::Object obj;

        int step , step2, channels;
        int heightStartIndex, heightEndIndex;
        int widthStartIndex, widthEndIndex;
        uchar *data1;
        uchar *data2;
        int i, j;
        uchar S, L;
        int match = 0;
        int nrOfMatches = 0;
        CvSeqReader reader;
        CvPoint p;
        CvPoint pLeft;
        CvPoint pRight;
        int fieldEdgeTemp[img->width];
        float lineLength;

        // get the image data
        step      = work->widthStep;
        channels  = work->nChannels;
        data1     = (uchar *)work->imageData;

        // get the image data
        step2     = binary->widthStep;
        data2     = (uchar *)binary->imageData;

        heightStartIndex = work->roi->yOffset;
        heightEndIndex = work->roi->yOffset + work->roi->height;

        widthStartIndex = work->roi->xOffset;
        widthEndIndex = work->roi->xOffset + work->roi->width;

        for(i = heightStartIndex; i < heightEndIndex; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
                match = 0;
                if (i > hor) {
                        L = data1[i*step+j*channels+1];
                        S = data1[i*step+j*channels+2];

                        // if pixel is black
                        if (L < type.lMax && S < type.sMax) match = 1;
                }

                if (match) {
                        nrOfMatches++;
                        data2[i*step2+j] = 255;
                } else {
                        data2[i*step2+j] = 0;
                }
        }

        if (nrOfMatches < MIN_AREA) return;

        storage = cvCreateMemStorage(0);
        i = cvFindContours(binary, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));
        //fprintf(stderr,"Contours: %d\n", i);

        if (i == 0) {
                cvReleaseMemStorage(&storage);
                return;
        }

        while (contours) {
                obj.area = fabs(cvContourArea(contours, CV_WHOLE_SEQ));

                if (obj.area >= MIN_AREA * 4) {
                        obj.rect = cvBoundingRect(contours, 0);

                        obj.center.x = obj.rect.x+obj.rect.width / 2;
                        obj.center.y = obj.rect.y+obj.rect.height / 2;

                        cvStartReadSeq(contours, &reader, 0);

                        pLeft.x = work->width;
                        pRight.x = 0;
                        memset(fieldEdgeTemp, -1, sizeof(fieldEdgeTemp));

                        // find image maximum y for every x
                        for (i = 0; i < contours->total; i++) {
                                CV_READ_SEQ_ELEM(p, reader);
                                if (p.y > fieldEdge[p.x] && p.y > fieldEdgeTemp[p.x]) fieldEdgeTemp[p.x] = p.y;

                                if (p.x < pLeft.x) {
                                    pLeft.x = p.x;
                                    pLeft.y = p.y;
                                }

                                if (p.x > pRight.x) {
                                    pRight.x = p.x;
                                    pRight.y = p.y;
                                }
                        }

                        // Black contour line length
                        lineLength = sqrt(pow(pLeft.x-pRight.x, 2) + pow(pLeft.y-pRight.y, 2));

                        if (conf->getShowDebugInfo()) {
                            cvLine(img, pLeft, pRight, CV_RGB(255,0,255), 1, 8, 0);
                            sprintf(str, "%d", (int) lineLength);
                            cvPutText(img, str, pLeft, &font, CV_RGB(255,0,255));
                        }

                        if (lineLength > work->width/4 || lineLength >= work->roi->width || lineLength >= work->roi->height) {
                            memcpy(&fieldEdge[pLeft.x], &fieldEdgeTemp[pLeft.x], sizeof(int)*(pRight.x - pLeft.x));
                        }
                }
                // take the next contour
                contours = contours->h_next;
        }

        // draw field edge
        if (conf->getShowDebugInfo()) {
            for(i = widthStartIndex; i < widthEndIndex; i++) {
                        cvLine(img, cvPoint(i, fieldEdge[i]), cvPoint(i, fieldEdge[i] - 3), CV_RGB(0,0,255), 1, 8, 0);
                }
        }

        // cleanup
        cvReleaseMemStorage(&storage);
        return;
}


int Image::checkFieldEdge(IplImage* img, int* fieldEdge, CvPoint pt1, CvPoint pt2, CvPoint * intersectionPt) {
        CvLineIterator iterator;
        int count;

        if (conf->getShowDebugInfo()) cvLine(img, pt1, pt2, CV_RGB(255,0,0), 1, 8, 0);

        count = cvInitLineIterator(img, pt1, pt2, &iterator, 8, 0);

        for (int i = 0; i < count; i++) {
                int offset, x, y;

                offset = iterator.ptr - (uchar*)(img->imageData);
                y = offset/img->widthStep;
                x = (offset - y*img->widthStep)/(3*sizeof(uchar));
                y -= img->roi->yOffset; //if ROI is used
                x -= img->roi->xOffset; //if ROI is used

                if (conf->getShowDebugInfo()) cvLine(img, cvPoint(x,y), cvPoint(x,y), CV_RGB(0,0,0), 1, 8, 0);

                if (fieldEdge[x] > y) {
                    if (intersectionPt != NULL) {
                        intersectionPt->x = x;
                        intersectionPt->y = y;
                    }

                    return 0;
                }

                CV_NEXT_LINE_POINT(iterator);
        }

        return 1;
}

void Image::fastCvtColor(IplImage* imgSrc, CvMat* LUT) {
    int step1, channels, BGR, i, j;
    int heightStartIndex, heightEndIndex;
    int widthStartIndex, widthEndIndex;
    uchar *dataSrc;

    int   step2  = LUT->step/sizeof(uchar);
    uchar *dataLUT = LUT->data.ptr;

    // get the image data
    step1 = imgSrc->widthStep;
    channels = imgSrc->nChannels;
    dataSrc = (uchar *)imgSrc->imageData;

    heightStartIndex = imgSrc->roi->yOffset;
    heightEndIndex = imgSrc->roi->yOffset + imgSrc->roi->height;

    widthStartIndex = imgSrc->roi->xOffset;
    widthEndIndex = imgSrc->roi->xOffset + imgSrc->roi->width;

    int* ptr;
    for(i = heightStartIndex; i < heightEndIndex; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
        ptr = (int*)&dataSrc[i*step1+j*channels];

        BGR = *ptr & 0xFFFFFF;

        memcpy(ptr, &dataLUT[BGR*step2], 3);
    }
}

void Image::fastCvtColor(IplImage* imgSrc, IplImage* imgDest, CvMat* LUT) {
    int step1, channels, BGR, i, j;
    int heightStartIndex, heightEndIndex;
    int widthStartIndex, widthEndIndex;

    uchar *dataSrc;
    uchar *dataDest;

    int   step2  = LUT->step/sizeof(uchar);
    uchar *dataLUT = LUT->data.ptr;

    // get the image data
    step1 = imgSrc->widthStep;
    channels = imgSrc->nChannels;
    dataSrc = (uchar *)imgSrc->imageData;
    dataDest = (uchar *)imgDest->imageData;

    heightStartIndex = imgSrc->roi->yOffset;
    heightEndIndex = imgSrc->roi->yOffset + imgSrc->roi->height;

    widthStartIndex = imgSrc->roi->xOffset;
    widthEndIndex = imgSrc->roi->xOffset + imgSrc->roi->width;

    int* ptrSrc;
    int* ptrDest;
    int index;

    for(i = heightStartIndex; i < heightEndIndex; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
        index = i*step1+j*channels;
        ptrSrc = (int*)&dataSrc[index];
        ptrDest = (int*)&dataDest[index];

        BGR = *ptrSrc & 0xFFFFFF;

        memcpy(ptrDest, &dataLUT[BGR*step2], 3);
    }
}

/**todo comment */
void Image::fastCvtColorFraction(IplImage* imgSrc, IplImage* imgDest, CvMat* LUT, int part, int sumOfParts) {
    int step1, channels, BGR, i, j;
    int heightStartIndex, heightEndIndex;
    int widthStartIndex, widthEndIndex;


//    qDebug() << "CVT COLOR IN THREAD: " << QThread::currentThread(); //Thread id print
    int   step2  = LUT->step/sizeof(uchar);
    uchar *dataLUT = LUT->data.ptr;

    // get the image data
    step1 = imgSrc->widthStep;
    channels = imgSrc->nChannels;

    heightStartIndex = imgSrc->roi->yOffset;
    heightEndIndex = imgSrc->roi->yOffset + imgSrc->roi->height;

    widthStartIndex = imgSrc->roi->xOffset;
    widthEndIndex = imgSrc->roi->xOffset + imgSrc->roi->width;


    uchar *dataSrc;
    uchar *dataDest;
    dataSrc = (uchar *)imgSrc->imageData;
    dataDest = (uchar *)imgDest->imageData;

    int* ptrSrc;
    int* ptrDest;
    int index;

    int startH, endH;

    if(part == 0){
        startH = 0;
        endH = 0;
    }else if(part == 1){
        startH=heightStartIndex;
        endH=(heightEndIndex/sumOfParts);
    }else if(1 < part && part < sumOfParts){
        startH=(heightEndIndex/sumOfParts);
        endH=((heightEndIndex/sumOfParts)*part);
    }else if(part == sumOfParts){
        startH=((heightEndIndex/sumOfParts)*(part-1));
        endH=(heightEndIndex);
    }else{
        qDebug() << "Misuse of function" << endl;
        exit(1);
    }

    for(i = startH; i < endH; i++) for(j = widthStartIndex; j < widthEndIndex; j++) {
        index = i*step1+j*channels;
        ptrSrc = (int*)&dataSrc[index];
        ptrDest = (int*)&dataDest[index];

        BGR = *ptrSrc & 0xFFFFFF;

        memcpy(ptrDest, &dataLUT[BGR*step2], 3);
    }
}

#ifdef XI
void Image::takeWhiteBalance(){
    VisionTime::DO()->markBegin("takeWhiteBalance");
    if (conf->takeWB){

        float kr,kb,kg;

        if (Camera::isCameraEnabled(CAMERA_FRONT)) {
            xiSetParamInt(xiH_front,XI_PRM_MANUAL_WB,1);
            xiGetParamFloat(xiH_front,XI_PRM_WB_KB,&kb);
            xiGetParamFloat(xiH_front,XI_PRM_WB_KR,&kr);
            xiGetParamFloat(xiH_front,XI_PRM_WB_KG,&kg);
            qDebug()<<"Front camera WB parameters are: kr="<<kr<<" kg:"<< kg << " kb:"<< kb;
        }

        if (Camera::isCameraEnabled(CAMERA_LEFT)) {
            xiSetParamInt(xiH_left,XI_PRM_MANUAL_WB,1);
            xiGetParamFloat(xiH_left,XI_PRM_WB_KB,&kb);
            xiGetParamFloat(xiH_left,XI_PRM_WB_KR,&kr);
            xiGetParamFloat(xiH_left,XI_PRM_WB_KG,&kg);
            qDebug()<<"Left camera WB parameters are: kr="<<kr<<" kg:"<< kg << " kb:"<< kb;
        }

        if (Camera::isCameraEnabled(CAMERA_RIGHT)) {
            xiSetParamInt(xiH_right,XI_PRM_MANUAL_WB,1);
            xiGetParamFloat(xiH_right,XI_PRM_WB_KB,&kb);
            xiGetParamFloat(xiH_right,XI_PRM_WB_KR,&kr);
            xiGetParamFloat(xiH_right,XI_PRM_WB_KG,&kg);
            qDebug()<<"Right camera WB parameters are: kr="<<kr<<" kg:"<< kg << " kb:"<< kb;
        }

        conf->takeWB = false;
    }
    VisionTime::DO()->markEnd("takeWhiteBalance");
}

void Image::whiteBalaceSlider(){
    VisionTime::DO()->markBegin("whiteBalanceSlider");
    if ((conf->WB_kr != WB_old_R) || (conf->WB_kg != WB_old_G) || (conf->WB_kb != WB_old_B)){
        xiSetParamFloat(xiH_front,XI_PRM_WB_KR,(float)(*conf->getWbKr()/100.0));
        xiSetParamFloat(xiH_front,XI_PRM_WB_KG,(float)(*conf->getWbKg()/100.0));
        xiSetParamFloat(xiH_front,XI_PRM_WB_KB,(float)(*conf->getWbKb()/100.0));

        if (Camera::isCameraEnabled(CAMERA_LEFT)) {
            xiSetParamFloat(xiH_left,XI_PRM_WB_KR,(float)(*conf->getWbKr()/100.0));
            xiSetParamFloat(xiH_left,XI_PRM_WB_KG,(float)(*conf->getWbKg()/100.0));
            xiSetParamFloat(xiH_left,XI_PRM_WB_KB,(float)(*conf->getWbKb()/100.0));
        }

        if (Camera::isCameraEnabled(CAMERA_RIGHT)) {
            xiSetParamFloat(xiH_right,XI_PRM_WB_KR,(float)(*conf->getWbKr()/100.0));
            xiSetParamFloat(xiH_right,XI_PRM_WB_KG,(float)(*conf->getWbKg()/100.0));
            xiSetParamFloat(xiH_right,XI_PRM_WB_KB,(float)(*conf->getWbKb()/100.0));
        }

        WB_old_G = conf->WB_kg;
        WB_old_R = conf->WB_kr;
        WB_old_B = conf->WB_kb;
    }
    VisionTime::DO()->markEnd("whiteBalanceSlider");
}

#endif

void Image::process(int* fieldTop) {
    CvMemStorage* storage = NULL;

    CvSeq* field = NULL;
    CvMemStorage* fieldStorage = NULL;

    CvSeq* goalYellowHull = NULL;
    CvMemStorage* goalYellowHullStorage = NULL;

    CvSeq* goalBlueHull = NULL;
    CvMemStorage* goalBlueHullStorage = NULL;

    int fieldEdge[img->width];

    if(found_objects != NULL && found_objects->storage != NULL){
        cvReleaseMemStorage(&found_objects->storage);
    }

    storage = cvCreateMemStorage(0);
    found_objects = cvCreateSeq(0, sizeof(CvSeq), sizeof(Object), storage);

    fieldStorage = cvCreateMemStorage(0);
    field = cvCreateSeq(CV_SEQ_CONTOUR, sizeof(CvSeq), sizeof(CvPoint), fieldStorage);

    goalYellowHullStorage = cvCreateMemStorage(0);
    goalYellowHull = cvCreateSeq(CV_SEQ_CONTOUR, sizeof(CvSeq), sizeof(CvPoint), goalYellowHullStorage);

    goalBlueHullStorage = cvCreateMemStorage(0);
    goalBlueHull = cvCreateSeq(CV_SEQ_CONTOUR, sizeof(CvSeq), sizeof(CvPoint), goalBlueHullStorage);

    VisionTime::DO()->markBegin("findField"); 
    findField(img, work, binary, *(conf->getGreen()), field, fieldTop);
    VisionTime::DO()->markEnd("findField");

    VisionTime::DO()->markBegin("findFieldEdge");
    //memset(fieldEdge, -1, sizeof(fieldEdge));
    //findFieldEdge(img, work, binary, fieldEdge, *(conf->getBlack()), minHeight);
    VisionTime::DO()->markEnd("findFieldEdge");

    if(field->total) {
        VisionTime::DO()->markBegin("findYellowGoal");
        findGoal(img, work, binary, *(conf->getYellow()), BASKET_PURPLE, field, goalYellowHull);
        VisionTime::DO()->markEnd("findYellowGoal");

        VisionTime::DO()->markBegin("findBlueGoal");
        findGoal(img, work, binary, *(conf->getBlue()), BASKET_BLUE, field, goalBlueHull);
        VisionTime::DO()->markEnd("findBlueGoal");

        VisionTime::DO()->markBegin("findBall");
        findBall(img, work, binary, *(conf->getOrange()), BALL, field, goalYellowHull, goalBlueHull, fieldEdge);
        VisionTime::DO()->markEnd("findBall");
    }

    cvReleaseMemStorage(&fieldStorage);
    cvReleaseMemStorage(&goalYellowHullStorage);
    cvReleaseMemStorage(&goalBlueHullStorage);

}
#ifdef XI
HANDLE Image::xiH_front=NULL;
HANDLE Image::xiH_left=NULL;
HANDLE Image::xiH_right=NULL;
XI_RETURN Image::stat=0;
#endif
