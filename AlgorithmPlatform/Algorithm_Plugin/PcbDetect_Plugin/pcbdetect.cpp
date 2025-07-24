#include "pcbdetect.h"
#include "ui_pcbdetect.h"
#include "pcb_detect.h"
#include <QDir>
#include<QDebug>

PcbDetect::PcbDetect(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PcbDetect)
{
    ui->setupUi(this);
    if (InitPcbDetect() == false){
        throw std::bad_alloc();
    }
    preModel = ui->modelBox->currentText();
    InitModel(preModel);
}

PcbDetect::~PcbDetect()
{
    delete ui;
}

bool PcbDetect::InitPcbDetect()
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

CustomGraphicsView *PcbDetect::getCustomGraphicsView()
{
    return customview;
}

QString PcbDetect::getModel()
{
    QString model_name = ui->modelBox->currentText();
    return model_name;
}

float PcbDetect::getConfindence()
{
    float confidence = ui->confidenceBox->value();
    return confidence;
}

float PcbDetect::getIouThreshold()
{
    float iouThreshold = ui->iouBox->value();
    return iouThreshold;
}

bool PcbDetect::InitModel(QString modelName)
{
    QDir pluginsdir(QDir::currentPath());
    pluginsdir.cdUp();  // 进入上一级目录
    QString model = pluginsdir.filePath(modelPathMap[modelName]);  // 拼接完整路径;
    return PCBDebug::InitDetector(model.toStdString());

}


void PcbDetect::on_modelBox_currentIndexChanged(const QString &model)
{
    qDebug() << "model_name:" << model;
    InitModel(model);
    return;
}
