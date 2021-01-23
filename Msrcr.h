#ifndef _MSRCR_H_
#define _MSRCR_H_

#include <opencv2\opencv.hpp>
#include <math.h>
#include<iostream>
using namespace std;
using namespace cv;

class Msrcr
{
public:
	void SingleScaleRetinex(const cv::Mat& src, cv::Mat& dst, int sigma);
	Mat MultiScaleRetinex(const cv::Mat& srct, double w[], double sigmas[]);
	Mat MultiScaleRetinexCR(const cv::Mat& src, double w[], double sigmas[], int alpha, int beta);
};

#endif // !_MSRCR_H_