#include "imagecalibrationtool.h"
#include <QMessageBox>
#include <QMenu>
#include <QAction>

ImageCalibrationTool::ImageCalibrationTool()
{
  Config & temp =Config::getConfig();
  conf = & temp;

  //Home widget for calibration tool
  QTabWidget *centralWidget = new QTabWidget(this);
  setCentralWidget(centralWidget);


  //Menu
  /*
  QMenu *menu = menuBar()->addMenu(tr("&Help"));
  QAction *aboutAction = new QAction(tr("&About"), this);
  menu->addAction(aboutAction);
  menu->addSeparator();
  QAction *doNotPressAction = new QAction(tr("don't press this one"), this);
  menu->addAction(doNotPressAction);
  QObject::connect(aboutAction, SIGNAL(triggered()), this, SLOT(helpMenuAbout()));
  QObject::connect(doNotPressAction, SIGNAL(triggered()), this, SLOT(helpMenuDoNotPress()));
   */

  QVBoxLayout *fieldTabLayout = new QVBoxLayout();
  QWidget * greenTabWidget= new QWidget();
  greenTabWidget->setLayout(fieldTabLayout);

  QVBoxLayout *ballTabLayout = new QVBoxLayout();
  QWidget * orangeTabWidget= new QWidget();
  orangeTabWidget->setLayout(ballTabLayout);

  QVBoxLayout *blueGoalTabLayout = new QVBoxLayout();
  QWidget * blueTabWidget= new QWidget();
  blueTabWidget->setLayout(blueGoalTabLayout);

  QVBoxLayout *purpleGoalTabLayout = new QVBoxLayout();
  QWidget * yellowTabWidget= new QWidget();
  yellowTabWidget->setLayout(purpleGoalTabLayout);

  QVBoxLayout *undefinedTabLayout = new QVBoxLayout();
  QWidget * undefinedTabWidget = new QWidget();
  undefinedTabWidget->setLayout(undefinedTabLayout);

  QVBoxLayout *whiteBalanceTabLayout = new QVBoxLayout();
  QWidget * whiteBalanceTabWidget= new QWidget();
  whiteBalanceTabWidget->setLayout(whiteBalanceTabLayout);

  QVBoxLayout *blackTabLayout = new QVBoxLayout();
  QWidget * blackTabWidget= new QWidget();
  blackTabWidget->setLayout(blackTabLayout );

  QVBoxLayout *imageRoiLayout = new QVBoxLayout();
  QWidget * imageRoiTabWidget= new QWidget();
  imageRoiTabWidget->setLayout(imageRoiLayout);

  //Add widgets to tabs
  centralWidget->addTab(blueTabWidget, QString("Blue Goal"));
  centralWidget->addTab(yellowTabWidget, QString("Purple Goal"));
  centralWidget->addTab(greenTabWidget, QString("Field"));
  centralWidget->addTab(orangeTabWidget, QString("Ball"));
  centralWidget->addTab(blackTabWidget, QString("Black"));
  centralWidget->addTab(whiteBalanceTabWidget, QString("White Balance"));
  centralWidget->addTab(imageRoiTabWidget, QString("Image ROI"));
  centralWidget->addTab(undefinedTabWidget, QString("Undefined"));

  //ADD CALIBRATION ROWS HERE
  //ARGUMENT SYNTAX: 1) Visible name of slider (string) 2) parameter in conf (int*) 3)parameter name in conf file (string) 4) tab (QVBoxLayout)
  new ImageCalibrationItem("Blue Goal Hue Min", &(conf->getBlue()->hMin), "blue.hMin", blueGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Blue Goal Hue Max", &(conf->getBlue()->hMax), "blue.hMax", blueGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Blue Goal Sat Min", &(conf->getBlue()->sMin), "blue.sMin", blueGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Blue Goal Sat Max", &(conf->getBlue()->sMax), "blue.sMax", blueGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Blue Goal Light Min", &(conf->getBlue()->lMin), "blue.lMin", blueGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Blue Goal Light Max", &(conf->getBlue()->lMax), "blue.lMax", blueGoalTabLayout, 0 , 255);

  new ImageCalibrationItem("Purple Goal Hue Min", &(conf->getYellow()->hMin), "yellow.hMin", purpleGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Purple Goal Hue Max", &(conf->getYellow()->hMax), "yellow.hMax", purpleGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Purple Goal Sat Min", &(conf->getYellow()->sMin), "yellow.sMin", purpleGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Purple Goal Sat Max", &(conf->getYellow()->sMax), "yellow.sMax", purpleGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Purple Goal Goal Light Min", &(conf->getYellow()->lMin), "yellow.lMin", purpleGoalTabLayout, 0 , 255);
  new ImageCalibrationItem("Yellow Light Max", &(conf->getYellow()->lMax), "yellow.lMax", purpleGoalTabLayout, 0 , 255);

  new ImageCalibrationItem("Ball Hue Min", &(conf->getOrange()->hMin), "orange.hMin", ballTabLayout, 0 , 255);
  new ImageCalibrationItem("Ball Hue Max", &(conf->getOrange()->hMax), "orange.hMax", ballTabLayout, 0 , 255);
  new ImageCalibrationItem("Ball Sat Min", &(conf->getOrange()->sMin), "orange.sMin", ballTabLayout, 0 , 255);
  new ImageCalibrationItem("Ball Sat Max", &(conf->getOrange()->sMax), "orange.sMax", ballTabLayout, 0 , 255);
  new ImageCalibrationItem("Ball Light Min", &(conf->getOrange()->lMin), "orange.lMin", ballTabLayout, 0 , 255);
  new ImageCalibrationItem("Ball Light Max", &(conf->getOrange()->lMax), "orange.lMax", ballTabLayout, 0 , 255);

  new ImageCalibrationItem("Field Hue Min", &(conf->getGreen()->hMin), "green.hMin", fieldTabLayout, 0 , 255);
  new ImageCalibrationItem("Field Hue Max", &(conf->getGreen()->hMax), "green.hMax", fieldTabLayout, 0 , 255);
  new ImageCalibrationItem("Field Sat Min", &(conf->getGreen()->sMin), "green.sMin", fieldTabLayout, 0 , 255);
  new ImageCalibrationItem("Field Sat Max", &(conf->getGreen()->sMax), "green.sMax", fieldTabLayout, 0 , 255);
  new ImageCalibrationItem("Field Light Min", &(conf->getGreen()->lMin), "green.lMin", fieldTabLayout, 0 , 255);
  new ImageCalibrationItem("Field Light Max", &(conf->getGreen()->lMax), "green.lMax", fieldTabLayout, 0 , 255);

  new ImageCalibrationItem("Black Hue Min", &(conf->getBlack()->hMin), "black.hMin", blackTabLayout, 0 , 255);
  new ImageCalibrationItem("Black Hue Max", &(conf->getBlack()->hMax), "black.hMax", blackTabLayout, 0 , 255);
  new ImageCalibrationItem("Black Sat Min", &(conf->getBlack()->sMin), "black.sMin", blackTabLayout, 0 , 255);
  new ImageCalibrationItem("Black Sat Max", &(conf->getBlack()->sMax), "black.sMax", blackTabLayout, 0 , 255);
  new ImageCalibrationItem("Black Light Min", &(conf->getBlack()->lMin), "black.lMin", blackTabLayout, 0 , 255);
  new ImageCalibrationItem("Black Light Max", &(conf->getBlack()->lMax), "black.lMax", blackTabLayout, 0 , 255);

  new ImageCalibrationItem("WhiteBalanceKB", &(conf->WB_kb), "WB_kb", whiteBalanceTabLayout, 0 , 255);
  new ImageCalibrationItem("WhiteBalanceKG", &(conf->WB_kg), "WB_kg", whiteBalanceTabLayout, 0 , 255);
  new ImageCalibrationItem("WhiteBalanceKR", &(conf->WB_kr), "WB_kr", whiteBalanceTabLayout, 0 , 255);

  new ImageCalibrationItem("Thrower x from", &(conf->getBlackout()->throwerXFrom), "blackout.thrower.x.from", imageRoiLayout, 0 , 1250);
  new ImageCalibrationItem("Thrower x to", &(conf->getBlackout()->throwerXTo), "blackout.thrower.x.to", imageRoiLayout, 0 , 1250);
  new ImageCalibrationItem("Thrower y", &(conf->getBlackout()->throwerY), "blackout.thrower.y", imageRoiLayout, 0, 650);
  new ImageCalibrationItem("Left corner x", &(conf->getBlackout()->cornerLeftX), "blackout.corner.left.x", imageRoiLayout, 0, 1250);
  new ImageCalibrationItem("Left corner y", &(conf->getBlackout()->cornerLeftY), "blackout.corner.left.y", imageRoiLayout, 0, 650);
  new ImageCalibrationItem("Right corner x", &(conf->getBlackout()->cornerRightX), "blackout.corner.right.x", imageRoiLayout, 0, 1250);
  new ImageCalibrationItem("Right corner y", &(conf->getBlackout()->cornerRightY), "blackout.corner.right.y", imageRoiLayout, 0, 650);
}

void ImageCalibrationTool::helpMenuAbout()
{
    QString about;
    about.append("Calibrate your Botmaster image and save parameters easily! \n\n");
    about.append("Botmaster Image Calibration Tool - Special tool for Botmaster image and camera calibration. ");
    about.append("Values are taken from 'Config' class and are written to '' configuration file in botmaster root folder.\n\n");
    about.append("If you want to add new tabs or sliders, go to 'imagecalibrationtool.cpp' and follow this simple syntax:\n\n");
    about.append("new ImageCalibrationItem(<Name of slider (string)>, &(conf-> <Parameter in 'Config' class (integer)>), <desired tab(QLayout)>);\n\n");
    about.append("For example:\n");
    about.append("new ImageCalibrationItem(\"Green Hue Min\", &(conf->getGreen()->hMin), greenTabLayout);");
    about.append("\n\nor ask me:");
    about.append("\n\n Erik Kaju \n <erik.kaju@gmail.com> \n 2013");

    QMessageBox::information(0, QString("About"), about, QMessageBox::Ok);
}

void ImageCalibrationTool::helpMenuDoNotPress()
{
    QMessageBox::information(0, QString("About"), QString("5 kittens just died!\n:("), QMessageBox::Ok);
}
