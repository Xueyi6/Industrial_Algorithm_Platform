#include "contour_detect.h"
#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // 读取图像
    std::string imagePath = "d:/rings_and_nuts.png";  // ← 请替换为你的测试图片路径
    cv::Mat src = cv::imread(imagePath);
    if (src.empty()) {
        std::cerr << "图像读取失败: " << imagePath << std::endl;
        return -1;
    }

    // 配置参数
    ContourDebug::ContourConfig config;

    // 轮廓显示模式
    config.displayMode = ContourDebug::ContourConfig::SHOW_ALL; // 显示所有轮廓

    // 轮廓筛选参数
    config.minAreaThreshold = 0.0;  // 最小面积

    // 显示选项
    config.drawContourLines = true;  // 绘制轮廓线
    config.drawBoundingRect = true;  // 绘制外接矩形
    config.drawRotatedRect = false;  // 不绘制旋转矩形
    config.drawIndex = false;         // 显示索引
    config.drawArea = false;          // 显示面积
    config.drawSize = false;          // 显示尺寸
    config.highlightMain = true;     // 突出显示主轮廓
    config.canny = false;

    // 预处理参数
    config.useBlur = false;           // 使用模糊
    config.blurKernelSize = 5;       // 模糊核大小
    config.useMorphology = false;     // 使用形态学操作
    config.morphKernelSize = 3;      // 形态学核大小

    // Canny边缘检测参数
    config.cannyLowThreshold = 50.0;  // Canny低阈值
    config.cannyHighThreshold = 150.0; // Canny高阈值

    // 轮廓检测模式
    config.contourMode = cv::RETR_TREE; // 只检测外部轮廓

    // 准备输出图像（避免在函数内创建）
    cv::Mat dst = src.clone();

    // 检测轮廓
    std::vector<ContourDebug::ContourResult> results;
    uint32_t status = ContourDebug::DetectContours(src, dst, config, results);

    if (status != 0) {
        std::cerr << "轮廓检测失败，状态码: " << status << std::endl;
        return -1;
    }

    // 输出结果
    std::cout << "检测到轮廓数量: " << results.size() << std::endl;
    for (const auto& result : results) {
        std::cout << "轮廓 " << result.index
            << " | 位置: (" << result.boundingRect.x << ", " << result.boundingRect.y << ")"
            << " | 尺寸: " << result.width << "x" << result.height
            << " | 面积area: " << result.area << std::endl;
    }

    // 显示图像
    cv::imshow("origin_fig", src);
    cv::imshow("contour_detect_result", dst);

    // 保存结果图像（可选）
    cv::imwrite("contour_result.jpg", dst);

    cv::waitKey(0);
    return 0;
}
