#include "PhotometricStereo.h"

#define CREATE_NUM(x) (const int NUM = x;)

cv::Mat globalHeights(cv::Mat Pgrads, cv::Mat Qgrads) 
{
    cv::Mat P(Pgrads.rows, Pgrads.cols, CV_32FC2, cv::Scalar::all(0));
    cv::Mat Q(Pgrads.rows, Pgrads.cols, CV_32FC2, cv::Scalar::all(0));
    cv::Mat Z(Pgrads.rows, Pgrads.cols, CV_32FC2, cv::Scalar::all(0));

    float lambda = 1.0f;
    float mu = 1.0f;

    cv::dft(Pgrads, P, cv::DFT_COMPLEX_OUTPUT);
    cv::dft(Qgrads, Q, cv::DFT_COMPLEX_OUTPUT);
    for (int i = 0; i < Pgrads.rows; i++) {
        for (int j = 0; j < Pgrads.cols; j++) {
            if (i != 0 || j != 0) {
                float u = sin((float)(i * 2 * CV_PI / Pgrads.rows));
                float v = sin((float)(j * 2 * CV_PI / Pgrads.cols));

                float uv = pow(u, 2) + pow(v, 2);
                float d = (1.0f + lambda) * uv + mu * pow(uv, 2);
                Z.at<cv::Vec2f>(i, j)[0] = (u * P.at<cv::Vec2f>(i, j)[1] + v * Q.at<cv::Vec2f>(i, j)[1]) / d;
                Z.at<cv::Vec2f>(i, j)[1] = (-u * P.at<cv::Vec2f>(i, j)[0] - v * Q.at<cv::Vec2f>(i, j)[0]) / d;
            }
        }
    }

    /* setting unknown average height to zero */
    Z.at<cv::Vec2f>(0, 0)[0] = 0.0f;
    Z.at<cv::Vec2f>(0, 0)[1] = 0.0f;

    cv::dft(Z, Z, cv::DFT_INVERSE | cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);

    return Z;
}

uint32_t PhotometricStereoDebug::PhotometricStereo(const vector<Mat>& srcImages,
	Mat& HeightField,
	Mat& Gradient,
	Mat& Albedo,
	int ImageCount,
	vector<float> Slants,
	vector<float> Tilts)
{
    if (srcImages.size() < ImageCount ||
        Slants.size() < ImageCount ||
        Tilts.size() < ImageCount)
    {
        return -1;
    }

    //【1】准备数据
    const int NUM_IMGS = 4;
    vector<Mat> modelImages;
    Mat Lights(NUM_IMGS, 3, CV_32F);//保存光源矩阵
    //【2】通过给定角度计算Lights
    for (int i = 0; i < NUM_IMGS; i++) 
    {
        cv::Mat Model = srcImages[i];
        if (Model.empty())
        {
            return -1;
        }
        //计算光源向量
        /* x,y are swapped here */
        float z = std::cos(CV_2PI * Slants[i] / 360.0);
        float xy = std::sin(CV_2PI * Slants[i] / 360.0);
        float x = std::sin(CV_2PI * Tilts[i] / 360.0) * xy;
        float y = std::cos(CV_2PI * Tilts[i] / 360.0) * xy;

        Vec3f L(x, y, z);
        Lights.at<float>(i, 0) = L[0];
        Lights.at<float>(i, 1) = L[1];
        Lights.at<float>(i, 2) = L[2];
        modelImages.push_back(Model);
    }

    const int height = modelImages[0].rows;
    const int width = modelImages[0].cols;
    //【3】计算反照率ρ和表面法向量N
    /* light directions, surface normals, p,q gradients */
    cv::Mat LightsInv;//L的逆矩阵，L-1
    cv::invert(Lights, LightsInv, cv::DECOMP_SVD);

    cv::Mat Normals(height, width, CV_32FC3, cv::Scalar::all(0));//法向量图
    cv::Mat AlbedoMap(height, width, CV_32F, cv::Scalar::all(0));//反照率图
    cv::Mat Pgrads(height, width, CV_32F, cv::Scalar::all(0));
    cv::Mat Qgrads(height, width, CV_32F, cv::Scalar::all(0));
    /* estimate surface normals and p,q gradients */
    for (int x = 0; x < width; x++) 
    {
        for (int y = 0; y < height; y++) 
        {
            Vec<float, NUM_IMGS> I;
            for (int i = 0; i < NUM_IMGS; i++) 
            {
                I[i] = modelImages[i].at<uchar>(Point(x, y));
            }

            cv::Mat n = LightsInv * cv::Mat(I);
            float kd = sqrt(n.dot(n));
            if (kd > 0) { n = n / kd; }
            if (n.at<float>(2, 0) == 0) { n.at<float>(2, 0) = 1.0; }
            int legit = 1;

            AlbedoMap.at<float>(cv::Point(x, y)) = kd / 255;
            /* avoid spikes ad edges */
            for (int i = 0; i < NUM_IMGS; i++) {
                legit *= modelImages[i].at<uchar>(Point(x, y)) >= 0;
            }
            if (legit) {
                Normals.at<cv::Vec3f>(cv::Point(x, y)) = n;
                Pgrads.at<float>(cv::Point(x, y)) = n.at<float>(0, 0) / n.at<float>(2, 0);
                Qgrads.at<float>(cv::Point(x, y)) = n.at<float>(1, 0) / n.at<float>(2, 0);
            }
            else {
                cv::Vec3f nullvec(0.0f, 0.0f, 1.0f);
                Normals.at<cv::Vec3f>(cv::Point(x, y)) = nullvec;
                Pgrads.at<float>(cv::Point(x, y)) = 0.0f;
                Qgrads.at<float>(cv::Point(x, y)) = 0.0f;
            }
        }
    }

    //【4】结果图像输出
    cv::normalize(AlbedoMap, AlbedoMap, 0, 255, cv::NORM_MINMAX, CV_8U);
    Albedo = AlbedoMap;
    cv::Mat HeightMap = globalHeights(Pgrads, Qgrads);
    cv::normalize(HeightMap, HeightMap, 0, 255, cv::NORM_MINMAX, CV_8U);
    HeightField = HeightMap;
    cv::normalize(Pgrads, Pgrads, 0, 255, cv::NORM_MINMAX, CV_8U);
    Gradient = Pgrads;

    return 0;
}