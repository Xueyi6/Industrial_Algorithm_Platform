#include <iostream>
#include <opencv2/opencv.hpp>
#include "PhotometricStereo.h"

using namespace std;
using namespace cv;

#define Tilts(x) Tilts
#define Slants(x) Slants

int main()
{
    const int NUM_IMGS = 4;
    //示例1
    //const string MODEL = "../images/blister_";
    //vector<float> Tilts(blister_tilts) = { 6.1,95.0,-176.1,-86.8 };
    //vector<float> Slants(blister_slants) = { 41.4,42.6,41.7,40.9 };
    
    //示例2
    //const string MODEL = "../images/braille_";
    //vector<float> Tilts(braille_tilts) = { 6.1,95.0,-176.1,-86.8 };
    //vector<float> Slants(braille_slants) = { 41.4,42.6,41.7,40.9 };

    //示例3
    //const string MODEL = "../images/tire_";
    //vector<float> Tilts(tire_tilts) = { 180,270,0,90 };
    //vector<float> Slants(tire_slants) = { 45,45,45,45 };

    //示例4
    //const string MODEL = "../images/toothpaste_";
    //vector<float> Tilts(toothpaste_tilts) = { 6.1,95.0,-176.1,-86.8 };
    //vector<float> Slants(toothpaste_slants) = { 41.4,42.6,41.7,40.9 };

    //示例5
    const string MODEL = "./images/test_";
    vector<float> Tilts(test_tilts) = { 0,90,180,270 };
    vector<float> Slants(test_slants) = { 45,45,45,45 };

    vector<Mat> modelImages;
    cv::Mat HeightMap;
    cv::Mat GradientMap;
    cv::Mat AlbedoMap;
    for (int i = 0; i < NUM_IMGS; i++) 
    {
        std::string path = MODEL + to_string(i) + ".png";
        cv::Mat Model = imread(path, IMREAD_GRAYSCALE);
        if (Model.empty() == true)
        {
            std::cout << "Read Image " << path << " failed!" << std::endl;
            return -1;
        }

        modelImages.push_back(Model);
    }
    auto ret = PhotometricStereoDebug::PhotometricStereo(modelImages, HeightMap, GradientMap, AlbedoMap, NUM_IMGS, Slants, Tilts);
    if (0 != ret)
    {
        std::cout << "Algo Run failed!" << std::endl;
        return -1;
    }

    //源图
    cv::Mat SrcImage = imread(MODEL + "1.png", IMREAD_GRAYSCALE);
    cv::imshow("SrcImage", SrcImage);
    //反照率图
    cv::imshow("Albedomap", AlbedoMap);
    //高度图
    cv::imshow("Heightmap", HeightMap);

    cv::imshow("GradientMap", GradientMap);
    cv::waitKey(0);
    return 0;
}