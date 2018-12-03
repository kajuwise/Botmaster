#include "robottesttoolitem.h"
#include "config.h"

RobotTestToolItem::RobotTestToolItem(const std::string& showName, int * paramValue, QLayout * layout, int min, int max) {
    //Init: host UI layout(tab), pointer to conf parameter, name of parameter in ini file
    rowHostLayout = layout;
    param = paramValue;

    //Slider numeric value
    QString name = QString(showName.c_str());
    labelName = new QLabel(name);
    labelSliderValue = new QLabel(QString::number(* param));

    //Slider
    slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(min);
    slider->setMaximum(max);
    slider->setValue(*param);
    slider->setMinimumWidth(200);

    //This is widget for current row
    QWidget *rowWidget = new QWidget();

    //Add UI elements to row layout
    QHBoxLayout * rowLayout = new QHBoxLayout();
    rowLayout->addWidget(labelName);
    rowLayout->addWidget(slider);
    rowLayout->addWidget(labelSliderValue);

    //Add row layout to row widget
    rowWidget->setLayout(rowLayout);

    //Add row widget to specified host layout
    rowHostLayout->addWidget(rowWidget);
    QObject::connect(slider,SIGNAL(valueChanged(int)), this,SLOT( valueChangedSlot(int)));
    }

//Slot action for slider value change
void RobotTestToolItem::valueChangedSlot(int value)
{
    *param = value;
    labelSliderValue->setText(QString::number(* param));
}
