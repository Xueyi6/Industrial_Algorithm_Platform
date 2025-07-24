#ifndef _AP_PCB_DETECT_H_
#define _AP_PCB_DETECT_H_

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

namespace PCBDebug {

    // 缺陷信息结构体
    struct DefectBox {
        cv::Rect bbox;           // 位置框
        std::string label;       // 缺陷类别名
        float confidence;        // 置信度
    };

    /**
     * @brief 初始化检测器，只需调用一次
     *
     * @param modelPath ONNX 模型路径
     * @param inputSize 模型输入尺寸（默认 640×640）
     * @return true 初始化成功
     * @return false 初始化失败（如模型路径错误）
     */
    AP_EXTERN_C AP_API
    bool InitDetector(const std::string& modelPath,
                      const cv::Size& inputSize = cv::Size(640, 640));

    /**
     * @brief YOLO 检测 PCB 缺陷
     *
     * @param srcImage 输入图像
     * @param dstImage 输出图像（已绘制框和标签）
     * @param confidenceThreshold YOLO 置信度阈值
     * @param iouThreshold IoU 阈值（用于 NMS）
     * @param outBoxes 检测到的缺陷框信息列表（输出）
     * @return uint32_t 返回值状态码，0 表示成功，999 表示未初始化，1 表示输入图为空
     */
    AP_EXTERN_C AP_API
    uint32_t DetectPCBDefect(const cv::Mat& srcImage,
                             cv::Mat& dstImage,
                             float confidenceThreshold,
                             float iouThreshold,
                             std::vector<DefectBox>& outBoxes);

} // namespace PCBDebug

#endif // !_AP_PCB_DETECT_H_
