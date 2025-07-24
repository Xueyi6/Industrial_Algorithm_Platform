#ifndef _AP_PHOTEMETRIC_STEREO_H_
#define _AP_PHOTEMETRIC_STEREO_H_

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
using namespace std;

namespace PhotometricStereoDebug
{
	/*
	* @srcImages:输入图像队列，至少包含3张图像，且每张图像光照角度不同
	* @HeightField:根据多个图像重构的高度信息图
	* @Gradient:根据多个图像重构的梯度信息图
	* @Albedo:根据多个图像重构的反射率信息图
	* @ImageCount:输入图像数量，常见的为4、6、12等等
	* @Slants:角度参数
	* @Tilts:角度参数
	*/
	uint32_t AP_API PhotometricStereo(const vector<Mat>& srcImages,
		Mat& HeightField,
		Mat& Gradient,
		Mat& Albedo,
		int ImageCount,
		vector<float> Slants,
		vector<float> Tilts);

}

#endif // !_AP_PHOTEMETRIC_STEREO_H_
