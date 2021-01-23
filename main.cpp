#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <math.h>

using namespace cv;
using namespace std;

void SSR(Mat frame, Mat frame_ssr, Size size_ssr);
void MSR(Mat frame, Mat *frame_ssr,Mat frame_msr, int size_num);
void SSRCR(Mat frame, Mat frame_ssr, Size size_ssr);
void MSRCR(Mat frame, Mat frame_msrcr, Size size_min, Size size_max, int size_num);


