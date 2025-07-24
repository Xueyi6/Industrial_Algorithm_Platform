#include "CannyPlugin.h"
#include "cannyui.h"
#include "opencv2/opencv.hpp"
#include "imageconvert.h"
#include "canny_detect.h"
// #include "XLCanny.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>

CannyPlugin::CannyPlugin()
{
    mGenWidget = new CannyUi();
    cannyUi = Q_NULLPTR;
}

CannyPlugin::~CannyPlugin()
{

}

QString CannyPlugin::name()
{
    return "Canny";
}

QString CannyPlugin::information()
{
    return "Canny Plugin";
}

int CannyPlugin::Init()
{
    // 建立一个临时的结果存储
    result_imgs = QVector<QImage>();
    return 0;
}

QVector<QImage> CannyPlugin::getAllResult()
{
    return m_dstImages;
}

bool CannyPlugin::Export(QString dirPath)
{
    for (int i = 0; i < m_dstImages.size(); ++i) {
        QString fileName = QString("%1/canny_%2.png").arg(dirPath).arg(i, 3, 10, QChar('0'));
        bool success = m_dstImages[i].save(fileName, "PNG");

        if (!success) {
            qWarning() << "保存失败: " << fileName;
        }
    }
    return true;
}

void CannyPlugin::requestStop()
{
    m_stop = true;
}

void CannyPlugin::result_reset()
{
    m_dstImages.clear();
}

void CannyPlugin::run_algorithm(QImage qimg, int max_threshold, int min_threshold, int kernel_sie, bool L2){
    cv::Mat srcImage = QImage2cvMat(qimg);
    cv::Mat dstImage;
    CannyDebug::EdgeInfo resInfo;
    if (cannyUi->get_api_checked()){
        cv::Canny(srcImage,dstImage,max_threshold, min_threshold, kernel_sie, L2);
    }
    else{
        CannyDebug::APCanny(srcImage, dstImage, resInfo, max_threshold, min_threshold, kernel_sie, L2);
    }
    // XiangLong::XLCanny(srcImage, dstImage, resInfo, max_threshold, min_threshold, kernel_sie, L2);
    m_dstImage = cvMat2QImage(dstImage);
    m_dstImages.push_back(m_dstImage);
    result_imgs.push_back(m_dstImage);
}

QVector<QImage> CannyPlugin::Execute(QVector<QImage> images, int currentrow)
{
    result_imgs.clear();
    // 1. 图像列表为空或者索引为-999，说明没有上传或选择选测定图片
    if((images.size() == 0) | (currentrow == -999)){
        // QMessageBox::warning(mGenWidget, tr("运行错误"), tr("未选择图片!!!"));
        return result_imgs;
    }
    // 获取参数
    int max_threshold = cannyUi->get_max_threshold();
    int min_threshold = cannyUi->get_min_threshold();
    int kernel_sie = cannyUi->get_kernel_size();
    bool L2 = cannyUi->get_paradigm();
    // 2. 图像列表不为空，看current_row
    //      2.1 current_row = -1 --> 说明全部运行
    //      2.2 current_row！= -1 --> 说明运行所选图片索引
    if(currentrow == -1){
        for(QImage qimag:images){
            if(!m_stop){
                run_algorithm(qimag, max_threshold, min_threshold, kernel_sie, L2);
            }
            else {
                m_stop = !m_stop;
                break;
            }
        }
    }else {
        run_algorithm(images[currentrow], max_threshold, min_threshold, kernel_sie, L2);
    }
    return result_imgs;
}

CustomGraphicsView *CannyPlugin::getCustomGraphicsView()
{
    return customView;
}

QWidget *CannyPlugin::GetProcessPanel(QWidget *parent)
{
    cannyUi = qobject_cast<CannyUi*>(mGenWidget);
    if (cannyUi) {
        customView = cannyUi->getCustomGraphicsView();
    }
    return mGenWidget;
}
