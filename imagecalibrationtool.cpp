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

  //Green widget and layout
  QVBoxLayout *greenTabLayout = new QVBoxLayout();
  QWidget * greenTabWidget= new QWidget();
  greenTabWidget->setLayout(greenTabLayout);

  //Orange widget and layout
  QVBoxLayout *orangeTabLayout = new QVBoxLayout();
  QWidget * orangeTabWidget= new QWidget();
  orangeTabWidget->setLayout(orangeTabLayout);

  //Blue widget and layout
  QVBoxLayout *blueTabLayout = new QVBoxLayout();
  QWidget * blueTabWidget= new QWidget();
  blueTabWidget->setLayout(blueTabLayout);

  //Yellow widget and layout
  QVBoxLayout *yellowTabLayout = new QVBoxLayout();
  QWidget * yellowTabWidget= new QWidget();
  yellowTabWidget->setLayout(yellowTabLayout);

  //Undefined tab widget and layout
  QVBoxLayout *undefinedTabLayout = new QVBoxLayout();
  QWidget * undefinedTabWidget = new QWidget();
  undefinedTabWidget->setLayout(undefinedTabLayout);

  //WhiteBalance widget and layout
  QVBoxLayout *whiteBalanceTabLayout = new QVBoxLayout();
  QWidget * whiteBalanceTabWidget= new QWidget();
  whiteBalanceTabWidget->setLayout(whiteBalanceTabLayout);

  //Black widget and layout
  QVBoxLayout *blackTabLayout = new QVBoxLayout();
  QWidget * blackTabWidget= new QWidget();
  blackTabWidget->setLayout(blackTabLayout );

  //Add widgets to tabs
  centralWidget->addTab(blueTabWidget, QString("Blue"));
  centralWidget->addTab(yellowTabWidget, QString("Yellow"));
  centralWidget->addTab(greenTabWidget, QString("Green"));
  centralWidget->addTab(orangeTabWidget, QString("Orange"));
  centralWidget->addTab(blackTabWidget, QString("Black"));
  centralWidget->addTab(whiteBalanceTabWidget, QString("White Balance"));
  centralWidget->addTab(undefinedTabWidget, QString("Undefined"));

  //ADD CALIBRATION ROWS HERE
  //ARGUMENT SYNTAX: 1) Visible name of slider (string) 2) parameter in conf (int*) 3)parameter name in conf file (string) 4) tab (QVBoxLayout)
  new ImageCalibrationItem("Blue Hue Min", &(conf->getBlue()->hMin), "blue.hMin", blueTabLayout);
  new ImageCalibrationItem("Blue Hue Max", &(conf->getBlue()->hMax), "blue.hMax", blueTabLayout);
  new ImageCalibrationItem("Blue Sat Min", &(conf->getBlue()->sMin), "blue.sMin", blueTabLayout);
  new ImageCalibrationItem("Blue Sat Min", &(conf->getBlue()->sMax), "blue.sMax", blueTabLayout);
  new ImageCalibrationItem("Blue Light Min", &(conf->getBlue()->lMin), "blue.lMin", blueTabLayout);
  new ImageCalibrationItem("Blue Light Max", &(conf->getBlue()->lMax), "blue.lMax", blueTabLayout);

  new ImageCalibrationItem("Yellow Hue Min", &(conf->getYellow()->hMin), "yellow.hMin", yellowTabLayout);
  new ImageCalibrationItem("Yellow Hue Max", &(conf->getYellow()->hMax), "yellow.hMax", yellowTabLayout);
  new ImageCalibrationItem("Yellow Sat Min", &(conf->getYellow()->sMin), "yellow.sMin", yellowTabLayout);
  new ImageCalibrationItem("Yellow Sat Max", &(conf->getYellow()->sMax), "yellow.sMax", yellowTabLayout);
  new ImageCalibrationItem("Yellow Light Min", &(conf->getYellow()->lMin), "yellow.lMin", yellowTabLayout);
  new ImageCalibrationItem("Yellow Light Max", &(conf->getYellow()->lMax), "yellow.lMax", yellowTabLayout);

  new ImageCalibrationItem("Orange Hue Min", &(conf->getOrange()->hMin), "orange.hMin", orangeTabLayout);
  new ImageCalibrationItem("Orange Hue Max", &(conf->getOrange()->hMax), "orange.hMax", orangeTabLayout);
  new ImageCalibrationItem("Orange Sat Min", &(conf->getOrange()->sMin), "orange.sMin", orangeTabLayout);
  new ImageCalibrationItem("Orange Sat Max", &(conf->getOrange()->sMax), "orange.sMax", orangeTabLayout);
  new ImageCalibrationItem("Orange Light Min", &(conf->getOrange()->lMin), "orange.lMin", orangeTabLayout);
  new ImageCalibrationItem("Orange Light Max", &(conf->getOrange()->lMax), "orange.lMax", orangeTabLayout);

  new ImageCalibrationItem("Green Hue Min", &(conf->getGreen()->hMin), "green.hMin", greenTabLayout);
  new ImageCalibrationItem("Green Hue Max", &(conf->getGreen()->hMax), "green.hMax", greenTabLayout);
  new ImageCalibrationItem("Green Sat Min", &(conf->getGreen()->sMin), "green.sMin", greenTabLayout);
  new ImageCalibrationItem("Green Sat Max", &(conf->getGreen()->sMax), "green.sMax", greenTabLayout);
  new ImageCalibrationItem("Green Light Min", &(conf->getGreen()->lMin), "green.lMin", greenTabLayout);
  new ImageCalibrationItem("Green Light Max", &(conf->getGreen()->lMax), "green.lMax", greenTabLayout);

  new ImageCalibrationItem("Black Hue Min", &(conf->getBlack()->hMin), "black.hMin", blackTabLayout);
  new ImageCalibrationItem("Black Hue Max", &(conf->getBlack()->hMax), "black.hMax", blackTabLayout);
  new ImageCalibrationItem("Black Sat Min", &(conf->getBlack()->sMin), "black.sMin", blackTabLayout);
  new ImageCalibrationItem("Black Sat Max", &(conf->getBlack()->sMax), "black.sMax", blackTabLayout);
  new ImageCalibrationItem("Black Light Min", &(conf->getBlack()->lMin), "black.lMin", blackTabLayout);
  new ImageCalibrationItem("Black Light Max", &(conf->getBlack()->lMax), "black.lMax", blackTabLayout);

  new ImageCalibrationItem("WhiteBalanceKB", &(conf->WB_kb), "WB_kb", whiteBalanceTabLayout);
  new ImageCalibrationItem("WhiteBalanceKG", &(conf->WB_kg), "WB_kg", whiteBalanceTabLayout);
  new ImageCalibrationItem("WhiteBalanceKR", &(conf->WB_kr), "WB_kr", whiteBalanceTabLayout);
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
