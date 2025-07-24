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
	* @srcImages:����ͼ����У����ٰ���3��ͼ����ÿ��ͼ����սǶȲ�ͬ
	* @HeightField:���ݶ��ͼ���ع��ĸ߶���Ϣͼ
	* @Gradient:���ݶ��ͼ���ع����ݶ���Ϣͼ
	* @Albedo:���ݶ��ͼ���ع��ķ�������Ϣͼ
	* @ImageCount:����ͼ��������������Ϊ4��6��12�ȵ�
	* @Slants:�ǶȲ���
	* @Tilts:�ǶȲ���
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
