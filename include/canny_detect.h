#ifndef _AP_CANNY_H_
#define _AP_CANNY_H_

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C 
#endif

#ifdef ALGO_EXPORT
#define AP_API __declspec(dllexport)
#else
#define AP_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>

using namespace cv;

namespace CannyDebug {

	struct EdgeInfo {
		std::vector<cv::Point> edge_points;
		std::vector<double> edge_magnitude;
		std::vector<double> edge_derivative_x;
		std::vector<double> edge_derivative_y;

		void clear() {
			edge_points.clear();
			edge_magnitude.clear();
			edge_derivative_x.clear();
			edge_derivative_y.clear();
		}

		size_t size() const {
			return edge_points.size();
		}
	};

	/*
	 * @brief 手动实现 Canny 边缘检测算法（不依赖 OpenCV Canny）
	 * @param srcImage       输入单通道图像
	 * @param edgeImage      输出边缘图像
	 * @param edgeInfo       输出边缘信息（由内部填充，外部调用方需负责释放）
	 * @param maxContrast    高阈值
	 * @param minContrast    低阈值
	 * @param apertureSize   Sobel 核大小，必须为奇数（默认 3）
	 * @param L2gradient     是否使用 L2 范式计算梯度幅值
	 * @return uint32_t      状态码，0 表示成功
	 */
	EXTERN_C AP_API
	uint32_t APCanny(const Mat& srcImage, Mat& edgeImage,
					 EdgeInfo& edgeInfo, int maxContrast,
					 int minContrast, int apertureSize = 3,
					 bool L2gradient = false);
}

#endif // !_AP_CANNY_H_
