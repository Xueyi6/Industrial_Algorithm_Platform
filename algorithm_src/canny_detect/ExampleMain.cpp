#include "canny_detect.h"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace cv;

int main() {
    cv::Mat image = cv::imread("d:/1.jpg");
    cv::Mat edgeImage, canny_img, canny_dst;
    CannyDebug::EdgeInfo edgeInfo;

    CannyDebug::APCanny(image, edgeImage, edgeInfo, 30, 15, 3, true);
    // 1. 高斯滤波
    cv::GaussianBlur(image, canny_img, cv::Size(5, 5), 0, 0);
    Canny(canny_img, canny_dst, 30, 15, 3, true);
    // 访问边缘信息
    //for (size_t i = 0; i < edgeInfo.size(); ++i) {
    //    std::cout << "Edge point: " << edgeInfo.edge_points[i]
    //              << " magnitude: " << edgeInfo.edge_magnitude[i] << std::endl;
    //}
    imshow("cannyImage", edgeImage);
    imshow("canny:", canny_dst);
    waitKey();
    return 0;
}
