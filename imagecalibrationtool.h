//imagecalibrationtool.h File

#ifndef IMAGECALIBRATIONTOOL_H
#define IMAGECALIBRATIONTOOL_H
#include<QtGui>
#include "config.h"
#include "imagecalibrationitem.h"
#include <QMainWindow>

class ImageCalibrationTool: public QMainWindow
{
  Q_OBJECT

  public:
    ImageCalibrationTool();

    ~ ImageCalibrationTool(){};

  public slots:
    void helpMenuAbout();
    void helpMenuDoNotPress();
  public:



   private:
    Config * conf;
};
#endif
