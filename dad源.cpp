#include<opencv.hpp>
using namespace cv;

int main()
{
	VideoCapture cap;
	cap.open("stack.mp4");
	Mat src;
	cap >> src;
	Mat bgr_res[3];
	Mat bgr[3];
	for( int j = 0 ; j < 3; j++)
	{
		split(src, bgr);
		for (int i = 0; i < 3; ++i)
		{
			if (i != j)bgr[i] = Mat::zeros(src.size(), bgr[0].type());
		}
		merge(bgr, 3, bgr_res[j]);
	}
	imshow("picture", bgr_res[0]);
	imshow("picture1", bgr_res[1]);
	imshow("picture2", bgr_res[2]);
	waitKey(0);
}