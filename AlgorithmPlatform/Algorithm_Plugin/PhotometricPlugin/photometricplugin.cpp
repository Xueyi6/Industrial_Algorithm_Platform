#include "photometricplugin.h"
#include "PhotometricStereo.h"
#include "opencv2/opencv.hpp"
#include "photometric_ui.h"
#include "imageconvert.h"
#include <QMessageBox>
#include <QDebug>

PhotometricPlugin::PhotometricPlugin()
{
    mGenWidget = new Photometric_ui();;
    photometric_ui = Q_NULLPTR;
}

PhotometricPlugin::~PhotometricPlugin()
{

}

QString PhotometricPlugin::name()
{
    return "PhotometricStereo";
}

QString PhotometricPlugin::information()
{
    return "Photometric Plugin";
}

int PhotometricPlugin::Init()
{
    return 0;
}

QVector<QImage> PhotometricPlugin::getAllResult()
{
    return m_dstImages;
}

bool PhotometricPlugin::Export(QString dirPath)
{
    for (int i = 0; i < m_dstImages.size(); ++i) {
        QString fileName = QString("%1/photometric_%2.png").arg(dirPath).arg(i, 3, 10, QChar('0'));
        bool success = m_dstImages[i].save(fileName, "PNG");

        if (!success) {
            qWarning() << "保存失败: " << fileName;
        }
    }
    return true;
}

void PhotometricPlugin::requestStop()
{
    m_stop = true;
}

void PhotometricPlugin::result_reset()
{
    m_dstImages.clear();
}

QVector<QImage> PhotometricPlugin::Execute(QVector<QImage> images, int currentrow)
{
    result_imgs.clear();
    if(images.size() < 4){
        // QMessageBox::warning(mGenWidget, tr("运行错误"),tr("图片数量不足，数量需: %1").arg(4));
        return result_imgs;
    }
    qDebug() << "开始运行...";
    //准备输入数据和输出数据
    std::vector<cv::Mat> srcImages;
    std::vector<float> srcSlants;
    std::vector<float> srcTilts;
    cv::Mat dstHeightFieldImg;
    cv::Mat dstGradientImg;
    cv::Mat dstAlbedoImg;
    QList<float> m_Slants = photometric_ui->getSlant();
    QList<float> m_Tilts = photometric_ui->getTilt();
    //先将输入参数整理为算法动态库接受的格式
    // for(int i = 0; i < images.size(); i++)
    for(int i = 0; i < 4; i++)
    {
        qDebug() << "开始循环获取图片...";
        QImage qsrcImage = images.at(i).convertToFormat(QImage::Format_Grayscale8);
        cv::Mat srcImage = QImage2cvMat(qsrcImage);
        srcImages.push_back(srcImage);
        srcSlants.push_back(m_Slants.at(i));
        srcTilts.push_back(m_Tilts.at(i));
    }
    qDebug() << "开始运行算法...";
    //运行算法
    PhotometricStereoDebug::PhotometricStereo(srcImages,
                               dstHeightFieldImg,
                               dstGradientImg,
                               dstAlbedoImg,
                               srcImages.size(),
                               srcSlants,
                               srcTilts);
    qDebug() << "运行算法成功...";
    //将算法结果转换并保存
    m_dstImages.push_back(cvMat2QImage(dstHeightFieldImg));
    m_dstImages.push_back(cvMat2QImage(dstGradientImg));
    m_dstImages.push_back(cvMat2QImage(dstAlbedoImg));

    result_imgs.push_back(cvMat2QImage(dstAlbedoImg));
    result_imgs.push_back(cvMat2QImage(dstHeightFieldImg));
    result_imgs.push_back(cvMat2QImage(dstGradientImg));

    // customView->SetImage(cvMat2QImage(dstAlbedoImg));
    return result_imgs;
}

CustomGraphicsView *PhotometricPlugin::getCustomGraphicsView()
{
    return customView;
}

QWidget *PhotometricPlugin::GetProcessPanel(QWidget *parent)
{
    photometric_ui = qobject_cast<Photometric_ui*>(mGenWidget);
    if (photometric_ui) {
        customView = photometric_ui->getCustomGraphicsView();
    }
    return mGenWidget;
}

