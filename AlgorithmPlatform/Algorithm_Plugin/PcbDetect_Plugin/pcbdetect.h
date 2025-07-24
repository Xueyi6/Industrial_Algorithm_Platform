#ifndef PCBDETECT_H
#define PCBDETECT_H

#include <QWidget>
#include <CustomGraphicsView.h>
namespace Ui {
class PcbDetect;
}

class PcbDetect : public QWidget
{
    Q_OBJECT

public:
    explicit PcbDetect(QWidget *parent = nullptr);
    ~PcbDetect();
    bool InitPcbDetect();
    CustomGraphicsView * getCustomGraphicsView();
    QString getModel();
    float getConfindence();
    float getIouThreshold();
    bool InitModel(QString modelName);

private slots:
    void on_modelBox_currentIndexChanged(const QString &arg1);

private:
    Ui::PcbDetect *ui;
    CustomGraphicsView * customview;
    QString preModel;
    // 模型路径映射
    std::map<QString, QString> modelPathMap = {
        {"YOLOv5", "models/yolov5.onnx"},
        {"YOLOv8", "models/yolov8.onnx"}
    };
};

#endif // PCBDETECT_H
