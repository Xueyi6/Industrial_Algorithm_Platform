#include "pcb_detect.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

using namespace PCBDebug;

int main() {
    // 模型路径
    std::string modelPath = "./model/best.onnx";  // <<< 替换为你实际模型路径
    cv::Size inputSize(640, 640);

    // 初始化模型
    if (!InitDetector(modelPath, inputSize)) {
        std::cerr << "初始化模型失败！" << std::endl;
        return -1;
    }

    // 加载测试图像
    std::string imagePath = "./images/12_short_04.jpg";  // <<< 替换为你的测试图像
    cv::Mat inputImage = cv::imread(imagePath);
    if (inputImage.empty()) {
        std::cerr << "图像读取失败: " << imagePath << std::endl;
        return -1;
    }

    // 调用检测接口
    cv::Mat resultImage, test_imgs;
    std::vector<DefectBox> boxes;
    float confThresh = 0.25f;
    float iouThresh = 0.45f;
    //uint32_t status = DetectPCBDefect(inputImage, test_imgs, confThresh, iouThresh, boxes);
    uint32_t status = DetectPCBDefect(inputImage, resultImage, confThresh, iouThresh, boxes);
    if (status != 0) {
        std::cerr << "检测失败，错误码: " << status << std::endl;
        return -1;
    }

    // 输出检测结果
    std::cout << "检测到缺陷数量: " << boxes.size() << std::endl;
    for (size_t i = 0; i < boxes.size(); ++i) {
        std::cout << "缺陷 " << i << ":" << boxes[i].label
                  << " 置信度=" << boxes[i].confidence
                  << " 坐标=" << boxes[i].bbox << std::endl;
    }

    // 显示检测结果图像
    cv::imshow("PCB Result", resultImage);
    cv::waitKey(0); // 等待按键
    return 0;
}
