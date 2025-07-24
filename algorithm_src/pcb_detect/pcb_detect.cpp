#include "pcb_detect.h"
#include <opencv2/dnn.hpp>
#include <shared_mutex>
#include <chrono>

namespace PCBDebug {

// ==== 内部全局状态 ====
static std::vector<std::string> g_classNames = {"missing_hole", "mouse_bite", "open_circuit", "short", "spur", "spurious_copper"}; // 请根据模型实际类别改
static std::string g_currentModelPath;
static std::shared_mutex g_net_mutex;
static cv::dnn::Net g_net;
static cv::Size g_inputSize;

bool InitDetector(const std::string& modelPath, const cv::Size& inputSize) {
    std::unique_lock<std::shared_mutex> lock(g_net_mutex);  // 写锁，独占
    if (!g_currentModelPath.empty() && modelPath == g_currentModelPath) {
        std::cout << "[InitDetector] 模型已经加载，无需切换: " << modelPath << std::endl;
        return true;  // 当前模型已加载，无需切换
    }

    try {
        std::cout << "[InitDetector] 正在加载新模型: " << modelPath << std::endl;

        g_net = cv::dnn::Net();  // 清空旧模型
        g_net = cv::dnn::readNetFromONNX(modelPath);

        // 启用 CUDA（可选）
        // g_net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        // g_net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

        g_inputSize = inputSize;
        g_currentModelPath = modelPath;

        std::cout << "[InitDetector] 模型加载完成: " << modelPath << std::endl;
        return true;

    }
    catch (const std::exception& e) {
        std::cerr << "[InitDetector] 模型加载失败: " << e.what() << std::endl;
        return false;
    }
}


uint32_t DetectPCBDefect(const cv::Mat& srcImage,
                         cv::Mat& dstImage,
                         float confidenceThreshold,
                         float iouThreshold,
                         std::vector<DefectBox>& outBoxes) {
    if (g_currentModelPath.empty()) return 999;
    if (srcImage.empty()) return 1;

    dstImage = srcImage.clone();
    int h0 = srcImage.rows;
    int w0 = srcImage.cols;

    // 假设你的模型输入为 640x640
    float gain_w = static_cast<float>(w0) / 640.f;
    float gain_h = static_cast<float>(h0) / 640.f;

    // === 预处理 ===
    cv::Mat blob = cv::dnn::blobFromImage(srcImage, 1.0 / 255.0, g_inputSize, cv::Scalar(), true, false);
    std::shared_lock<std::shared_mutex> lock(g_net_mutex);  // 并发读
    g_net.setInput(blob);

    std::vector<cv::Mat> outputs;
    //auto start = std::chrono::high_resolution_clock::now();  // 开始计时
    g_net.forward(outputs, g_net.getUnconnectedOutLayersNames());
     //auto end = std::chrono::high_resolution_clock::now();    // 结束计时
     //auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
     //std::cout << "detect_time: " << duration_ms << " ms" << std::endl;
    cv::Mat det = outputs[0]; // (batch_size, boxs, dims)
    if (det.dims == 3) {
        det = det.reshape(1, det.size[1]);  // [N, dims]
    }
    /*
    你的是 11 维，说明：

    [0:4]：位置（x, y, w, h）

    [4]：objectness 置信度

    [5:11]：你训练的 6 个类别（因为 11 - 5 = 6）
    */
    std::vector<cv::Rect> boxes;
    std::vector<float> confidences;
    std::vector<int> classIds;

    for (int i = 0; i < det.rows; ++i) {
        float* data = (float*)det.ptr(i);
        float objConf = data[4];

        cv::Mat scores(1, g_classNames.size(), CV_32FC1, data + 5);
        cv::Point classIdPoint;
        double maxClassScore;
        cv::minMaxLoc(scores, nullptr, &maxClassScore, nullptr, &classIdPoint);

        float finalConf = objConf * static_cast<float>(maxClassScore);
        if (finalConf < confidenceThreshold) continue;
        // 坐标变换
        float cx = data[0] * gain_w;
        float cy = data[1] * gain_h;
        float w = data[2] * gain_w;
        float h = data[3] * gain_h;

        int left = static_cast<int>(cx - w / 2);
        int top = static_cast<int>(cy - h / 2);
        int width = static_cast<int>(w);
        int height = static_cast<int>(h);

        boxes.emplace_back(left, top, width, height);
        confidences.push_back(finalConf);
        classIds.push_back(classIdPoint.x);
    }

    // === NMS 处理 ===
    std::vector<int> indices;
    if (!boxes.empty() && !confidences.empty() && boxes.size() == confidences.size()) {
        cv::dnn::NMSBoxes(boxes, confidences, confidenceThreshold, iouThreshold, indices);
    }
    for (int idx : indices) {
        const auto& box = boxes[idx];
        const auto& conf = confidences[idx];
        const auto& cls = classIds[idx];

        DefectBox result;
        result.bbox = box;
        result.confidence = conf;
        result.label = (cls >= 0 && cls < g_classNames.size()) ? g_classNames[cls] : "unknown";
        outBoxes.push_back(result);

        // 画框与文字
        cv::rectangle(dstImage, box, cv::Scalar(0, 0, 255), 2);
        std::string labelText = result.label + " " + cv::format("%.2f", conf);
        int baseline = 0;
        cv::Size textSize = cv::getTextSize(labelText, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);
        int top = std::max(box.y, textSize.height);
        cv::rectangle(dstImage, cv::Rect(box.x, top - textSize.height, textSize.width, textSize.height + baseline), cv::Scalar(0, 0, 255), cv::FILLED);
        cv::putText(dstImage, labelText, cv::Point(box.x, top), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
    return 0;
}

} // namespace PCBDebug
