#include "robottesttool.h"
#include "robottesttoolitem.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>

RobotTestTool::RobotTestTool(int * omniTestDirDeg, int * omniTestVelocityBody, int * omniTestVelocityAngular, int * throwerTestPwm, int* throwerTestRpm)
{
  Config & temp =Config::getConfig();
  conf = & temp;

  //Home widget for calibration tool
  QTabWidget * centralWidget = new QTabWidget(this);
  setCentralWidget(centralWidget);


  QVBoxLayout * omniTabLayout = new QVBoxLayout();
  QWidget * omniTabWidget= new QWidget();
  omniTabWidget->setLayout(omniTabLayout);

  QVBoxLayout * throwerTabLayout = new QVBoxLayout();
  QWidget * throwerTabWidget= new QWidget();
  throwerTabWidget->setLayout(throwerTabLayout);

  //Add widgets to tabs
  centralWidget->addTab(omniTabWidget, QString("Omni test"));
  centralWidget->addTab(throwerTabWidget, QString("Thrower test"));

  /* Omni cheatsheet
   * dirDeg - direction of movement (0 is forward, 90 is sideways to the right)
   * velocityBody: drive speed
   * velocityAngular: spinning speed
  */
  //Omni test
  new RobotTestToolItem("Omni dirDeg", omniTestDirDeg, omniTabLayout, -255, 255);
  new RobotTestToolItem("Omni velocityBody", omniTestVelocityBody, omniTabLayout, -255, 255);
  new RobotTestToolItem("Omni velocityAngular", omniTestVelocityAngular, omniTabLayout, -255, 255);

  //Thrower test
  new RobotTestToolItem("Roller pwm", throwerTestPwm, throwerTabLayout, 0, 256);
  new RobotTestToolItem("Thrower RPM", throwerTestRpm, throwerTabLayout, 0, 25000);
}

void RobotTestTool::toggleBool(bool * boolean) {
    qDebug() << "changing boolean value to: " << *boolean;
    * boolean = ! *boolean;
    qDebug() << "New value is: " << *boolean;
}
