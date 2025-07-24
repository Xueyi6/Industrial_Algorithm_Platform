#ifndef _AP_CONTOUR_DETECT_H_
#define _AP_CONTOUR_DETECT_H_

#ifdef __cplusplus
#define AP_EXTERN_C extern "C"
#else
#define AP_EXTERN_C
#endif

#ifdef AP_EXPORT
#define AP_API __declspec(dllexport)
#else
#define AP_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

namespace ContourDebug {

    // 轮廓配置结构体
    struct ContourConfig {
        // 轮廓显示模式
        enum DisplayMode {
            SHOW_NONE,     // 不显示任何轮廓
            SHOW_MAIN,     // 只显示主轮廓（面积最大的）
            SHOW_ALL       // 显示所有轮廓
        };
        DisplayMode displayMode = SHOW_ALL;  // 默认显示所有轮廓

        // 主轮廓突出显示选项
        bool highlightMain = false;    // 是否突出显示主轮廓

        // 轮廓筛选参数
        double minAreaThreshold = 100.0;  // 最小面积阈值

        // 显示选项
        bool drawContourLines = true; // 是否绘制轮廓线
        bool drawBoundingRect = true; // 是否绘制外接矩形
        bool drawRotatedRect = true; // 是否绘制旋转矩形
        bool drawIndex = false;       // 是否显示轮廓索引
        bool drawArea = true;        // 是否显示轮廓面积
        bool drawSize = true;         // 是否显示尺寸（宽高）
        bool canny = false;         // 是否使用canny算法
        bool segment = false;      // 是否使用阈值分割

        // 预处理参数
        bool useBlur = false;          // 是否使用模糊滤波
        int blurKernelSize = 5;        // 模糊核大小（奇数）
        bool useMorphology = false;    // 是否使用形态学操作
        int morphKernelSize = 3;       // 形态学核大小
        int segment_threshold = 100;   // 阈值分割大小

        // Canny边缘检测参数
        double cannyLowThreshold = 50.0;  // Canny低阈值
        double cannyHighThreshold = 150.0; // Canny高阈值

        // 轮廓检测模式
        int contourMode = cv::RETR_EXTERNAL; // 轮廓检索模式
    };


    /// 轮廓结果结构体
    struct ContourResult {
        cv::Rect boundingRect;       // 外接矩形
        cv::RotatedRect rotatedRect; // 最小外接旋转矩形
        double area;                 // 轮廓面积
        int index;                   // 轮廓索引
        float width;                 // 旋转矩形宽度
        float height;                // 旋转矩形高度
    };

    /**
     * @brief 轮廓检测主函数
     * 
     * @param srcImage 输入图像
     * @param dstImage 输出图像（绘制结果）
     * @param config 检测配置参数
     * @param results 输出的轮廓结构体列表
     * @return uint32_t 0成功，1图像为空
     */
    AP_EXTERN_C AP_API
    uint32_t DetectContours(const cv::Mat& srcImage,
                            cv::Mat& dstImage,
                            const ContourConfig& config,
                            std::vector<ContourResult>& results);

} // namespace ContourDebug

#endif // !_AP_CONTOUR_DETECT_H_
