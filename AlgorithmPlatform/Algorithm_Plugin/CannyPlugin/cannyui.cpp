#include "cannyui.h"
#include "ui_cannyui.h"
#include "CustomGraphicsView.h"


CannyUi::CannyUi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CannyUi),
    customview(Q_NULLPTR)
{
    ui->setupUi(this);
    if (InitCanny() == false){
        throw std::bad_alloc();
    };

}

CannyUi::~CannyUi()
{
    delete ui;
}

bool CannyUi::InitCanny()
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

CustomGraphicsView *CannyUi::getCustomGraphicsView()
{

    return customview;
}

int CannyUi::get_max_threshold()
{
    max_threshold = ui->max_threshold_box->value();
    return max_threshold;
}

int CannyUi::get_min_threshold()
{
    min_threshold = ui->min_threshold_box->value();
    return min_threshold;
}

int CannyUi::get_kernel_size()
{
    kernel_size = ui->kernel_size_box->currentText();
    if (kernel_size == "3Size"){
        return 3;
    }
    if (kernel_size == "5Size"){
        return 5;
    }
    if (kernel_size == "7Size"){
        return 5;
    }
    return 3;
}

bool CannyUi::get_paradigm()
{
    paradigm = ui->paradigm_box->currentText();
    if (paradigm == "L1范式"){
        return false;
    }
    return true;
}
bool CannyUi::get_api_checked(){
    return ui->canny_api->isChecked();
}
