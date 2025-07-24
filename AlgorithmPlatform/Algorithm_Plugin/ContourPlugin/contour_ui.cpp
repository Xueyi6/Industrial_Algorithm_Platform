#include "contour_ui.h"
#include "ui_contour_ui.h"

QString Contour_ui::get_contour_display_model()
{
    return ui->Contour_display_model->currentText();
}

QString Contour_ui::get_contour_detect_model()
{
    return ui->Contour_detect_model->currentText();
}

int Contour_ui::get_gauss_size()
{
    QString gauss_size = ui->gauss_size->currentText();
    if (gauss_size == "3Size"){
        return 3;
    }
    if (gauss_size == "5Size"){
        return 5;
    }
    if (gauss_size == "7Size"){
        return 3;
    }
    return 3;
}

int Contour_ui::get_max_threshold()
{
    return ui->max_threshold->value();
}

int Contour_ui::get_min_threshold()
{
    return ui->min_threshold->value();
}

int Contour_ui::get_segment_threshold()
{
    return ui->min_seg_threshold->value();
}


double Contour_ui::get_min_area_threshold()
{
    return ui->min_area_threshold->value();
}

bool Contour_ui::is_canny()
{
    return ui->is_canny->isChecked();
}

bool Contour_ui::get_is_highlight()
{
    return ui->Is_highlight_main->isChecked();
}

bool Contour_ui::get_is_gauss()
{
    return ui->is_gauss->isChecked();
}

bool Contour_ui::drawRotatedRect()
{
    return ui->Is_drawRotatedRect->isChecked();
}

bool Contour_ui::drawArea()
{
    return ui->is_drawArea->isChecked();
}

bool Contour_ui::drawBoundingRect()
{
    return ui->Is_drawBoundingRect->isChecked();
}

Contour_ui::Contour_ui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Contour_ui)
{
    ui->setupUi(this);
    if (InitContour() == false){
        throw std::bad_alloc();
    }
}

Contour_ui::~Contour_ui()
{
    delete ui;
}

bool Contour_ui::InitContour()
{
    customview = new CustomGraphicsView();
    if (!ui->img_show_widget->layout()) {
        // 创建一个新的布局，例如水平布局
        QHBoxLayout *layout = new QHBoxLayout(ui->img_show_widget);
        layout->setContentsMargins(0, 0, 0, 0); // 去掉边距
        ui->img_show_widget->setLayout(layout);
    }
    ui->img_show_widget->layout()->addWidget(customview);
    ui->is_canny->setChecked(true);
    ui->max_threshold->setEnabled(true);
    ui->min_threshold->setEnabled(true);

    ui->is_segment->setChecked(false);
    ui->min_seg_threshold->setEnabled(false);

    ui->gauss_size->setEnabled(false);
    return true;
}

CustomGraphicsView *Contour_ui::getCustomGraphicsView()
{
    return customview;
}

void Contour_ui::on_is_segment_clicked(bool checked)
{
    ui->min_seg_threshold->setEnabled(checked);
    // canny和阈值分割只能启动一种，当选了阈值分割，canny就不能触发
    ui->is_canny->setChecked(!checked);
    ui->max_threshold->setEnabled(!checked);
    ui->min_threshold->setEnabled(!checked);
}

void Contour_ui::on_is_gauss_clicked(bool checked)
{
    ui->gauss_size->setEnabled(checked);
}

void Contour_ui::on_is_canny_clicked(bool checked)
{
    ui->max_threshold->setEnabled(checked);
    ui->min_threshold->setEnabled(checked);
    // canny和阈值分割只能启动一种，当选了canny，阈值分割就不能触发
    ui->is_segment->setChecked(!checked);
    ui->min_seg_threshold->setEnabled(!checked);
}
