//imagecalibrationtool.h File

#ifndef IMAGECALIBRATIONTOOL_H
#define IMAGECALIBRATIONTOOL_H
#include<QtGui>
#include "config.h"
#include <QMainWindow>

class RobotTestTool: public QMainWindow
{
  Q_OBJECT

  public:
    RobotTestTool(int * omniTestDirDeg, int * omniTestVelocityBody, int * omniTestVelocityAngular, int * throwerTestPwm);
    ~ RobotTestTool(){};
    void toggleBool(bool *);

  public slots:
  public:

  private:
    Config * conf;
};
#endif
