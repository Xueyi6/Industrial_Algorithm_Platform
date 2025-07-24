#include "contourplugin.h"
#include "opencv2/opencv.hpp"
#include "imageconvert.h"
#include "contour_detect.h"
#include <QMessageBox>
#include <QDebug>
#include<QFile>

ContourPlugin::ContourPlugin()
{
    mGenWidget = new Contour_ui();;
    contour_ui = Q_NULLPTR;
}

ContourPlugin::~ContourPlugin()
{

}

QString ContourPlugin::name()
{
    return "ContourPlugin";
}

QString ContourPlugin::information()
{
    return "ContourPlugin";
}

int ContourPlugin::Init()
{
    return 0;
}

QVector<QImage> ContourPlugin::getAllResult()
{
    return m_dstImages;
}

bool ContourPlugin::Export(QString dirPath)
{
    for (int i = 0; i < m_dstImages.size(); ++i) {
        const QString imgName = QString("contour_%1").arg(i, 3, 10, QChar('0'));
        const QString imagePath = QString("%1/%2.png").arg(dirPath).arg(imgName);
        const QString labelPath = QString("%1/%2.txt").arg(dirPath).arg(imgName);

        // 1. 保存图像
        bool success = m_dstImages[i].save(imagePath);

        // 2. 保存轮廓检测结果
        const std::vector<ContourDebug::ContourResult>& results = allContours[i];
        QFile file(labelPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);

            // 写表头
            out << "index"
                << "\t" << "x"
                << "\t" << "y"
                << "\t" << "width"
                << "\t" << "height"
                << "\t" << "rotated_center_x"
                << "\t" << "rotated_center_y"
                << "\t" << "rotated_width"
                << "\t" << "rotated_height"
                << "\t" << "angle"
                << "\t" << "area"
                << "\n";

            for (int j = 0; j < results.size(); ++j) {
                const auto& r = results[j];
                const auto& rect = r.boundingRect;
                const auto& rrect = r.rotatedRect;

                out << r.index << "\t"
                    << rect.x << "\t"
                    << rect.y << "\t"
                    << rect.width << "\t"
                    << rect.height << "\t"
                    << rrect.center.x << "\t"
                    << rrect.center.y << "\t"
                    << r.width << "\t"
                    << r.height << "\t"
                    << rrect.angle << "\t"
                    << r.area << "\n";
                qDebug() << "r.area:" << r.area;
            }
            file.close();
        }
    }
    return true;
}

void ContourPlugin::requestStop()
{
    m_stop = true;
}

void ContourPlugin::result_reset()
{
    m_dstImages.clear();
    allContours.clear();
    allContours.shrink_to_fit();
}

void ContourPlugin::run_algorithm(QImage qimg, ContourDebug::ContourConfig config){
    cv::Mat dstImage;
    std::vector<ContourDebug::ContourResult> results;
    cv::Mat srcImage = QImage2cvMat(qimg);
    if (srcImage.channels() == 4){
        cv::cvtColor(srcImage, srcImage, cv::COLOR_BGRA2BGR);
    }
    qDebug() << "contour mode:" << config.contourMode;
    uint32_t status = ContourDebug::DetectContours(srcImage, dstImage, config, results);
    m_dstImage = cvMat2QImage(dstImage);
    m_dstImages.push_back(m_dstImage);
    allContours.push_back(results);
    result_imgs.push_back(m_dstImage);

}

QVector<QImage> ContourPlugin::Execute(QVector<QImage> images, int currentrow)
{
    result_imgs.clear();
    // 1. 图像列表为空，说明没有图片
    if((images.size() == 0) | (currentrow == -999)){
        // QMessageBox::warning(mGenWidget, tr("运行错误"), tr("未选择图片!!!"));
        return result_imgs;
    }
    int max_threshold = contour_ui->get_max_threshold();
    int min_threshold = contour_ui->get_min_threshold();
    double min_area_threshold = contour_ui->get_min_area_threshold();
    int gauss_size = contour_ui->get_gauss_size();
    QString detect_model = contour_ui->get_contour_detect_model();
    QString display_model = contour_ui->get_contour_display_model();
    bool is_highlight = contour_ui->get_is_highlight();
    bool is_gauss = contour_ui->get_is_gauss();
    bool draw_area = contour_ui->drawArea();
    bool draw_rotate = contour_ui->drawRotatedRect();
    bool draw_bound = contour_ui->drawBoundingRect();
    bool is_canny = contour_ui->is_canny();
    int min_segment_threshold = contour_ui->get_segment_threshold();
    // 准备参数
    ContourDebug::ContourConfig config;
    // 设置值
    if (display_model=="只显示主轮廓"){
        qDebug() << "display:" << config.displayMode;
        config.displayMode = ContourDebug::ContourConfig::DisplayMode::SHOW_MAIN;
    }
    else{
        config.displayMode = ContourDebug::ContourConfig::DisplayMode::SHOW_ALL;
    }
    if (detect_model == "仅检索外轮廓"){
        config.contourMode = cv::RETR_EXTERNAL;
    }
    else if (detect_model.contains("重构"))
    {
        config.contourMode = cv::RETR_TREE;
    }
    else
    {
        config.contourMode = cv::RETR_LIST;
    }
    config.cannyLowThreshold = min_threshold;
    config.cannyHighThreshold = max_threshold;
    config.minAreaThreshold = min_area_threshold;
    config.highlightMain = is_highlight;
    config.useBlur = is_gauss;
    config.drawBoundingRect = draw_bound;
    config.drawArea = draw_area;
    config.drawRotatedRect = draw_rotate;
    config.blurKernelSize = gauss_size;
    config.canny = is_canny;
    config.segment_threshold = min_segment_threshold;

    if(currentrow == -1){
        for(QImage qimg:images){
            if(!m_stop){
                run_algorithm(qimg, config);
            }
            else {
                m_stop = !m_stop;
                break;
            }
        }
    }else {
        run_algorithm(images[currentrow], config);
    }
    return result_imgs;
}

CustomGraphicsView *ContourPlugin::getCustomGraphicsView()
{
    return customView;
}

QWidget *ContourPlugin::GetProcessPanel(QWidget *parent)
{
    contour_ui = qobject_cast<Contour_ui*>(mGenWidget);
    if (contour_ui) {
        customView = contour_ui->getCustomGraphicsView();
    }
    return mGenWidget;
}

