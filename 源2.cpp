#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<fstream>
using namespace std;
using namespace cv;

float local_max, local_min;
int swit = 1;                          //�����ж���ʲôֵ
int temp;                    //���ڴ����ֵ
int num[15] = {0,0,0,0,0,0};                  //��������bit�Ĺ�ϵ
int length = 1;                 //��������bit�Ĺ�ϵ���ȴ��������
//const int noise = 30;              //������������ֵ  ���������ж�
int exist = 0;                //�����Ƿ��м���Сֵ
int tempp1 = 0;                   //position λ��locla_max and local_min
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
	resize(gray_src, gray_src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);                              //��linear �� nearest ��ʵ��𲻴�
	Mat oneline = gray_src(Rect(0, gray_src.rows / 2, gray_src.cols, 1));
	uchar* data = oneline.ptr<uchar>(0);
	local_max = data[0];
	for (int j = 1; j < oneline.cols; j++)  //��ѭ��
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
		if(swit == 1)                           //Ѱ��local_max
		{
			if ((data[i] - data[i + 1]) * (data[i] - data[i - 1]) > 0 && data[i]>data[i-1])                   //��ֵ��
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
				if ((data[i]-data[i + 1])>noise)                             //���ֽ�Ծ����������ļ�ֵ��local_max
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
		if (swit == 2)                             //Ѱ��local_min
		{
			if ((data[i] - data[i + 1]) * (data[i] - data[i - 1]) > 0&&(data[i] - data[i+1]) < 0)                         //��ֵ��
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
				if ((data[i + 1] - data[i]) > noise)                             //���ֽ�Ծ����������ļ�ֵ��local_min
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