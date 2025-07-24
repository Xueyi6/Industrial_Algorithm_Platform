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
//模板学习参数组
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

//匹配结果结构体
struct MatchResult
{
	double          scale;				//模板缩放
	double 			angel;				//模板角度
	int 			centerLocX;			//匹配参考点X坐标
	int				centerLocY;			//匹配参考点Y坐标
	float 			resultScore;		//匹配得分
	Rect            bBox;               //BoundingBox
};

//模板运行参数组
struct MatchRunParam
{
	double minScore;   //得分低阈值
	double greediness; //贪婪系数
};

//边界信息数据结构
//用来保存对每一副图像所计算的边界信息
//包括每个点的梯度幅值、X方向梯度、Y方向梯度
struct PyramidEdgeInfo
{
	int     level;					//金字塔层级
	int	    num_cordinates;			//坐标点个数
	Point* edge_points;			//坐标点
	double* edge_magnitude;		//梯度幅值数列
	double* edge_derivative_x;		//X方向梯度
	double* edge_derivative_y;		//Y方向梯度
	Point   center_gravity;			//模板重心坐标
};

//用来存储目标图像多尺度的梯度信息
struct ImgEdgeInfo
{
	int16_t* grad_x;
	int16_t* grad_y;
	float* magnitude;
};

//搜索区域
struct SearchRegion
{
	int 	start_x;	//x方向起点
	int 	start_y;	//y方向起点
	int 	end_x;		//x方向终点
	int 	end_y;		//y方向终点
};

//对输入图像进行学习得到的模板边缘信息
struct TemplateEdgeInfo
{
	vector<PyramidEdgeInfo> pyramidEdgeInfos;
	pair<double, double> scaleAngleValue; //缩放和旋转角度
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
	vector<TemplateEdgeInfo> m_templateEdgeInfos;	//存储了模板经过计算后的所有边缘信息
	vector<MatchResult> m_resultLists;				//模板匹配结果
	map<double, Size> m_modelSizes;//模板图像的宽、高
	int m_model_height{};
	int m_model_width{};

};

}// !XiangLong

#endif // !_XL_GRAYAREA_H_
