#include "EVA.h"

void EVA::Find_local(Mat BGRchannels)       //找出所有的极值点
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

void EVA::Find_frequt()
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

void EVA::Find_usefullocal(Mat BGRchannels)
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

void EVA::Input_csv(Mat BGRchannels, int flag)   //EVA函数实现       //flag == 1则输入csv
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
	for (int i = lominp; i < lomaxp + 1; i++)                    //csv
	{
		if (i == maxpv[tempp1].x)
		{
			local_max = maxpv[tempp1].y;
			if (tempp1 < maxpv.size() - 1)tempp1++;
			t = 1;
			//opt << int(data[i]) << "," << local_max << endl;
		}
		else if (i == minpv[tempp2].x)
		{
			local_min = minpv[tempp2].y;
			if (tempp2 < minpv.size() - 1)tempp2++;
			t = 1;
			//opt << int(data[i]) << "," << local_min << endl;
		}
		temp_thre = (local_max + local_min) / 2;
		Bill = (int(data[i]) > temp_thre);
		opt << int(data[i]) << "," << temp_thre << "," << Bill << endl;
		if (t == 0)opt << int(data[i]) << "," << "0" << "," << Bill << endl;
		decode.push_back(Bill);
	}
	if (flag) opt.close();
}

void EVA::Find_length(int flag)          //对decode进行处理
{
	int num1 = 0;
	int num0 = 0;
	int first_time = 1;
	int error_judge = 0;    //误判矫正
	for (int i = 0; i < decode.size(); i++)
	{
		if (decode[i] == 1 && num0 == 0)
		{
			num1++;
		}
		else if (decode[i] == 1 && num0 != 0)
		{
			if (num0 <= sca * (1 + error_judge))         //出现误判     此时也可能是后面的出现误判，要进一步矫正
			{
				error_judge++;
				num1 += (num0 + 1);
				num0 = 0;
			}
			else          //正常保存
			{
				error_judge = 0;
				if (first_time == 0)
				{
					length[num0]++;
					num_of_lianxv[0].push_back(num0);
					num_of_lianxv[1].push_back(0);
				}
				first_time = 0;
				num0 = 0;
				num1++;
			}
		}
		else if (decode[i] == 0 && num1 == 0)
		{
			num0++;
		}
		else if (decode[i] == 0 && num1 != 0)
		{
			if (num1 <= sca * (1 + error_judge))
			{
				error_judge++;
				num0 += (num1 + 1);
				num1 = 0;
			}
			else
			{
				error_judge = 0;
				if (first_time == 0)
				{
					length[num1]++;
					num_of_lianxv[0].push_back(num1);
					num_of_lianxv[1].push_back(1);
				}
				first_time = 0;
				num1 = 0;
				num0++;
			}
		}
	}
	int store_len[3] = { 0,1,2 };
	int poss = length[0];
	pxlength = 0;
	for (int i = 0; i < sizeof(length) / sizeof(int); i++)
	{
		if (i > 2)
		{
			for (int x = 2; x > 0; x--)                       //升序排列
			{
				for (int j = 0; j < x; j++)
				{
					if (length[store_len[j]] > length[store_len[j + 1]])
					{
						int temp = store_len[j];
						store_len[j] = store_len[j + 1];
						store_len[j + 1] = temp;
					}
					else if (length[store_len[j]] == length[store_len[j + 1]])  //相同时则小数在前面
					{
						if (store_len[j] > store_len[j + 1]) 
						{
							int temp = store_len[j];
							store_len[j] = store_len[j + 1];
							store_len[j + 1] = temp;
						}
					}
				}
			}
			if (length[i] >= length[store_len[0]])           //找出长度频率最小的那个进行替换
			{
				store_len[0] = i;
			}
		}
		cout << i << "  pixel/bit   " << length[i] << endl;        //陈列结果
	}
	for (int x = 2; x > 0; x--)                       //升序排列
	{
		for (int j = 0; j < x; j++)
		{
			if (store_len[j] > store_len[j + 1])
			{
				int temp = store_len[j];
				store_len[j] = store_len[j + 1];
				store_len[j + 1] = temp;
			}
		}
	}
	//////////////////////防止pxlength出现7，14此组合后平均为11 ，而应该是直接为7
	/*if ((float(store_len[2]) / store_len[0] - store_len[2] / store_len[0] == 0 )||(float(store_len[1]) / store_len[0] - store_len[1] / store_len[0] == 0))pxlength = store_len[0];
	else if (float(store_len[2]) / store_len[1] - store_len[2] / store_len[1] == 0) pxlength = store_len[1];*/
	if (store_len[2] / store_len[0] >= 2 || store_len[1] / store_len[0] >= 2) pxlength = store_len[0];
	else if (store_len[2] / store_len[1] >= 2) pxlength = store_len[1];
	else if (store_len[0] - store_len[1] == -1) pxlength = store_len[0];
	else if (store_len[1] - store_len[2] == -1)pxlength = store_len[1];
	else 
	{
		for (int i = 0; i < 3; i++)
		{
			pxlength += store_len[i];
		}
		pxlength /= 3;
	}
	int int_num = int(pxlength);
	if (pxlength - int_num != 0) pxlength = int_num + 1;
	cout << pxlength << "  pixel/bit(mostly possible)   " << endl;      //找出最有可能的  first_time  再找小数的pxlength

//////////////////////////////////////接下来找小数的pxlength
	int sum_time = 0;           //length的次数
	int sum_length = 0;           //pxlength的取值               //上面两个参数用于求加权平均
	for (int i = 0; i <= pxlength * 1.5; i++)
	{
		sum_time += length[i];
		sum_length += length[i] * i;
	}
	pxlength = float(sum_length) / sum_time;
	cout << "  pixel/bit(mostly possible)  second_time" << pxlength << endl;
	cout << "num_of_lianxv" << endl;
	for (int i = 0; i < num_of_lianxv[0].size(); i++)
	{
		cout << num_of_lianxv[0][i] << "  ";
	}
	cout << endl;
	for (int i = 0; i < num_of_lianxv[0].size(); i++)
	{
		cout << num_of_lianxv[1][i];
	}
	cout << endl;
}

void EVA::Find_real_data()
{
	int de_num = 0;                 //代表0还是1
	int int_num = 0;                //整数部分
	float left_num = 0;   //下一个的余数
	int flag = 0;                 //用于记录是否要回溯
	vector<int>store_i;
	int* len;
	int* _01_;
	int* avil_way;                         //用于记录是否存在0.5的进位   或者   是小数的直接进位   -1为减，0为未使用，1为已使用
	int* sum_len;                          //关联前一个的len
	int* source_len;                       //用len直接转换而成
	len = new int[num_of_lianxv[0].size()];
	_01_ = new int[num_of_lianxv[0].size()];
	avil_way = new int[num_of_lianxv[0].size()];
	sum_len = new int[num_of_lianxv[0].size() - 1];
	source_len = new int[num_of_lianxv[0].size() - 1];
	for (int i = 0; i < num_of_lianxv[0].size(); i++)
	{
		avil_way[i] = 0;          //初始化
		de_num = num_of_lianxv[1][i];
		int_num = num_of_lianxv[0][i] / pxlength;
		left_num = num_of_lianxv[0][i] / pxlength - int_num;
		if (int_num >= 1)
		{
			if (left_num >= 0.5)
			{
				len[i] = (int_num + 1);
				avil_way[i] = 1;
				_01_[i] = de_num;
			}
			else
			{
				len[i] = int_num;
				_01_[i] = de_num;
			}
		}
		else
		{
			len[i] = 1;
			avil_way[i] = 1;
			_01_[i] = de_num;
		}
	}
	for (int i = 0; i < num_of_lianxv[0].size() - 1; i++)                   //填充sum_len和source_len
	{
		int temp = i + 1;
		int Sum_len = num_of_lianxv[0][i] + num_of_lianxv[0][temp];
		int_num = Sum_len / pxlength;
		left_num = Sum_len / pxlength - int_num;
		if (left_num >= 0.45) int_num++;
		sum_len[i] = int_num;
		source_len[i] = len[i] + len[temp];
	}
	for (int i = 0; i < num_of_lianxv[0].size() - 1; i++)              //二次筛选，将解码之间的关系建立起来                       
	{
		int temp = i + 1;
		if (sum_len[i] - source_len[i] == 1)                              //需要添加一个
		{
			if (avil_way[i] == avil_way[temp])                    //存在争议部分 可将前后相关联进行改造
			{
				if (avil_way[i] == 1) cout << "出现len错误1，请检查" << endl;
				else { store_i.push_back(i); }
			}
			else                   //成功处理
			{
				if (avil_way[i] < avil_way[temp])
				{
					len[i] ++; avil_way[i] = 1;
					if (store_i.size() != 0 && store_i[store_i.size() - 1] == i - 1) //回到上一个位点进行运算
					{
						store_i.pop_back();
						i = i - 2;
					}
				}
				else { len[temp]++; avil_way[temp] = 1; }
			}
		}
		else if (source_len[i] - sum_len[i] == 1)                        //需要减少一个
		{
			if (avil_way[i] == avil_way[temp])
			{
				if (avil_way[i] == -1)  cout << "出现len错误2，请检查" << endl;
				else
				{
					if (len[i] == 1 && len[temp] != 1)        //成功处理
					{
						len[temp]--;
						avil_way[temp] = -1;
					}
					else if (len[i] != 1 && len[temp] == 1)
					{
						len[i] --;
						avil_way[i] = -1;
						if (store_i.size() != 0 && store_i[store_i.size() - 1] == i - 1) //回到上一个位点进行运算
						{
							store_i.pop_back();
							i = i - 2;
						}
					}
					else
					{
						store_i.push_back(i);
					}
				}
			}
			else
			{
				if (avil_way[i] > avil_way[temp])      //成功处理
				{
					if (len[i] != 1)
					{
						len[i] --;
						avil_way[i] = -1;
						if (store_i.size() != 0 && store_i[store_i.size() - 1] == i - 1) //回到上一个位点进行运算
						{
							store_i.pop_back();
							i = i - 2;
						}
					}
					else if (len[temp] != 1)
					{
						len[temp] --;
						avil_way[temp] = -1;
					}
					else
					{
						cout << "出现两个1但要减去1" << endl;
					}
				}
				else
				{
					if (len[temp] != 1)
					{
						len[temp]--;
						avil_way[temp] = -1;
					}
					else if (len[i] != 1)
					{
						len[i] --;
						avil_way[i] = -1;
						if (store_i.size() != 0 && store_i[store_i.size() - 1] == i - 1) //回到上一个位点进行运算
						{
							store_i.pop_back();
							i = i - 2;
						}
					}
					else
					{
						cout << "出现两个1但要减去1" << endl;
					}
				}
			}
		}
	}
	for (int i = 0; i < store_i.size(); i++)
	{
		int temp = i + 1;
		if (i < store_i.size() - 1 && abs(store_i[i] - store_i[temp]) == 1)              //相邻元素
		{
			if (sum_len[store_i[i]] - source_len[store_i[i]] == sum_len[store_i[temp]] - source_len[store_i[temp]])         //同等类型
			{
				if (sum_len[store_i[i]] - source_len[store_i[i]] == 1) { len[store_i[temp]]++; avil_way[store_i[temp]] = 1; } //加
				else if (source_len[store_i[i]] - sum_len[store_i[i]] == 1)                   //减
				{
					if (len[store_i[temp]] != 1)
					{
						len[store_i[temp]]--; avil_way[store_i[temp]] = -1;
					}
					else { cout << "出现错误" << endl; }
				}
			}
			else { cout << "出现错误" << endl; }
			i++;                 //成对的进行处理
		}
		else                   //单独的进行处理此时   已经无法直接从他和后者的联系判断出len了
		{
			if (sum_len[store_i[i]] - source_len[store_i[i]] == 1)       ///+
			{
				if (num_of_lianxv[0][store_i[i]] >= num_of_lianxv[0][store_i[i] + 1])
				{
					len[store_i[i]]++; avil_way[store_i[i]] = 1;
				}
				else
				{
					len[store_i[i] + 1]++; avil_way[store_i[i] + 1] = 1;
				}
			}
			else if (source_len[store_i[i]] - sum_len[store_i[i]] == 1)        ///-
			{
				if (num_of_lianxv[0][store_i[i]] >= num_of_lianxv[0][store_i[i] + 1])
				{
					if (len[store_i[i]] != 1)
					{
						len[store_i[i]]--; avil_way[store_i[i]] = -1;
					}
					else if (len[store_i[i] + 1] != 1)
					{
						len[store_i[i] + 1]--; avil_way[store_i[i] + 1] = -1;
					}
					else { cout << "出现错误" << endl; }
				}
			}
		}
	}

	for (int i = 0; i < num_of_lianxv[0].size(); i++)
	{
		for (int j = 0; j < len[i]; j++) my_decode.push_back(_01_[i]);
	}
	cout << "my_decode如下：" << endl;
	for (int i = 0; i < my_decode.size(); i++)cout << my_decode[i];
	cout << endl;
	delete[num_of_lianxv[0].size()] avil_way;
	delete[num_of_lianxv[0].size()] _01_;
	delete[num_of_lianxv[0].size()] len;
	delete[num_of_lianxv[0].size() - 1] sum_len;
	delete[num_of_lianxv[0].size() - 1] source_len;
}

void EVA::Find_D_with_header(int flag,int header_box[])
{
	int numhead = 0;
	int i = 0;
	int First_header = 0;
	int Second_header = 0;
	for (; i < my_decode.size() - 1; i++)
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
	if (i < my_decode.size() - 1)
	{
		First_header = i - header_length;           //找到完整的第1个头
		cout << "First_header" << First_header << endl;
		for (; i < my_decode.size() - 1; i++)
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
		if (i < my_decode.size() - 1) { Second_header = i - header_length; }
		else if (i == my_decode.size() - 1 && numhead != 0)
		{
			if (my_decode[i] == 1) 
			{
				numhead++;
				if (numhead >= header_length) Second_header = i - header_length ;
				else { Second_header = i - numhead + 1; }
			} 
			else 
			{
				if (numhead >= header_length) Second_header = i - header_length;
			}
		}
		else if(i == my_decode.size() && numhead != 0)
		{
			if (numhead >= header_length) Second_header = i - header_length;
			else { Second_header = i - numhead; }
		}
		else{ Second_header = my_decode.size()-1; }
		cout << "Second_header" << Second_header << endl;
		cout << endl;
	}
	else 
	{
		First_header = my_decode.size()-1;
		cout << "First_header" << First_header << endl;
		Second_header = my_decode.size() - 1;
		cout << "Second_header" << Second_header << endl;
		cout << endl;
	}

	if (First_header == Second_header) { header_box[0] = header_box[1] = First_header; header_box[2] = 0; }
	else 
	{
		for (i = First_header; i < Second_header; i++)
		{
			D_with_header.push_back(my_decode[i]);
			if (flag)inter_fix[inter_num].push_back(my_decode[i]);
		}
		if (flag)inter_num++;
		cout << endl;
		header_box[0] = First_header;
		header_box[1] = Second_header;
		header_box[2] = 1;
	}
}

void EVA::initial_vector()
{
	maxpv.clear();
	minpv.clear();
	decode.clear();
	num_of_lianxv[0].clear();
	num_of_lianxv[1].clear();
	my_decode.clear();
	D_with_header.clear();
	for (int i = 0; i < 50; i++)length[i] = 0;
}

void EVA::Sum_EVA(Mat BGRchannels,int header_box[], int flag)
{
	int First_H = 0;
	minpv.clear();
	maxpv.clear();
	Find_local(BGRchannels);            //找 0 ，1代表
	Find_frequt();                                                //设置极大值和极小值阈值
	Find_usefullocal(BGRchannels);                   //开始找真的local
	Input_csv(BGRchannels, 1);                        //输出像素0，1
	Find_length(flag);               //找0，1对应length
	Find_real_data();             //找真实数据
	Find_D_with_header(flag,header_box);          //找不包含header的数据
}