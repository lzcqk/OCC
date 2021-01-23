#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<fstream>
using namespace std;
using namespace cv;

float local_max, local_min;
int swit = 1;                          //用于判断找什么值
int temp;                    //用于存放阈值
int num[15] = {0,0,0,0,0,0};                  //找像素与bit的关系
int length = 1;                 //将像素与bit的关系长度存放在这里
//const int noise = 30;              //正常噪声波动值  将来进行判断
int exist = 0;                //表明是否有极大极小值
int tempp1 = 0;                   //position 位置locla_max and local_min
int tempp2 = 0;                

int main()
{
	ofstream opt;
	opt.open("data.csv");
	VideoCapture video;
	video.open("stack.mp4");
	Mat src = imread("fix.jpg");
	Mat gray_src;
	cvtColor(src, gray_src, COLOR_RGB2GRAY);
	resize(gray_src, gray_src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);                              //用linear 和 nearest 其实差别不大
	Mat oneline = gray_src(Rect(0, gray_src.rows / 2, gray_src.cols, 1));
	uchar* data = oneline.ptr<uchar>(0);
	local_max = data[0];
	for (int j = 1; j < oneline.cols; j++)  //列循环
	{
		int intensityF = data[j - 1];
		int intensityS = data[j];
		//cout << intensityS << " ";
		//opt << intensity<< "," << temp << endl;
		if (abs(intensityS - intensityF) < noise)
		{
			length++;
		}
		else
		{
			cout << length << endl;
			if (length >= 0 && length < 15)
			{
				num[length] ++;
			}
			length = 1;
		}
	}
	int max = num[0];
	for (int i = 0; i < 15; i++)
	{
		cout << num[i] << endl;
		if (max < num[i])
		{
			max = num[i];
			length = i ;
		}
	}
	cout << endl;
	cout << "length = " << length << endl;

	for (int i = 1; i < oneline.cols-1;i++)
	{
		if(swit == 1)                           //寻找local_max
		{
			if ((data[i] - data[i + 1]) * (data[i] - data[i - 1]) > 0 && data[i]>data[i-1])                   //极值点
			{
				if (exist == 0)
				{
					local_max = data[i];
					tempp1 = i;
					exist++;
				}
				else 
				{
					if (data[i] > local_max)
					{
						local_max = data[i];
						tempp1 = i;
						exist++;
					}
				}
				if ((data[i]-data[i + 1])>noise)                             //出现阶跃，表明是真的极值点local_max
				{
					swit = 2; 
					exist = 0;
					int thre = (local_max + local_min) / 2;
					for (int j = tempp2 + 1; j <=tempp1  ; j++)
					{
						opt << int(data[j]) << "," << thre << endl;
					}
				}
			}
		}
		if (swit == 2)                             //寻找local_min
		{
			if ((data[i] - data[i + 1]) * (data[i] - data[i - 1]) > 0&&(data[i] - data[i+1]) < 0)                         //极值点
			{
				if (exist == 0)
				{
					local_min = data[i];
					tempp2 = i;
					exist++;
				}
				else
				{
					if (data[i] < local_min)
					{
						local_min = data[i];
						tempp2 = i;
						exist++;
					}
				}
				if ((data[i + 1] - data[i]) > noise)                             //出现阶跃，表明是真的极值点local_min
				{
					swit = 1;
					exist = 0;
					int thre = (local_max + local_min) / 2;
					for (int j = tempp1 + 1; j <= tempp2; j++)
					{
						
						opt << int(data[j]) << ","  <<thre<< endl;
					}
				}
			}
		}
	}
	opt.close();
	return 0;
}