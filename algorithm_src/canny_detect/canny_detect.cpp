#include "canny_detect.h"

uint32_t CannyDebug::APCanny(const Mat& src_Image, Mat& edgeImage, EdgeInfo& edgeInfo, int maxContrast, int minContrast, int apertureSize, bool L2gradient)
{
    // 清空之前的结果
    edgeInfo.clear();

    cv::Mat srcImage;
    if (src_Image.channels() != 1) {
        if (src_Image.channels() == 4) {
            cv::cvtColor(src_Image, srcImage, cv::COLOR_RGBA2GRAY);
        } else {
            cv::cvtColor(src_Image, srcImage, cv::COLOR_BGR2GRAY);
        }
    } else {
        srcImage = src_Image.clone();
    }

    int width = srcImage.cols;
    int height = srcImage.rows;

    // 初始化边缘图像
    edgeImage = cv::Mat::zeros(srcImage.size(), CV_8UC1);

    // 缓冲区
    cv::Mat gradX(height, width, CV_16SC1, cv::Scalar(0));
    cv::Mat gradY(height, width, CV_16SC1, cv::Scalar(0));
    cv::Mat magnitude(height, width, CV_32FC1, cv::Scalar(0));
    cv::Mat direction(height, width, CV_32SC1, cv::Scalar(0));

    // 1. 高斯滤波
    cv::GaussianBlur(srcImage, srcImage, cv::Size(5, 5),0,0);

    // 2. Sobel 计算梯度
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            int dx = srcImage.at<uchar>(y, x + 1) - srcImage.at<uchar>(y, x - 1);
            int dy = srcImage.at<uchar>(y + 1, x) - srcImage.at<uchar>(y - 1, x);

            gradX.at<short>(y, x) = dx;
            gradY.at<short>(y, x) = dy;

            float mag = L2gradient ? std::sqrt(dx * dx + dy * dy) : std::abs(dx) + std::abs(dy);
            magnitude.at<float>(y, x) = mag;

            float angle = cv::fastAtan2((float)dy, (float)dx);
            int dir;
            if ((angle >= 0 && angle < 22.5) || (angle >= 157.5 && angle < 202.5) || (angle >= 337.5 && angle < 360))
                dir = 0;
            else if ((angle >= 22.5 && angle < 67.5) || (angle >= 202.5 && angle < 247.5))
                dir = 45;
            else if ((angle >= 67.5 && angle < 112.5) || (angle >= 247.5 && angle < 292.5))
                dir = 90;
            else
                dir = 135;
            direction.at<int>(y, x) = dir;
        }
    }

    // 3. 非极大值抑制
    float maxVal = 0;
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            float m = magnitude.at<float>(y, x);
            int dir = direction.at<int>(y, x);

            float neighbor1 = 0, neighbor2 = 0;
            switch (dir) {
                case 0:
                    neighbor1 = magnitude.at<float>(y, x - 1);
                    neighbor2 = magnitude.at<float>(y, x + 1);
                    break;
                case 45:
                    neighbor1 = magnitude.at<float>(y - 1, x + 1);
                    neighbor2 = magnitude.at<float>(y + 1, x - 1);
                    break;
                case 90:
                    neighbor1 = magnitude.at<float>(y - 1, x);
                    neighbor2 = magnitude.at<float>(y + 1, x);
                    break;
                case 135:
                    neighbor1 = magnitude.at<float>(y - 1, x - 1);
                    neighbor2 = magnitude.at<float>(y + 1, x + 1);
                    break;
            }

            if (m >= neighbor1 && m >= neighbor2) {
                edgeImage.at<uchar>(y, x) = static_cast<uchar>(std::min(m, 255.0f));
                if (m > maxVal) maxVal = m;
            } else {
                edgeImage.at<uchar>(y, x) = 0;
            }
        }
    }

    // 4. 滞后阈值处理 & 收集边缘点
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            uchar val = edgeImage.at<uchar>(y, x);
            if (val >= maxContrast) {
                // 强边缘，保留
            } else if (val < minContrast) {
                edgeImage.at<uchar>(y, x) = 0;
                continue;
            } else {
                // 判断邻域
                bool connected = false;
                for (int dy = -1; dy <= 1 && !connected; ++dy) {
                    for (int dx = -1; dx <= 1 && !connected; ++dx) {
                        if (edgeImage.at<uchar>(y + dy, x + dx) >= maxContrast)
                            connected = true;
                    }
                }
                if (!connected) {
                    edgeImage.at<uchar>(y, x) = 0;
                    continue;
                }
            }

            // 保存有效边缘信息
            int dx = gradX.at<short>(y, x);
            int dy = gradY.at<short>(y, x);
            float mag = magnitude.at<float>(y, x);
            edgeInfo.edge_points.push_back(cv::Point(x, y));
            edgeInfo.edge_derivative_x.push_back(dx);
            edgeInfo.edge_derivative_y.push_back(dy);
            edgeInfo.edge_magnitude.push_back(mag > 0 ? 1.0 / mag : 0.0);
        }
    }
    return 0;
}
