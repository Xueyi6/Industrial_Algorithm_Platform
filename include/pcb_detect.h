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

    // ȱ����Ϣ�ṹ��
    struct DefectBox {
        cv::Rect bbox;           // λ�ÿ�
        std::string label;       // ȱ�������
        float confidence;        // ���Ŷ�
    };

    /**
     * @brief ��ʼ���������ֻ�����һ��
     *
     * @param modelPath ONNX ģ��·��
     * @param inputSize ģ������ߴ磨Ĭ�� 640��640��
     * @return true ��ʼ���ɹ�
     * @return false ��ʼ��ʧ�ܣ���ģ��·������
     */
    AP_EXTERN_C AP_API
    bool InitDetector(const std::string& modelPath,
                      const cv::Size& inputSize = cv::Size(640, 640));

    /**
     * @brief YOLO ��� PCB ȱ��
     *
     * @param srcImage ����ͼ��
     * @param dstImage ���ͼ���ѻ��ƿ�ͱ�ǩ��
     * @param confidenceThreshold YOLO ���Ŷ���ֵ
     * @param iouThreshold IoU ��ֵ������ NMS��
     * @param outBoxes ��⵽��ȱ�ݿ���Ϣ�б������
     * @return uint32_t ����ֵ״̬�룬0 ��ʾ�ɹ���999 ��ʾδ��ʼ����1 ��ʾ����ͼΪ��
     */
    AP_EXTERN_C AP_API
    uint32_t DetectPCBDefect(const cv::Mat& srcImage,
                             cv::Mat& dstImage,
                             float confidenceThreshold,
                             float iouThreshold,
                             std::vector<DefectBox>& outBoxes);

} // namespace PCBDebug

#endif // !_AP_PCB_DETECT_H_
