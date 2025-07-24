#include "photometric_ui.h"
#include "ui_photometric_ui.h"

Photometric_ui::Photometric_ui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Photometric_ui)
{
    ui->setupUi(this);
    if (InitPhotometric() == false){
        throw std::bad_alloc();
    };
}

bool Photometric_ui::InitPhotometric()
{
    customview = new CustomGraphicsView();
    if (!ui->img_show_widget->layout()) {
        // 创建一个新的布局，例如水平布局
        QHBoxLayout *layout = new QHBoxLayout(ui->img_show_widget);
        layout->setContentsMargins(0, 0, 0, 0); // 去掉边距
        ui->img_show_widget->setLayout(layout);
    }
    ui->img_show_widget->layout()->addWidget(customview);
    return true;

}

Photometric_ui::~Photometric_ui()
{
    delete ui;
}


CustomGraphicsView *Photometric_ui::getCustomGraphicsView()
{
    return customview;
}

QList<float> Photometric_ui::getSlant()
{
    Slants.push_back(ui->SlantBox_1->value());
    Slants.push_back(ui->SlantBox_2->value());
    Slants.push_back(ui->SlantBox_3->value());
    Slants.push_back(ui->SlantBox_4->value());
    return Slants;
}

QList<float> Photometric_ui::getTilt()
{
    Tilts.push_back(ui->TiltBox_1->value());
    Tilts.push_back(ui->TiltBox_2->value());
    Tilts.push_back(ui->TiltBox_3->value());
    Tilts.push_back(ui->TiltBox_4->value());
    return Tilts;
}
