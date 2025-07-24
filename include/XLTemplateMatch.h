#ifndef _XL_TEMPLATEMATCH_H_
#define _XL_TEMPLATEMATCH_H_

#ifdef __cplusplus
#define EXTERN_C extern "C"
#else
#define EXTERN_C 
#endif

#ifdef ALGO_EXPORT
#define XL_API __declspec(dllexport)
#else
#define XL_API __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>
#include "XLErrorDefine.h"
#include "XLRoiDefine.h"

using namespace cv;
using namespace std;

namespace XiangLong
{
//ģ��ѧϰ������
struct MatchLearnParam
{
	double max_contrast;
	double min_contrast;
	int num_pyramid;
	double angle_start;
	double angle_end;
	double angle_step;
	double scale_start;
	double scale_end;
	double scale_step;
};

//ƥ�����ṹ��
struct MatchResult
{
	double          scale;				//ģ������
	double 			angel;				//ģ��Ƕ�
	int 			centerLocX;			//ƥ��ο���X����
	int				centerLocY;			//ƥ��ο���Y����
	float 			resultScore;		//ƥ��÷�
	Rect            bBox;               //BoundingBox
};

//ģ�����в�����
struct MatchRunParam
{
	double minScore;   //�÷ֵ���ֵ
	double greediness; //̰��ϵ��
};

//�߽���Ϣ���ݽṹ
//���������ÿһ��ͼ��������ı߽���Ϣ
//����ÿ������ݶȷ�ֵ��X�����ݶȡ�Y�����ݶ�
struct PyramidEdgeInfo
{
	int     level;					//�������㼶
	int	    num_cordinates;			//��������
	Point* edge_points;			//�����
	double* edge_magnitude;		//�ݶȷ�ֵ����
	double* edge_derivative_x;		//X�����ݶ�
	double* edge_derivative_y;		//Y�����ݶ�
	Point   center_gravity;			//ģ����������
};

//�����洢Ŀ��ͼ���߶ȵ��ݶ���Ϣ
struct ImgEdgeInfo
{
	int16_t* grad_x;
	int16_t* grad_y;
	float* magnitude;
};

//��������
struct SearchRegion
{
	int 	start_x;	//x�������
	int 	start_y;	//y�������
	int 	end_x;		//x�����յ�
	int 	end_y;		//y�����յ�
};

//������ͼ�����ѧϰ�õ���ģ���Ե��Ϣ
struct TemplateEdgeInfo
{
	vector<PyramidEdgeInfo> pyramidEdgeInfos;
	pair<double, double> scaleAngleValue; //���ź���ת�Ƕ�
};

class XL_API XLTemplateMatching
{
public:
	XLTemplateMatching();
	~XLTemplateMatching();
	uint32_t TemplateLearn(const Mat& templateImg, const MatchLearnParam& learnParam);
	uint32_t TemplateMatch(const Mat& srcImage, const MatchRunParam& runParam, vector<MatchResult>& ResultLists);
	uint32_t TemplateMatch(const Mat& srcImage, const Rect& roi, const MatchRunParam& runParam, vector<MatchResult>& ResultLists);
	uint32_t DrawContours(Mat& srcImage, Scalar color, int lineWidth, const PyramidEdgeInfo& edgePoints);
	uint32_t DrawContours(Mat& srcImage, Point COG, Scalar color, int lineWidth);
	uint32_t DrawContours(Mat& srcImage, Scalar color, int lineWidth);
	uint32_t DrawBoundingBox(Mat& srcImage, vector<Point>& resultCenters, Scalar color, int lineWidth);
	uint32_t DrawBoundingBox(Mat& srcImage, Scalar color, int lineWidth);

private:
	uint32_t extract_shape_info(const Mat& srcImage, PyramidEdgeInfo& edgeInfo, int maxContrast, int minContrast);
	uint32_t extract_srcimage_edgeinfo(const Mat& srcImage, ImgEdgeInfo& imgEdgeInfo);
	uint32_t shape_match_accurate(const Mat& srcImage, const ImgEdgeInfo& imgEdgeInfo, const PyramidEdgeInfo& edgeInfo,
		int maxContrast, int MinContrast, float MinScore, float Greediness, const SearchRegion& SearchRegion, MatchResult& matchResult);
	uint32_t rotate_image(const Mat& srcImage, Mat& dstImage, float angle);
	float r_sqrtf(float f);
	float intersection_over_union(MatchResult rest1, MatchResult rest2); // iou
	vector<MatchResult> non_maximum_suppression(std::vector<MatchResult>& vec_boxs, float threshold); // nms

private:
	MatchLearnParam m_learnParam{};
	vector<TemplateEdgeInfo> m_templateEdgeInfos;	//�洢��ģ�徭�����������б�Ե��Ϣ
	vector<MatchResult> m_resultLists;				//ģ��ƥ����
	map<double, Size> m_modelSizes;//ģ��ͼ��Ŀ���
	int m_model_height{};
	int m_model_width{};

};

}// !XiangLong

#endif // !_XL_GRAYAREA_H_
