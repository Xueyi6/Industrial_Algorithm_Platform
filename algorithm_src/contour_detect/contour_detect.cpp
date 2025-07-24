#include "contour_detect.h"

namespace ContourDebug {

    uint32_t DetectContours(const cv::Mat& srcImage,
                            cv::Mat& dstImage,
                            const ContourConfig& config,
                            std::vector<ContourResult>& results)
    {
        // 转换为灰度图
        std::cout << "std1:contour mode:" << config.contourMode << std::endl;  // 应该输出 0
        cv::Mat gray;
        if (srcImage.channels() == 3) {
            cv::cvtColor(srcImage, gray, cv::COLOR_BGR2GRAY);
        }
        else {
            gray = srcImage.clone();
        }

        // 图像预处理
        cv::Mat processed = gray.clone();
        if (config.useBlur && config.blurKernelSize > 0) {
            // 确保核大小为奇数
            int ksize = config.blurKernelSize | 1; // 确保为奇数
            cv::GaussianBlur(processed, processed, cv::Size(ksize, ksize), 0);
        }
        if (config.useMorphology && config.morphKernelSize > 0) {
            int ksize = std::max(1, config.morphKernelSize);
            cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
            cv::morphologyEx(processed, processed, cv::MORPH_CLOSE, kernel);
        }

        // Canny边缘检测
        cv::Mat edges;
        if (config.canny) {
            cv::Canny(processed, edges, config.cannyLowThreshold, config.cannyHighThreshold);
        }
        else {
            cv::threshold(processed, edges, config.segment_threshold, 255, cv::THRESH_BINARY);
        }
        // 检查类型
        std::cout << "std2:contour mode:" << config.contourMode << std::endl;  // 应该输出 0
        // 查找轮廓
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(edges, contours, hierarchy, config.contourMode, cv::CHAIN_APPROX_SIMPLE);
        if (srcImage.channels() == 1) {
            cv::cvtColor(srcImage, dstImage, cv::COLOR_GRAY2BGR);
        }
        else {
            dstImage = srcImage.clone();
        }

        // 如果没有找到轮廓，直接返回
        if (contours.empty()) {
            cv::putText(dstImage, "NO CONTOURS FOUND", cv::Point(20, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
            return 0;
        }

        // 找到主轮廓（面积最大的）
        int mainContourIdx = -1;
        double maxArea = 0;
        for (size_t i = 0; i < contours.size(); ++i) {
            double area = cv::contourArea(contours[i]);
            if (area > maxArea) {
                maxArea = area;
                mainContourIdx = static_cast<int>(i);
            }
        }

        // 遍历并绘制轮廓
        for (size_t i = 0; i < contours.size(); ++i) {
            double area = cv::contourArea(contours[i]);

            // 应用面积阈值筛选
            if (area < config.minAreaThreshold) {
                continue;
            }

            // 检查显示模式
            bool isMainContour = (static_cast<int>(i) == mainContourIdx);
            bool shouldDraw = false;

            switch (config.displayMode) {
            case ContourConfig::SHOW_NONE:
                shouldDraw = false;
                break;
            case ContourConfig::SHOW_MAIN:
                shouldDraw = isMainContour;
                break;
            case ContourConfig::SHOW_ALL:
                shouldDraw = true;
                break;
            }

            if (!shouldDraw) {
                continue;
            }

            cv::Rect rect = cv::boundingRect(contours[i]);
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);

            // 确定绘制样式
            bool isHighlighted = isMainContour && config.highlightMain;
            cv::Scalar contourColor = isHighlighted ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 255, 0);  // 黄色或绿色
            int contourThickness = isHighlighted ? 3 : 1;
            cv::Scalar rectColor = isHighlighted ? cv::Scalar(0, 255, 255) : cv::Scalar(0, 0, 255);  // 黄色或红色

            // 绘制轮廓线
            if (config.drawContourLines) {
                cv::drawContours(dstImage, contours, static_cast<int>(i), contourColor, contourThickness);
            }

            // 绘制外接矩形
            if (config.drawBoundingRect) {
                cv::rectangle(dstImage, rect, rectColor, 1);
            }

            // 绘制旋转矩形
            if (config.drawRotatedRect) {
                cv::Point2f vertices[4];
                rotatedRect.points(vertices);
                for (int j = 0; j < 4; j++) {
                    cv::line(dstImage, vertices[j], vertices[(j + 1) % 4], cv::Scalar(255, 0, 0), 1);
                }
            }

            // 显示索引
            if (config.drawIndex) {
                cv::putText(dstImage, std::to_string(i), rect.tl() + cv::Point(0, -5),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
            }

            // 显示面积
            if (config.drawArea) {
                cv::putText(dstImage, "A:" + std::to_string(int(area)),
                    rect.tl() + cv::Point(0, 15),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(20, 99, 255), 1);
            }

            // 显示尺寸
            if (config.drawSize) {
                std::string sizeText = "W:" + std::to_string(int(rotatedRect.size.width)) +
                    " H:" + std::to_string(int(rotatedRect.size.height));
                cv::Scalar sizeColor = isHighlighted ? cv::Scalar(41, 184, 255) : cv::Scalar(41, 184, 255);  // 黄色
                cv::putText(dstImage, sizeText, rotatedRect.center,
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, sizeColor, 1);
            }

            // 保存结果
            ContourResult result;
            result.boundingRect = rect;
            result.rotatedRect = rotatedRect;
            result.area = area;
            result.index = static_cast<int>(i);
            result.width = rotatedRect.size.width;
            result.height = rotatedRect.size.height;
            results.push_back(result);
        }

        // 显示主轮廓信息（如果需要）
        if (mainContourIdx >= 0 && config.displayMode != ContourConfig::SHOW_NONE) {
            cv::RotatedRect mainRect = cv::minAreaRect(contours[mainContourIdx]);
            cv::Size2f mainSize = mainRect.size;

            std::string mainText = "Main: W:" + std::to_string(int(mainSize.width)) +
                " H:" + std::to_string(int(mainSize.height));
            cv::putText(dstImage, mainText, cv::Point(20, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(41, 184, 255), 2);
        }
        return 0;
    }

} // namespace ContourDebug
