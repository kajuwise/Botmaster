//imagecalibrationitem.h File

#ifndef IMAGECALIBRATIONITEM_H
#define IMAGECALIBRATIONITEM_H
#include<QtGui>
#include<QLayout>
#include<QLabel>
#include<QToolButton>
#include<QSlider>

class ImageCalibrationItem : QObject
{
  Q_OBJECT

  public:
    ImageCalibrationItem(const std::string&, int *, const std::string&, QLayout *);

    ~ ImageCalibrationItem(){};

  public slots:
    void valueChangedSlot(int value);
    void saveParameter();
    void resetParameter();
  public:
    QLabel* label;
    QLabel* label2;
    QToolButton* button;
    QToolButton* button2;
    QSlider* slider;
    QSlider* slider2;

    QLayout* rowHostLayout;
    int * param;
    void setParam(int*);
    void setRowHostLayout(QLayout *);
    QString confParamName;
};
#endif
