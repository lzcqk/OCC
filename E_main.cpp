#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<fstream>
#include"EVA.h"
#include"defin.h"
#include"Msrcr.h"

using namespace std;
using namespace cv;                  //flag形参用于将来跟踪下一个帧

double  power[3] = { 0.333333333,0.333333333,0.333333333 };
double sigma[3] = { 15,80,200 };


///////////////////////////////////////////stack.mp4 && bubvideo.mp4
int channel = 2;                          //测试的通道
int channel_data[] = { 1,0,1,0,1,0,1,0,1,0,1,1,0,1,1,0,1,0,0,0,0,1,1,0,1,1,1,0,1,1,0,0,1,0,0,0,1,1,1,0,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,0,0,0 };
//Bchannel_data = {1,0,1,0,1,0,1,0,1,0,1,1,1,0,1,0,0,0,0,0,0,0,1,0,1,1,1,0,0,0,1,1,0,1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,0,1,1,0,1,1,1,1,1,0,1,0}
//Gchannel_data = {1,0,1,0,1,0,1,0,1,0,1,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,1,1,0,1,0,0,1,1,0,1,0,0,1,1,1}
//Rchannel_data = {1,0,1,0,1,0,1,0,1,0,1,1,0,1,1,0,1,0,0,0,0,1,1,0,1,1,1,0,1,1,0,0,1,0,0,0,1,1,1,0,0,1,0,0,1,0,1,1,1,0,1,1,0,1,1,0,0,1,1,1}


/////////////////////////////////////////////////new.mp4
//Gray_data = { 1,0,1,0,1,0,1,0,1,0,1,1,0,1,1,0,1,0,0,0,0,1,1,0,1,1,1,0,1,1,0,0,1,0,0,0,1,1,1,0,0,1,0,0,1,0,1,1,1,0,1,1,1,1,1,0,0,1,0,0,0,1,1,1,0,0,0,1,0,1,1,0,0,0,0,0,1,0,0,1,0,0,1,0,1,1,1,0,1,0,1,1,1,0,1,0,0,0,0,0}

void vote_data()
{
	const int box_size = 200;
	int vote_box[box_size];
	for (int i = 0; i < box_size; i++)vote_box[i] = 0;
	for (int in_time = 0; in_time < 10; in_time++)
	{
		if (inter_fix[in_time].size() < box_size)
		{
			vote_box[inter_fix[in_time].size()]++;
		}
	}
	int maxlen = 0;               //长度
	int maxseq = vote_box[0];           //频率
	for (int i = 1; i < box_size; i++)
	{
		if (maxseq < vote_box[i])
		{
			maxlen = i;
			maxseq = vote_box[i];
		}
	}
	cout << "最大频率为" << maxseq << endl;
	cout << "最大长度为" << maxlen << endl;
	int* sto_use_D;
	sto_use_D = new int[maxseq];
	int ptr = 0;
	for (int in_t = 0; in_t < 10; in_t++)         //将相同长度的解码的index记录下来
	{
		if (inter_fix[in_t].size() == maxlen)            //保存index
		{
			sto_use_D[ptr] = in_t;
			ptr++;
		}
	}
	D_with_header.clear();
	int zero_or_one[2];                //保存0，1的次数
	for (int i = 0; i < maxlen; i++)         //开始各个位置的连接计数
	{
		zero_or_one[0] = 0;
		zero_or_one[1] = 0;
		for (int j = 0; j < maxseq; j++)
		{
			int dat = inter_fix[sto_use_D[j]][i];
			zero_or_one[dat]++;
		}
		if (zero_or_one[0] > zero_or_one[1]) D_with_header.push_back(0);
		else if (zero_or_one[0] < zero_or_one[1]) D_with_header.push_back(1);
		else D_with_header.push_back(2);
	}
}

void eyes_compare()
{
	cout << "我的数据：" << endl;
	for (int i = 0; i < D_with_header.size(); i++)
	{
		cout << D_with_header[i];
	}
	cout << endl;
	cout << "真的数据：" << endl;
	for (int i = 0; i < sizeof(channel_data) / sizeof(int); i++)
	{
		cout << channel_data[i];
	}
	cout << endl;
}

void count_error_rate(int First_header, int Second_header, int flag)
{
	if (flag == 1)                //找到了匹配的数据包
	{
		int erroR = 0;
		int using_BER = 1;         //用于指出此处的BER是否有效 1为有效 0 为无效

		int real_data_len = sizeof(channel_data) / sizeof(int);
		int my_data_len = Second_header - First_header;
		vector<int> real_len_01_num[2];            //real_data
		vector<int> my_len_01_num[2];            //my_data
		int num0 = 0;
		int num1 = 0;

		////////////////////////////////////////将数据的data转换为len —— num的组合
		for (int i = 0; i < real_data_len; i++)
		{
			if (channel_data[i] == 0 && num1 == 0)
			{
				num0++;
			}
			else if (channel_data[i] == 0 && num1 != 0)
			{
				num0++;
				real_len_01_num[0].push_back(num1);
				real_len_01_num[1].push_back(1);
				num1 = 0;
			}
			else if (channel_data[i] == 1 && num0 == 0)
			{
				num1++;
			}
			else if (channel_data[i] == 1 && num0 != 0)
			{
				num1++;
				real_len_01_num[0].push_back(num0);
				real_len_01_num[1].push_back(0);
				num0 = 0;
			}
		}
		if (num0 != 0)
		{
			real_len_01_num[0].push_back(num0);
			real_len_01_num[1].push_back(0);
		}
		else
		{
			real_len_01_num[0].push_back(num1);
			real_len_01_num[1].push_back(1);
		}
		num0 = 0;
		num1 = 0;


		/////////////////////////////////////////处理数据包部分
		if (real_data_len == my_data_len)          //长度匹配
		{
			int real_data_index = 0;
			int my_data_index = First_header;
			for (; real_data_index < real_data_len; real_data_index++, my_data_index++)
			{
				if (channel_data[real_data_index] != my_decode[my_data_index])erroR++;
			}
		}
		else                              //长度不匹配
		{
			///////////////////////////////////////求my_decode数据包部分的len和num 
			for (int i = First_header; i < Second_header; i++)  //ptr 指向的是my_decode的index
			{
				if (my_decode[i] == 0 && num1 == 0)
				{
					num0++;
				}
				else if (my_decode[i] == 0 && num1 != 0)
				{
					num0++;
					my_len_01_num[0].push_back(num1);
					my_len_01_num[1].push_back(1);
					num1 = 0;
				}
				else if (my_decode[i] == 1 && num0 == 0)
				{
					num1++;
				}
				else if (my_decode[i] == 1 && num0 != 0)
				{
					num1++;
					my_len_01_num[0].push_back(num0);
					my_len_01_num[1].push_back(0);
					num0 = 0;
				}
			}
			if (num0 != 0)
			{
				my_len_01_num[0].push_back(num0);
				my_len_01_num[1].push_back(0);
			}
			else
			{
				my_len_01_num[0].push_back(num1);
				my_len_01_num[1].push_back(1);
			}
			num0 = 0;
			num1 = 0;

			int history_less = -100;                   //用于解决误判  出现误判   00011  判断成     00111 此情况
			int history_more = -100;
			int ptr_r = 0;
			for (int ptr_i = 0; ptr_i < my_len_01_num[0].size(); ptr_i++, ptr_r++)
			{
				if (ptr_r >= real_len_01_num[0].size()) { cout << "错误0" << endl; using_BER = 0; ptr_i = my_len_01_num[0].size(); break; }       //此处应该只包含一个数据包因此如果ptr_r越界则说明出现错误
				int diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i];   //大于0 我少了     小于0 我多了
				if (ptr_i < my_len_01_num[0].size() - 1)               //不为最后一个
				{
					//情况1               完全匹配
					if (diff_num == 0) {}
					//情况2               多或者少1个位   
					else if (abs(diff_num) == 1)
					{
						if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //出现误判 现在我多一个 而上一回合 我少一个 所以多和少的那个为误判元素
						else if (history_more == (ptr_i - 1) && diff_num == 1) {}
						else { erroR++; }                               //未出现误判 
						if (diff_num == 1)history_less = ptr_i;
						else history_more = ptr_i;
					}
					//情况3               出现多个位置的缺少 此时可能是 0010 被 解码成 00000之类的 形成误解码 或者 0000 被解码成 01000
					else
					{
						if (diff_num > 0)                      //////////////////我的数据少了
						{
							if (ptr_i < my_len_01_num[0].size() - 2)           //加两个数据
							{
								diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i] - my_len_01_num[0][ptr_i + 1] - my_decode[ptr_i + 2];
								if (abs(diff_num) == 0)
								{
									erroR += my_len_01_num[0][ptr_i + 1];
									ptr_i += 2;          //位移两位
								}
								if (abs(diff_num) == 1)
								{
									erroR += (my_len_01_num[0][ptr_i + 1] + 1);
									ptr_i += 2;
								}
								else      //无法处理 
								{
									cout << "错误1" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								erroR += my_len_01_num[0][my_len_01_num[0].size() - 1];
								ptr_i++;
							}
						}
						else if (diff_num < 0)     //我的数据多了
						{
							if (ptr_r < real_len_01_num[0].size() - 2)
							{
								diff_num = real_len_01_num[0][ptr_r] + real_len_01_num[0][ptr_r + 1] + real_len_01_num[0][ptr_r + 2] - my_len_01_num[0][ptr_i];
								if (abs(diff_num) == 0)
								{
									erroR += real_len_01_num[0][ptr_r + 1];
									ptr_r += 2;
								}
								if (abs(diff_num) == 1)
								{
									erroR += (real_len_01_num[0][ptr_r + 1] + 1);
									ptr_r += 2;
								}
								else
								{
									cout << "错误3" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								cout << "错误4" << endl;
								using_BER = 0;
								ptr_i = my_len_01_num[0].size();
							}
						}
					}
				}
				else   //为最后一个 
				{
					if (Second_header == my_decode.size() - 1)  //未找到第二个头
					{
						if (diff_num >= 0) {}
						else { erroR += abs(diff_num); }
					}
					else
					{
						if (ptr_r != real_len_01_num[0].size() - 1)       //出现错误
						{
							cout << "错误5" << endl;
							using_BER = 0;
						}
						else  //正常处理    如上
						{
							if (abs(diff_num) == 1)
							{
								if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //出现误判 现在我多一个 而上一回合 我少一个 所以多和少的那个为误判元素
								else if (history_more == (ptr_i - 1) && diff_num == 1) {}
								else { erroR++; }                               //未出现误判 
								if (diff_num == 1)history_less = ptr_i;
								else history_more = ptr_i;
							}
							else
							{
								erroR += abs(diff_num);
							}
						}
					}
				}
			}
		}

		////////////////////////////////////////处理Second_header后面部分
		my_len_01_num[0].clear(); my_len_01_num[1].clear();
		if (Second_header != my_decode.size() - 1 && using_BER != 0)
		{
			for (int i = Second_header; i < my_decode.size(); i++)  //ptr 指向的是my_decode的index
			{
				if (my_decode[i] == 0 && num1 == 0)
				{
					num0++;
				}
				else if (my_decode[i] == 0 && num1 != 0)
				{
					num0++;
					my_len_01_num[0].push_back(num1);
					my_len_01_num[1].push_back(1);
					num1 = 0;
				}
				else if (my_decode[i] == 1 && num0 == 0)
				{
					num1++;
				}
				else if (my_decode[i] == 1 && num0 != 0)
				{
					num1++;
					my_len_01_num[0].push_back(num0);
					my_len_01_num[1].push_back(0);
					num0 = 0;
				}
			}
			if (num0 != 0)
			{
				my_len_01_num[0].push_back(num0);
				my_len_01_num[1].push_back(0);
			}
			else
			{
				my_len_01_num[0].push_back(num1);
				my_len_01_num[1].push_back(1);
			}
			num0 = 0;
			num1 = 0;
			int history_less = -100;
			int history_more = -100;
			int ptr_r = 0;
			for (int ptr_i = 0; ptr_i < my_len_01_num[0].size(); ptr_i++, ptr_r++)
			{
				if (ptr_r >= real_len_01_num[0].size()) { ptr_r = ptr_r - real_len_01_num[0].size(); }
				int diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i];   //大于0 我少了     小于0 我多了
				if (ptr_i < my_len_01_num[0].size() - 1)               //不为最后一个
				{
					//情况1               完全匹配
					if (diff_num == 0) {}
					//情况2               多或者少1个位   
					else if (abs(diff_num) == 1)
					{
						if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //出现误判 现在我多一个 而上一回合 我少一个 所以多和少的那个为误判元素
						else if (history_more == (ptr_i - 1) && diff_num == 1) {}
						else { erroR++; }                               //未出现误判 
						if (diff_num == 1)history_less = ptr_i;
						else history_more = ptr_i;
					}
					//情况3               出现多个位置的缺少 此时可能是 0010 被 解码成 00000之类的 形成误解码 或者 0000 被解码成 01000
					else
					{
						if (diff_num > 0)                      //////////////////我的数据少了
						{
							if (ptr_i < my_len_01_num[0].size() - 2)           //加两个数据
							{
								diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i] - my_len_01_num[0][ptr_i + 1] - my_decode[ptr_i + 2];
								if (abs(diff_num) == 0)
								{
									erroR += my_len_01_num[0][ptr_i + 1];
									ptr_i += 2;          //位移两位
								}
								if (abs(diff_num) == 1)
								{
									erroR += (my_len_01_num[0][ptr_i + 1] + 1);
									ptr_i += 2;
								}
								else      //无法处理 
								{
									cout << "错误6" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								erroR += my_len_01_num[0][my_len_01_num[0].size() - 1];
								ptr_i++;
							}
						}
						else if (diff_num < 0)     //我的数据多了
						{
							int temp1 = ptr_r + 1;
							int temp2 = ptr_r + 2;
							if (temp1 >= real_len_01_num[0].size()) { temp1 -= real_len_01_num[0].size(); }
							if (temp2 >= real_len_01_num[0].size()) { temp2 -= real_len_01_num[0].size(); }
							diff_num = real_len_01_num[0][ptr_r] + real_len_01_num[0][temp1] + real_len_01_num[0][temp2] - my_len_01_num[0][ptr_i];
							if (abs(diff_num) == 0)
							{
								erroR += real_len_01_num[0][temp1];
								ptr_r += 2;
							}
							if (abs(diff_num) == 1)
							{
								erroR += (real_len_01_num[0][temp1] + 1);
								ptr_r += 2;
							}
							else
							{
								cout << "错误7" << endl;
								using_BER = 0;
								ptr_i = my_len_01_num[0].size();
							}
						}
					}
				}
				else   //为最后一个 
				{
					if (diff_num >= 0) {}
					else { erroR += abs(diff_num); }
				}
			}
		}

		////////////////////////////////////////处理First_header前面部分
		my_len_01_num[0].clear(); my_len_01_num[1].clear();
		real_len_01_num[0].clear(); real_len_01_num[1].clear();
		///////////////////////////////////////求my_decode前半部分的len和num 
		if (using_BER != 0)
		{
			for (int i = real_data_len-1; i >= 0; i--)
			{
				if (channel_data[i] == 0 && num1 == 0)
				{
					num0++;
				}
				else if (channel_data[i] == 0 && num1 != 0)
				{
					num0++;
					real_len_01_num[0].push_back(num1);
					real_len_01_num[1].push_back(1);
					num1 = 0;
				}
				else if (channel_data[i] == 1 && num0 == 0)
				{
					num1++;
				}
				else if (channel_data[i] == 1 && num0 != 0)
				{
					num1++;
					real_len_01_num[0].push_back(num0);
					real_len_01_num[1].push_back(0);
					num0 = 0;
				}
			}
			if (num0 != 0)
			{
				real_len_01_num[0].push_back(num0);
				real_len_01_num[1].push_back(0);
			}
			else
			{
				real_len_01_num[0].push_back(num1);
				real_len_01_num[1].push_back(1);
			}
			num0 = 0;
			num1 = 0;

			for (int i = First_header-1; i >= 0; i--)  //ptr 指向的是my_decode的index
			{
				if (my_decode[i] == 0 && num1 == 0)
				{
					num0++;
				}
				else if (my_decode[i] == 0 && num1 != 0)
				{
					num0++;
					my_len_01_num[0].push_back(num1);
					my_len_01_num[1].push_back(1);
					num1 = 0;
				}
				else if (my_decode[i] == 1 && num0 == 0)
				{
					num1++;
				}
				else if (my_decode[i] == 1 && num0 != 0)
				{
					num1++;
					my_len_01_num[0].push_back(num0);
					my_len_01_num[1].push_back(0);
					num0 = 0;
				}
			}
			if (num0 != 0)
			{
				my_len_01_num[0].push_back(num0);
				my_len_01_num[1].push_back(0);
			}
			else
			{
				my_len_01_num[0].push_back(num1);
				my_len_01_num[1].push_back(1);
			}
			num0 = 0;
			num1 = 0;

			int history_less = -100;
			int history_more = -100;
			int ptr_r = 0;
			for (int ptr_i = 0; ptr_i < my_len_01_num[0].size(); ptr_i++, ptr_r++)
			{
				if (ptr_r >= real_len_01_num[0].size()) { cout << "错误10" << endl; using_BER = 0; ptr_i = my_len_01_num[0].size(); break; }
				int diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i];   //大于0 我少了     小于0 我多了
				if (ptr_i < my_len_01_num[0].size() - 1)               //不为最后一个
				{
					//情况1               完全匹配
					if (diff_num == 0) {}
					//情况2               多或者少1个位   
					else if (abs(diff_num) == 1)
					{
						if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //出现误判 现在我多一个 而上一回合 我少一个 所以多和少的那个为误判元素
						else if (history_more == (ptr_i - 1) && diff_num == 1) {}
						else { erroR++; }                               //未出现误判 
						if (diff_num == 1)history_less = ptr_i;
						else history_more = ptr_i;
					}
					//情况3               出现多个位置的缺少 此时可能是 0010 被 解码成 00000之类的 形成误解码 或者 0000 被解码成 01000
					else
					{
						if (diff_num > 0)                      //////////////////我的数据少了
						{
							if (ptr_i < my_len_01_num[0].size() - 2)           //加两个数据
							{
								diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i] - my_len_01_num[0][ptr_i + 1] - my_decode[ptr_i + 2];
								if (abs(diff_num) == 0)
								{
									erroR += my_len_01_num[0][ptr_i + 1];
									ptr_i += 2;          //位移两位
								}
								if (abs(diff_num) == 1)
								{
									erroR += (my_len_01_num[0][ptr_i + 1] + 1);
									ptr_i += 2;
								}
								else      //无法处理 
								{
									cout << "错误11" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								erroR += my_len_01_num[0][my_len_01_num[0].size() - 1];
								ptr_i++;
							}
						}
						else if (diff_num < 0)     //我的数据多了
						{
							if (ptr_r < real_len_01_num[0].size() - 2)
							{
								diff_num = real_len_01_num[0][ptr_r] + real_len_01_num[0][ptr_r + 1] + real_len_01_num[0][ptr_r + 2] - my_len_01_num[0][ptr_i];
								if (abs(diff_num) == 0)
								{
									erroR += real_len_01_num[0][ptr_r + 1];
									ptr_r += 2;
								}
								if (abs(diff_num) == 1)
								{
									erroR += (real_len_01_num[0][ptr_r + 1] + 1);
									ptr_r += 2;
								}
								else
								{
									cout << "错误12" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								cout << "错误13" << endl;
								using_BER = 0;
								ptr_i = my_len_01_num[0].size();
							}
						}
					}
				}
				else   //为最后一个 
				{
					if (diff_num >= 0) {}
					else { erroR += abs(diff_num); }
				}
			}
		}

		if (using_BER == 1)
		{
			error_num += erroR;
			sum_num += my_decode.size() + 1;
		}
	}
	else
	{
		cout << "出现无法处理数据，请在记事本中手动查看" << endl;
		ofstream open_txt("error_data.txt", ios::app);
		for (int i = 0; i < my_decode.size(); i++) open_txt << my_decode[i];
		open_txt << endl;
		open_txt.close();
	}
}

/*
int main()                         //找数据包   10帧投票
{
	EVA my_eva;
	Mat src, oneline, split_src[3];
	Mat gama_pic;
	VideoCapture cap;
	cap.open("stack.mp4");
	//cap.open("bubvideo.mp4");
	//cap.open("new.mp4");
	for (int i = 0; i < 10; i++) cap >> src;
	for (int i = 0; i < 10; i++)
	{
		cap >> src;
		my_eva.initial_vector();
		sca = int(1080.0 / src.rows + 1);
		cout << sca << endl;
		resize(src, src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);                              //用linear 和 nearest 其实差别不大
		//cvtColor(src, src, COLOR_BGR2GRAY);
		//debubble(src);
		//oneline = src(Rect((src.cols - 1), 0, 1, src.rows));
		split(src, split_src);               //RGB分离
		debubble(split_src[channel]); //RGB分离
		oneline = split_src[channel](Rect((src.cols - 1), 0, 1, src.rows));//RGB分离
		oneline = oneline.t();
		my_eva.Sum_EVA(oneline);
		eyes_compare();
	}
	vote_data();
	for(int i = 0;i<5;i++) cout << endl;
	cout << "最终结果为" << endl;
	eyes_compare();
	return 0;
}

*/

int main()                  //求BER
{
	EVA my_eva;
	Msrcr my_MSR;
	Mat src, oneline, split_src[3];
	Mat src1;
	int header_box[3];              //第一位为First_header 第二位为Second_header 第三位为是否找到header
	VideoCapture cap;
	//cap.open("stack.mp4");
	cap.open("new.mp4");
	ofstream open_txt("error_data.txt", ios::trunc);                  //清空文件
	open_txt.close();
	while (1)
	{
		cap >> src;
		if (src.empty()) break;
		//imshow("0", src);
		//src = my_MSR.MultiScaleRetinex(src, power, sigma);
		//imshow("1", src);
		//waitKey(0);
		my_eva.initial_vector();
		sca = int(1080.0 / src.rows + 1);
		cout << "sca == " << sca << endl;
		resize(src, src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);
		cvtColor(src, src, COLOR_BGR2GRAY);
		cv::sort(src, src, SORT_EVERY_ROW + SORT_ASCENDING);
		oneline = src(Rect((src.cols - 2), 0, 1, src.rows));
		//split(src, split_src);               //RGB分离
		//cv::sort(split_src[channel], split_src[channel], SORT_EVERY_ROW + SORT_ASCENDING);
		//oneline = split_src[channel](Rect((src.cols - 2), 0, 1, src.rows));
		oneline = oneline.t();
		my_eva.Sum_EVA(oneline, header_box, 0);
		eyes_compare();
		count_error_rate(header_box[0], header_box[1], header_box[2]);
	}
	cout << "BER为" << error_num / sum_num << endl;
	return 0;
}



