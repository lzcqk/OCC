#include<opencv2\opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
void on_mouse(int EVENT, int x, int y, int flags, void* userdata);

void main()
{
    namedWindow("��display��");
    Mat src;
    src = imread("bub.jpg");
    //cvtColor(src, src, COLOR_RGB2GRAY);
    setMouseCallback("��display��", on_mouse, &src);
    //��40msˢ����ʾ
    while (1)
    {
        imshow("��display��", src);
        waitKey(40);
    }
}

void on_mouse(int EVENT, int x, int y, int flags, void* userdata)
{
    Mat hh;
    hh = *(Mat*)userdata;
    Point p(x, y);
    switch (EVENT)
    {
    case EVENT_LBUTTONDOWN:
    {

        printf("b=%d\t", hh.at<Vec3b>(p)[0]);
        printf("g=%d\t", hh.at<Vec3b>(p)[1]);
        printf("r=%d\n", hh.at<Vec3b>(p)[2]);
        circle(hh, p, 2, Scalar(255), 3);
    }
    break;

    }

}
