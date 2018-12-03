//#include <QtCore/QCoreApplication>
#include <QApplication>
#include "server.h"
#include <iostream>
#include "abstractrobot.h"
#include "robotfactory.h"
#include "config.h"
#include "visiontime.h"
#include "imagecalibrationtool.h"

using namespace std;

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString & msg)
{
    QString txt = QTime::currentTime().toString("hh:mm:ss.zzz ");
    switch (type) {
    case QtDebugMsg:
        txt += QString("Debug: %1").arg(msg);
        break;
    case QtWarningMsg:
        txt += QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt += QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt += QString("Fatal: %1").arg(msg);
        break;
    }
    // QFile outFile("~/Desktop/log.txt");
    QFile outFile("/home/robot/Desktop/log.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{

    //Visiontime init
    //VisionTime::DO()->turnOn();
    VisionTime::DO()->setStartMeasuringFromFrame(100);
    VisionTime::DO()->setMeasureNumberOfFrames(30);

//   QCoreApplication a(argc, argv);
    QApplication a(argc,argv);

    Config  & conf  = Config::getConfig(/*argn,argv*/);
    conf.setConfig(argc,argv);

    // ?? cout << "new QTcpServer" << endl;

    AbstractRobot * robot = RobotFactory::getRobot(conf.getRobotName());

    //Slider stuff
//    QApplication app(argc, argv);
//      qInstallMessageHandler(myMessageHandler); // text file logging

    ImageCalibrationTool *window = new ImageCalibrationTool();
    window->setWindowTitle(QString::fromUtf8("Botmaster 2k18 Image Calibration tool"));
    window->show();

    //Slider stuff end

    robot->go();

    return a.exec();
}
