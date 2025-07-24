#include "pcbdetect_plugin.h"
#include "opencv2/opencv.hpp"
#include <QMessageBox>
#include <imageconvert.h>
#include "pcb_detect.h"
#include <QFile>
PcbDetect_Plugin::PcbDetect_Plugin()
{
    mGenWidget = new PcbDetect();
    PcbUi = Q_NULLPTR;
}

PcbDetect_Plugin::~PcbDetect_Plugin()
{

}

QString PcbDetect_Plugin::name()
{
    return "PcbDetect";
}

QString PcbDetect_Plugin::information()
{
    return "PcbDetect Plugin";
}

int PcbDetect_Plugin::Init()
{
    result_imgs = QVector<QImage>();
    return true;
}


void PcbDetect_Plugin::run_algorithm(QImage qimg, float Confindence, float IouThreshold){
    // 先将输入参数整理为算法动态库接受的格式
    cv::Mat srcImage = QImage2cvMat(qimg);
    cv::Mat dstImage;
    std::vector<PCBDebug::DefectBox> boxes;
    if (srcImage.channels() == 4){
        cv::cvtColor(srcImage, srcImage, cv::COLOR_BGRA2BGR);
    }
    if (srcImage.channels() == 1){
        cv::cvtColor(srcImage, srcImage, cv::COLOR_GRAY2BGR);
    }
    uint32_t status = PCBDebug::DetectPCBDefect(srcImage, dstImage, Confindence, IouThreshold, boxes);
    m_dstImage = cvMat2QImage(dstImage);
    m_dstImages.push_back(m_dstImage);
    allBoxes.push_back(boxes);
    result_imgs.push_back(m_dstImage);
}

QVector<QImage> PcbDetect_Plugin::Execute(QVector<QImage> images, int currentrow)
{
    result_imgs.clear();
    // 1. 图像列表为空，说明没有图片
    if((images.size() == 0) | (currentrow == -999)){
        // QMessageBox::warning(mGenWidget, tr("运行错误"), tr("未选择图片!!!"));
        return result_imgs;
    }
    qDebug() << "开始运行...";
    // 准备输入数据和输出数据
    float Confindence = PcbUi->getConfindence();
    float IouThreshold = PcbUi->getIouThreshold();

    if(currentrow == -1){
        for(QImage qimg:images){
            if(!m_stop){
                run_algorithm(qimg, Confindence, IouThreshold);
            }
            else {
                m_stop = !m_stop;
                break;
            }
        }

    }else {
        run_algorithm(images[currentrow], Confindence, IouThreshold);
    }
    return result_imgs;
}

bool PcbDetect_Plugin::Export(QString dirPath)
{
    for (int i = 0; i < m_dstImages.size(); ++i) {
        const QString imgName = QString("pcb_%1").arg(i, 3, 10, QChar('0'));
        const QString imagePath = QString("%1/%2.png").arg(dirPath).arg(imgName);
        const QString labelPath = QString("%1/%2.txt").arg(dirPath).arg(imgName);

        // 1. 保存对应的图片
        bool success = m_dstImages[i].save(imagePath);
        // 2. 保存对应的 boxes
        const std::vector<PCBDebug::DefectBox>& boxes = allBoxes[i];
        QFile file(labelPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << QString::fromStdString("index") << "   "
                << QString::fromStdString("type") << "   "
                << QString::fromStdString("x") << "   "
                << QString::fromStdString("y") << "   "
                << QString::fromStdString("width") << "   "
                << QString::fromStdString("height") << "   "
                << QString::fromStdString("confidence") << "\n";
            for (int j = 0; j < boxes.size(); ++j) {
                const auto& box = boxes[j];
                out << j+1 << " "                         // 索引
                    << QString::fromStdString(box.label) << " "
                    << box.bbox.x << " "
                    << box.bbox.y << " "
                    << box.bbox.width << " "
                    << box.bbox.height << " "
                    << box.confidence << "\n";
            }
            file.close();
        }
    }
    return true;
}

void PcbDetect_Plugin::requestStop()
{
    m_stop = true;
}

void PcbDetect_Plugin::result_reset()
{
    m_dstImages.clear();
    allBoxes.clear();
    allBoxes.shrink_to_fit();
}


QWidget *PcbDetect_Plugin::GetProcessPanel(QWidget *parent)
{
    PcbUi = qobject_cast<PcbDetect*>(mGenWidget);
    if (PcbUi) {
        customView = PcbUi->getCustomGraphicsView();
    }
    return mGenWidget;
}

CustomGraphicsView *PcbDetect_Plugin::getCustomGraphicsView()
{
    return customView;
}

QVector<QImage> PcbDetect_Plugin::getAllResult()
{
    return m_dstImages;
}
