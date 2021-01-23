#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<fstream>
using namespace std;
using namespace cv;                  //flag形参用于将来跟踪下一个帧

int local_max = 255;                   //temp_value
int local_min = 0;
int tempp1, tempp2;			//temp_place
vector<Point2i> maxpv;         //  place and value 
vector<Point2i> minpv;;
vector<int> decode;              //用于观察解码结果
vector<int> my_decode;         //用于存放我的解码结果
vector<int> decode_without_header[10];
vector<int> temp_decode;         //用于存放后续图片跟踪解码
vector<int> final_decode_data;     //最后的解码结果
vector<int> num_of_lianxv;          //记录连续的0，1有多少个，便于选择多少个0代表一个bit
int temp;                                 //用于存放阈值
int length[50] = {};                //找像素与bit的关系
int pxlength = 0;                    //找像素与bit的关系最有概率的值
float noise = 0;                      //正常噪声波动值  将来进行判断  
int frequt[256] = {};             //找最大频率的数代表0，1的grayscalar
float thre = 0;                       //  0,1代表位的中间阈值
int sca;                                 //图片变化比率
const int iteration_time = 5;            //迭代次数
//int header[] = { 1,0,1,0,1,0,1,0,1,0 };            //保存头部信息       如果为规律的结构，比如 0，1，0，1就不该使用此种结构
int header_length = 10;                                     //保存头部信息        规律结构使用这种
int channel_data[] = { 1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,1,0 };      //保存数据包     
//Bchannel_data = {1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,1,0}
//Gchannel_data = {1,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,1,1,0,1,0,0,1,1,0,1,0,0,1,1,1}
//Rchannel_data = {1,1,0,1,1,0,1,0,0,0,0,1,1,0,1,1,1,0,1,1,0,0,1,0,0,0,1,1,1,0,0,1,0,0,1,0,1,1,1,0,1,1,0,1,1,0,0,1,1,1}

void initial_vector()
{
	maxpv.clear();
	minpv.clear();
	decode.clear();
	my_decode.clear();
}

void Find_length()
{
	int num1 = 0;
	int num0 = 0;
	for (int i = 0; i < decode.size(); i++)
	{
		if (decode[i] == 1 && num0 == 0)
		{
			num1++;
		}
		else if (decode[i] == 1 && num0 != 0)
		{
			length[num0]++;
			num_of_lianxv.push_back(num0);
			num1++;
			num0 = 0;
		}
		else if (decode[i] == 0 && num1 == 0)
		{
			num0++;
		}
		else if (decode[i] == 0 && num1 != 0)
		{
			length[num1]++;
			num_of_lianxv.push_back(num1);
			num0++;
			num1 = 0;
		}
	}
	int poss = length[0];
	pxlength = 0;
	for (int i = 0; i < sizeof(length) / sizeof(int); i++)
	{
		if (length[i] > poss)
		{
			poss = length[i];
			pxlength = i;
		}
		cout << i << "  pixel/bit   " << length[i] << endl;
	}
	cout << pxlength << "  pixel/bit(mostly possible)   " << endl;
}

void Find_local(Mat BGRchannels)       //找出所有的极值点
{
	maxpv.clear();
	minpv.clear();
	int swit = 1;              //用于判断找什么值
	uchar* data = BGRchannels.ptr<uchar>(0);
	int flag = 0;
	for (int i = 1; i < BGRchannels.cols; i++)
	{
		if (swit == 1)                           //寻找local_max
		{
			if (((data[i] - data[i + 1]) * (data[i] - data[i - 1])) >= 0 && (data[i] > data[i - 1] || data[i] > data[i + 1]))                   //极值点max
			{
				if (flag >= 2)
				{
					maxpv.push_back(Point2i(i, data[i]));
					swit = 2;
				}
				else
				{
					flag++;
					swit = 2;
				}
			}
		}
		else if (swit == 2)                             //寻找local_min
		{
			if (((data[i] - data[i + 1]) * (data[i] - data[i - 1])) >= 0 && (data[i] < data[i + 1] || data[i - 1] > data[i]))                        //极值点min
			{
				if (flag >= 2)
				{
					minpv.push_back(Point2i(i, data[i]));
					swit = 1;
				}
				else
				{
					flag++;
					swit = 1;
				}
			}

		}
	}
}

void Find_frequt()
{
	int sum = 0;
	int times = 0;
	float grap = 0;
	for (int i = 0; i < (maxpv.size() + minpv.size()); i++)
	{
		if (i % 2 == 0)           //遍历maxpv
		{
			frequt[maxpv[i / 2].y] ++;
		}
		else                          //遍历minpv
		{
			frequt[minpv[i / 2].y] ++;
		}
	}
	for (int i = 0; i < 256; i++)
	{
		if (frequt[i] != 0)
		{
			//cout << i << "  的个数为" << frequt[i] << endl;
			sum += frequt[i] * i;
			times += frequt[i];
		}
	}
	cout << "   所有极值灰度值和   " << sum << endl;
	cout << "  累加次数   " << times << endl;
	grap = float(sum) / times;
	cout << "  极值分界线    " << grap << endl;  //有bug  因为0，1频率会干扰这个值
	int most_frequt = 0;
	for (int i = int(grap); i >= 0; i--)   //i代表灰度值   frequt[i]代表灰度值的频率
	{
		if (frequt[i] >= most_frequt)
		{
			most_frequt = frequt[i];
			local_min = i;
		}
	}
	most_frequt = 0;
	for (int i = int(grap); i < 256; i++)
	{
		if (frequt[i] >= most_frequt)
		{
			most_frequt = frequt[i];
			local_max = i;
		}
	}
	thre = (float(local_max) + local_min) / 2;
	cout << "  频率最大local_max  = " << local_max << endl;
	cout << "  频率最大local_min  = " << local_min << endl;
	cout << "  排除local的阈值 =      " << thre << endl;
}

void Find_usefullocal(Mat BGRchannels)
{
	maxpv.clear();              //清空minpv和maxpv 继续利用
	minpv.clear();
	int tempthre = 0;               //local_max+local_min/2
	int exist = 0;                       //找出最大的极大值和最小的极小值
	int max_min_sit = 1;           //1表示正在找local_max         ----      2表示正在找local_min
	int begin_i = 0;                     //begin_i
	uchar* data = BGRchannels.ptr<uchar>(0);
	int flag = 0;
	for (int i = 1; i < BGRchannels.cols - 1; i++)
	{
		if (((data[i] - data[i + 1]) * (data[i] - data[i - 1])) >= 0 && (data[i] > data[i - 1] || data[i] > data[i + 1]) && data[i] > thre)                   //极值点max
		{
			if (flag >= 2)
			{
				if (max_min_sit == 2)                      // 证明结束最后一个local_min，结算开始
				{
					tempthre = (local_max + local_min) / 2;
					if (tempp2 - tempp1 <= sca && exist == 1)
					{
						cout << "出现了local_min" << endl;
						exist = 0;
						if (data[i] >= local_max)
						{
							local_max = data[i];
							tempp1 = i;
							maxpv.pop_back();
						}
						max_min_sit = 1;
						exist++;
					}
					else
					{
						exist = 0;
						exist++;
						minpv.push_back(Point2i(tempp2, local_min));
						local_max = data[i];
						tempp1 = i;
						max_min_sit = 1;
					}
				}
				else if (max_min_sit == 1)   //证明还在local_max中，进行寻求区域lcoal_max
				{
					exist++;
					if (data[i] >= local_max)
					{
						local_max = data[i];
						tempp1 = i;
						max_min_sit = 1;
					}
				}
			}
			else
			{
				local_max = data[i];
				tempp1 = i;
				flag++;
				max_min_sit = 1;
				exist++;
			}
		}
		else if (((data[i] - data[i + 1]) * (data[i] - data[i - 1])) >= 0 && (data[i] < data[i + 1] || data[i - 1] > data[i]) && data[i] < thre)                        //极值点min
		{
			if (flag >= 2)
			{
				if (max_min_sit == 1)                 //    证明结束最后一个local_max，结算开始
				{
					tempthre = (local_max + local_min) / 2;
					if (tempp1 - tempp2 <= sca && exist == 1)  //有些bug 比如说可能是快速跳变 0，1
					{
						cout << "出现了local_max" << endl;
						exist = 0;
						if (data[i] <= local_min)
						{
							local_min = data[i];
							tempp2 = i;
							minpv.pop_back();
						}
						max_min_sit = 2;
						exist++;
					}
					else
					{
						exist = 0;
						exist++;
						maxpv.push_back(Point2i(tempp1, local_max));
						local_min = data[i];
						tempp2 = i;
						max_min_sit = 2;
					}
				}
				else if (max_min_sit == 2)   //证明还在local_max中，进行寻求区域lcoal_max
				{
					exist++;
					if (data[i] <= local_min)
					{
						local_min = data[i];
						tempp2 = i;
						max_min_sit = 2;
					}
				}
			}
			else
			{
				local_min = data[i];
				tempp2 = i;
				flag++;
				max_min_sit = 2;
				exist++;
			}
		}
	}
}

int Find_header(int position)
{
	int numhead = 0;
	int i = 0;
	int First_header = 0;
	int Second_header = 0;
	for (; i < my_decode.size() - 2; i++)
	{
		int temp = i + 1;
		if (my_decode[i] == 1 && my_decode[temp] == 0)
		{
			i++;
			numhead += 2;
		}
		else if (numhead >= header_length)
		{
			numhead = 0;
			break;
		}
		else
		{
			numhead = 0;
		}
	}
	cout << numhead << endl;
	First_header = i;
	cout << "First_header" << First_header << endl;
	for (; i < my_decode.size() - 2; i++)
	{
		int temp = i + 1;
		if (my_decode[i] == 1 && my_decode[temp] == 0)
		{
			i++;
			numhead += 2;
		}
		else if (numhead >= header_length)
		{
			break;
		}
		else
		{
			numhead = 0;
		}
	}
	Second_header = i;
	cout << "Second_header" << (Second_header - numhead) << endl;
	cout << endl;
	cout << "解码出来如下(不包括header)" << endl;
	if (First_header == Second_header)return 0;
	for (i = First_header; i < (Second_header - numhead); i++)
	{
		decode_without_header[position].push_back(my_decode[i]);
		cout << my_decode[i];
	}
	cout << endl;
	return 1;
}

void Input_csv(Mat BGRchannels, int flag)   //EVA函数实现       //flag == 1则输入csv
{
	decode.clear();
	uchar* data = BGRchannels.ptr<uchar>(0);
	int t = 0;            //观察极值使用
	ofstream opt;
	if (flag)
	{
		opt.open("data.csv");
	}
	int lominp = 1;
	int lomaxp = 1;
	tempp1 = 0;
	tempp2 = 0;
	if (maxpv[0].x < minpv[0].x) { lominp = maxpv[0].x; }              //找出出发点
	else { lominp = minpv[0].x; }
	if (maxpv.back().x > minpv.back().x) { lomaxp = maxpv.back().x; }          //找终点
	else { lomaxp = minpv.back().x; }
	local_min = minpv[0].y;
	local_max = maxpv[0].y;
	int temp_thre;
	int Bill;
	int pass_way;
	for (int i = lominp; i < lomaxp + 1; i++)                    //csv
	{
		if (i == maxpv[tempp1].x)
		{
			local_max = maxpv[tempp1].y;
			if (tempp1 < maxpv.size() - 1)tempp1++;
			t = 1;
			opt << int(data[i]) << "," << local_max << endl;
		}
		if (i == minpv[tempp2].x)
		{
			local_min = minpv[tempp2].y;
			if (tempp2 < minpv.size() - 1)tempp2++;
			t = 1;
			opt << int(data[i]) << "," << local_min << endl;
		}
		temp_thre = (local_max + local_min) / 2;
		Bill = (int(data[i]) > temp_thre);
		//pass_way = 
		if (flag)opt << int(data[i]) << "," << temp_thre << "," << Bill << endl;
		if (t == 0)opt << int(data[i]) << "," << "0" << "," << Bill << endl;
		decode.push_back(Bill);
	}
	if (flag) opt.close();
}

void judge(int flag, int num, int power)
{
	if (flag == 1)   //考虑到因为两边的像素点会因为卷帘快门的原因，可能属于上一个东西，所以会存在偏移量，卷帘快门行与行间曝光时间存在重叠
	{
		if (sca <= num && num <= pxlength + 1)       //Plan 1 对各个num都进行偏移处理
		{
			my_decode.push_back(power);
		}
		else
		{
			int i = 0;
			while (((num - i * pxlength) * (num - (i + 1) * pxlength)) > 0) { i++; }
			if (abs(num - i * pxlength) < abs(num - (i + 1) * pxlength)) { i = i; }
			else { i = i + 1; }
			num = num - i;
			i = 0;
			while (((num - i * pxlength) * (num - (i + 1) * pxlength)) > 0) { i++; }
			if (abs(num - i * pxlength) < abs(num - (i + 1) * pxlength))
			{
				for (int j = 0; j < i; j++)
				{
					my_decode.push_back(power);
				}
			}
			else
			{
				for (int j = 0; j < i + 1; j++)
				{
					my_decode.push_back(power);
				}
			}
		}
		/*
		if (sca <= num && num <= pxlength)       //Plan 2   只在sca的倍数处对num进行处理
		{
			my_decode.push_back(power);
		}
		else
		{
			int i = 0;
			while (((num - i * pxlength) * (num - (i + 1) * pxlength)) > 0) { i++; }
			if (abs(num - i * pxlength) < abs(num - (i + 1) * pxlength)) { i = i; }
			else { i = i + 1; }
			int flag = i / sca;
			num = num - flag*sca;
			i = 0;
			while (((num - i * pxlength) * (num - (i + 1) * pxlength)) > 0) { i++; }
			if (abs(num - i * pxlength) < abs(num - (i + 1) * pxlength))
			{
				for (int j = 0; j < i; j++)
				{
					my_decode.push_back(power);
				}
			}
			else
			{
				for (int j = 0; j < i + 1; j++)
				{
					my_decode.push_back(power);
				}
			}
		}
	//	*/
	}
	else
	{
		/*if (sca <= num && num < pxlength)
		{
			temp_decode.push_back(power);
		}
		else
		{
			for (int i = 0; i < iter3; i++)
			{
				temp_decode.push_back(power);
			}
		}*/
	}
}

void decode_pic(int flag)      //1为my_decode       2为temp_decode 
{
	temp_decode.clear();
	int num1 = 0;
	int num0 = 0;
	for (int i = 0; i < decode.size(); i++)
	{
		if (decode[i] == 1 && num0 == 0)
		{
			num1++;
		}
		else if (decode[i] == 1 && num0 != 0)
		{
			num1++;
			if (num0 < sca)
			{
				num1++;
				//num1 += num0;
			}
			else
			{
				judge(flag, num0, 0);
			}
			num0 = 0;
		}
		else if (decode[i] == 0 && num1 == 0)
		{
			num0++;
		}
		else if (decode[i] == 0 && num1 != 0)
		{
			num0++;
			if (num1 < sca)
			{
				num0++;
				//num0 += num1;
			}
			else
			{
				judge(flag, num1, 1);
			}
			num1 = 0;
		}
	}
	if (flag == 1)
	{
		int last_one = my_decode.back();
		my_decode.pop_back();
		if (my_decode.back() == last_one && my_decode.size() >= 1)
		{
			my_decode.pop_back();
		}
		cout << "   my_decode   ";
		for (int i = 0; i < my_decode.size(); i++)cout << my_decode[i];
		cout << endl;
	}
	else
	{
		int last_one = temp_decode.back();
		temp_decode.pop_back();
		if (temp_decode.back() == last_one && temp_decode.size() >= 1)
		{
			temp_decode.pop_back();
		}
		cout << " temp_decode  ";
		for (int i = 0; i < temp_decode.size(); i++)cout << temp_decode[i];
		cout << endl;
	}
}

void debubble(Mat transp[], int channel)               //个人认为时间复杂度过高  建议改进
{
	int* vec;
	vec = new int[1080];
	//Mat transp(Size(1080, 960), CV_16UC1);
	for (int coun = 0; coun < transp[channel].rows; coun++)
	{
		uchar* data = transp[channel].ptr<uchar>(coun);
		for (int i = transp[channel].cols - 1; i > 0; i--)
		{
			vec[i] = int(data[i]);
		}
		for (int ai = 0; ai < transp[channel].cols - 1; ai++)
		{
			for (int aj = 0; aj < transp[channel].cols - 1 - ai; aj++)
			{
				if (vec[aj] > vec[aj + 1])
				{
					int tmp;
					tmp = vec[aj];
					vec[aj] = vec[aj + 1];
					vec[aj + 1] = tmp;
				}
			}
		}
		for (int i = transp[channel].cols - 1; i > 0; i--)
		{
			data[i] = vec[i];
		}
	}
	cout << transp[channel].rows << "      " << transp[channel].cols << "    " << endl;
	delete vec;
}

/*
void eyes_compare(int position)
{
	cout << "我的数据：" << endl;
	for (int i = 0; i < decode_without_header[position].size(); i++)
	{
		cout << decode_without_header[position][i];
	}
	cout << endl;
	cout << "真的数据：" << endl;
	for (int i = 0; i < sizeof(channel_data) / sizeof(int); i++)
	{
		cout << channel_data[i];
	}
	cout << endl;
}
*/

void eyes_compare()
{
	cout << "我的数据：" << endl;
	for (int i = 0; i < final_decode_data.size(); i++)
	{
		cout << final_decode_data[i];
	}
	cout << endl;
	cout << "真的数据：" << endl;
	for (int i = 0; i < sizeof(channel_data) / sizeof(int); i++)
	{
		cout << channel_data[i];
	}
	cout << endl;
}

int EVA(Mat BGRchannels, int flag, int position)
{
	minpv.clear();
	maxpv.clear();
	if (flag == 1)
	{
		Find_local(BGRchannels);            //找 0 ，1代表
		Find_frequt();                                                //设置极大值和极小值阈值
		Find_usefullocal(BGRchannels);                   //开始找真的local
		Input_csv(BGRchannels, 1);                        //输出像素0，1
		//for (int i = 0; i < decode.size(); i++)cout << decode[i];
		//cout << endl;
		Find_length();               //找0，1对应length
		decode_pic(1);        //输出真的decode
		return Find_header(position);        //输出header 2 header内容
	}
	/*else
	{
		Find_local(BGRchannels, channels);
		Find_frequt();
		Find_usefullocal(BGRchannels, channels);
		Input_csv(BGRchannels, channels, 0);
		Find_length();               //找0，1对应length
		decode_pic(2);        //输出真的decode
		Find_following();
		cout << " my_decode  ";
		for (int i = 0; i < my_decode.size(); i++)cout << my_decode[i];
		cout << endl;
	}
	*/
}

//---------------------------------------------------------------------------未完成
void Find_following()        //用于跟踪视频后续bit
{
	int j = 0;
	int i = 0;
	while (j != (my_decode.size() - 1))
	{
		if (my_decode[j] == temp_decode[i])
		{
			if (j < (my_decode.size() - 1) && i < (temp_decode.size() - 1))
			{
				j++;
				i++;
			}
		}
		else if (i == 0 && j < (my_decode.size() - 1))
		{
			j++;
		}
		else if (i != 0 && j < (my_decode.size() - 1))
		{
			i = 0;
		}
	}
	//if (i > sizeof(header) / sizeof(int))
	if (0)
	{
		i++;
		cout << "   匹配成功！ " << endl;
		for (; i < temp_decode.size(); i++)
		{
			my_decode.push_back(temp_decode[i]);
		}
	}
	else cout << "匹配失败" << endl;
}

void compare_BER(int position)
{
	cout << " 真实数据 " << " 我的数据 " << endl;
	int maxlength;
	int i = 0;
	int j = 0;
	if (sizeof(channel_data) / sizeof(int) > decode_without_header[position].size()) maxlength = decode_without_header[position].size();
	else maxlength = sizeof(channel_data) / sizeof(int);
	while (j < maxlength && i < maxlength)
	{
		if (channel_data[i] == decode_without_header[position][j])
		{
			cout << channel_data[i] << "                " << decode_without_header[position][j] << endl;
			i++; j++;
		}
		else
		{
			if (i + 2 < maxlength && j + 1 < maxlength && channel_data[i + 1] == decode_without_header[position][j] && channel_data[i + 2] == decode_without_header[position][j + 1])
			{
				cout << " 少解码一位 " << "                " << channel_data[i] << endl;
				i += 2; j++;
			}
			else if (j + 2 < maxlength && i + 1 < maxlength && channel_data[i] == decode_without_header[position][j + 1] && channel_data[i + 1] == decode_without_header[position][j + 2])
			{
				cout << " 多解码一位 " << "                " << decode_without_header[position][j] << endl;
				i++; j += 2;
			}
			else if (j + 2 >= maxlength || j + 1 >= maxlength || i + 2 >= maxlength || i + 1 >= maxlength)
			{
				cout << " 发生溢出 " << endl;
				break;
			}
			else
			{
				cout << "出现大量错误（2个以上）" << endl;
				break;
			}
		}
	}
}

void vote()
{
	int counting_num = 1;                      //1，0重复的次数
	int vote_box[10];                             //存档票数   比如说    vote_box[1]   == 5 表示这个位置 1 个 1 或者  1个 0 有5个人投票赞成
	int index[iteration_time];                 //存档各个data解码的光标位
	int most_fre_v = 0;                          //最频繁值的value
	int most_fre_p = 0;                          //最频繁值的position
	int my_value = decode_without_header[0][0];

	for (int i = 0; i < iteration_time; i++) index[i] = 1;             //初始化光标位置
	while (index[iteration_time - 1] < decode_without_header[iteration_time - 1].size())
	{
		for (int i = 0; i < sizeof(vote_box) / sizeof(int); i++)    vote_box[i] = 0;          //初始化投票箱
		for (int iter_time = 0; iter_time < iteration_time; iter_time++)
		{
			for (; index[iter_time] < decode_without_header[iter_time].size(); )
			{
				if (decode_without_header[iter_time][(index[iter_time] - 1)] == decode_without_header[iter_time][index[iter_time]])
				{
					counting_num++;
					index[iter_time] ++;
					if (index[iter_time] == decode_without_header[iter_time].size())
					{
						vote_box[counting_num] ++;
						counting_num = 1;
						break;
					}
				}
				else
				{
					vote_box[counting_num] ++;
					counting_num = 1;
					index[iter_time] ++;
					break;
				}
			}
		}
		vote_box[0] = my_value;
		if (my_value == 1) my_value = 0;
		else my_value = 1;
		most_fre_v = vote_box[1];
		most_fre_p = 1;
		for (int num = 2; num < 10; num++)
		{
			if (most_fre_v < vote_box[num])
			{
				most_fre_v = vote_box[num];
				most_fre_p = num;
			}
		}
		for (int num = 0; num < most_fre_p; num++) final_decode_data.push_back(vote_box[0]);
	}
}

int main()
{
	Mat src, oneline, split_src[3];
	int channel = 0;
	VideoCapture cap;
	//cap.open("new.mp4");
	cap.open("stack.mp4");
	//cap.open("bubvideo.mp4");
	cap >> src;
	//src = src(Rect(src.cols / 3*2, 0, src.cols / 3-1, src.rows));
	int i = 0;
	/*for (int time = 0; time < iteration_time;time++)
	{
		initial_vector();
		i = 0;
		sca = int(1080.0 / src.rows + 1);
		cout << sca << endl;
		resize(src, src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);                              //用linear 和 nearest 其实差别不大
		split(src, split_src);
		debubble(split_src, channel);
		oneline = split_src[channel](Rect((src.cols - 50), 0, 1, src.rows));
		oneline = oneline.t();
		if (EVA(oneline, 1, time)==0)time--;                                                                             //如果EVA返回的是0，表示这一帧没有两个header
		while (i < 4)
		{
			i++;
			cap >> src;
			src = src(Rect(src.cols / 3*2, 0, src.cols / 3-1, src.rows));
		}
	}
	vote();
	eyes_compare();
	*/

	initial_vector();
	i = 0;
	sca = int(1080.0 / src.rows + 1);
	cout << sca << endl;
	resize(src, src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);                              //用linear 和 nearest 其实差别不大
	split(src, split_src);
	debubble(split_src, channel);
	oneline = split_src[channel](Rect((src.cols - 50), 0, 1, src.rows));
	oneline = oneline.t();
	EVA(oneline, 1, 0);
	vote();
	eyes_compare();
	return 0;
}