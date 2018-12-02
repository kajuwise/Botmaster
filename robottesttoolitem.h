//imagecalibrationitem.h File

#ifndef IMAGECALIBRATIONITEM_H
#define IMAGECALIBRATIONITEM_H
#include<QtGui>
#include<QLayout>
#include<QLabel>
#include<QToolButton>
#include<QSlider>
#include <QDebug>

class RobotTestToolItem : QObject
{
  Q_OBJECT

  public:
    RobotTestToolItem(const std::string&, int *, QLayout *, int, int);

    ~ RobotTestToolItem(){};

  public slots:
    void valueChangedSlot(int value);
  public:
    QLabel* labelName;
    QLabel* labelSliderValue;
    QSlider* slider;

    QLayout* rowHostLayout;
    int * param;
    void setRowHostLayout(QLayout *);
};
#endif
