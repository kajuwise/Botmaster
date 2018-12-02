#include "imagecalibrationitem.h"
#include "config.h"

ImageCalibrationItem::ImageCalibrationItem(const std::string& showName, int * paramValue, const std::string& paramName, QLayout * layout, int min, int max)
{
  //Init: host UI layout(tab), pointer to conf parameter, name of parameter in ini file
  rowHostLayout = layout;
  param = paramValue;
  confParamName = paramName.c_str();

  //Slider numeric value
  QString name = QString(showName.c_str());
  label = new QLabel(name);

  //Slider
  slider = new QSlider(Qt::Horizontal);
  slider->setMinimum(min);
  slider->setMaximum(max);
  slider->setValue(*param);
  slider->setMinimumWidth(200);

  //Hide button
  button = new QToolButton();
  button->setText(QString("Reset"));
  button->setDisabled(true);

  //Save button
  button2 = new QToolButton();
  button2->setText(QString("Save value: ") + QString::number(*param));

  //This is widget for current row
  QWidget *rowWidget = new QWidget();

  //Add UI elements to row layout
  QHBoxLayout * rowLayout = new QHBoxLayout();
  rowLayout->addWidget(label);
  rowLayout->addWidget(button);
  rowLayout->addWidget(slider);
  rowLayout->addWidget(button2);

  //Add row layout to row widget
  rowWidget->setLayout(rowLayout);

  //Add row widget to specified host layout
  rowHostLayout->addWidget(rowWidget);

  //Magically bind slider value to image configuration parameter pointer
  QObject::connect(slider,SIGNAL(valueChanged(int)), this,SLOT( valueChangedSlot(int)));
  QObject::connect(button2,SIGNAL(clicked()), this,SLOT( saveParameter()));
  QObject::connect(button,SIGNAL(clicked()), this,SLOT( resetParameter()));
}

//Slot action for slider value change
void ImageCalibrationItem::valueChangedSlot(int value)
{
//    qDebug() << "Changing slider value to: " << value;
  *param = value;

    QString buttonText = QString("Save value*: ") + QString::number(value);
  button2->setText(buttonText);
  button2->setStyleSheet("* { color: red }");
  button->setDisabled(false);
}

//Slot action for slider value change
void ImageCalibrationItem::saveParameter()
{
    //instance for saving settings - kas 2kki saaks olla hoopis shared?
    QSettings settings(Config::getConfig().colorConfFile, QSettings::IniFormat);

    QString log("Saving parameter \"");
    log.append(confParamName);
    log.append("\". Setting value: ");
    log.append(QString::number(slider->value()));
    log.append(" at ");
    log.append(settings.fileName());

    qDebug() << log;
    settings.setValue(confParamName, (int) slider->value());

   QString buttonText = QString("SAVED value: ") + QString::number(slider->value());
  button2->setText(buttonText);
  button2->setStyleSheet("* { color: rgb(0,153,77) }");
  button->setDisabled(true);
}

//Slot action for resetting slider value
void ImageCalibrationItem::resetParameter()
{
    //instance for saving settings - kas 2kki saaks olla hoopis shared?
    QSettings settings(Config::getConfig().colorConfFile, QSettings::IniFormat);

    QString log("Resetting parameter \"");
    log.append(confParamName);
    log.append("\". Restoring value from: ");
    log.append(settings.fileName());
    log.append(". Got value: ");

    *param = settings.value(confParamName).toInt();
    slider->setValue(*param);
  log.append(QString::number(*param));
  qDebug() << log;
   QString buttonText = QString("Save value: ") + QString::number(slider->value());
  button2->setText(buttonText);
  button2->setStyleSheet("* { color: black }");
  button->setDisabled(true);
}
