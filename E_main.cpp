#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include<fstream>
#include"EVA.h"
#include"defin.h"
#include"Msrcr.h"

using namespace std;
using namespace cv;                  //flag�β����ڽ���������һ��֡

double  power[3] = { 0.333333333,0.333333333,0.333333333 };
double sigma[3] = { 15,80,200 };


///////////////////////////////////////////stack.mp4 && bubvideo.mp4
int channel = 2;                          //���Ե�ͨ��
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
	int maxlen = 0;               //����
	int maxseq = vote_box[0];           //Ƶ��
	for (int i = 1; i < box_size; i++)
	{
		if (maxseq < vote_box[i])
		{
			maxlen = i;
			maxseq = vote_box[i];
		}
	}
	cout << "���Ƶ��Ϊ" << maxseq << endl;
	cout << "��󳤶�Ϊ" << maxlen << endl;
	int* sto_use_D;
	sto_use_D = new int[maxseq];
	int ptr = 0;
	for (int in_t = 0; in_t < 10; in_t++)         //����ͬ���ȵĽ����index��¼����
	{
		if (inter_fix[in_t].size() == maxlen)            //����index
		{
			sto_use_D[ptr] = in_t;
			ptr++;
		}
	}
	D_with_header.clear();
	int zero_or_one[2];                //����0��1�Ĵ���
	for (int i = 0; i < maxlen; i++)         //��ʼ����λ�õ����Ӽ���
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
	cout << "�ҵ����ݣ�" << endl;
	for (int i = 0; i < D_with_header.size(); i++)
	{
		cout << D_with_header[i];
	}
	cout << endl;
	cout << "������ݣ�" << endl;
	for (int i = 0; i < sizeof(channel_data) / sizeof(int); i++)
	{
		cout << channel_data[i];
	}
	cout << endl;
}

void count_error_rate(int First_header, int Second_header, int flag)
{
	if (flag == 1)                //�ҵ���ƥ������ݰ�
	{
		int erroR = 0;
		int using_BER = 1;         //����ָ���˴���BER�Ƿ���Ч 1Ϊ��Ч 0 Ϊ��Ч

		int real_data_len = sizeof(channel_data) / sizeof(int);
		int my_data_len = Second_header - First_header;
		vector<int> real_len_01_num[2];            //real_data
		vector<int> my_len_01_num[2];            //my_data
		int num0 = 0;
		int num1 = 0;

		////////////////////////////////////////�����ݵ�dataת��Ϊlen ���� num�����
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


		/////////////////////////////////////////�������ݰ�����
		if (real_data_len == my_data_len)          //����ƥ��
		{
			int real_data_index = 0;
			int my_data_index = First_header;
			for (; real_data_index < real_data_len; real_data_index++, my_data_index++)
			{
				if (channel_data[real_data_index] != my_decode[my_data_index])erroR++;
			}
		}
		else                              //���Ȳ�ƥ��
		{
			///////////////////////////////////////��my_decode���ݰ����ֵ�len��num 
			for (int i = First_header; i < Second_header; i++)  //ptr ָ�����my_decode��index
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

			int history_less = -100;                   //���ڽ������  ��������   00011  �жϳ�     00111 �����
			int history_more = -100;
			int ptr_r = 0;
			for (int ptr_i = 0; ptr_i < my_len_01_num[0].size(); ptr_i++, ptr_r++)
			{
				if (ptr_r >= real_len_01_num[0].size()) { cout << "����0" << endl; using_BER = 0; ptr_i = my_len_01_num[0].size(); break; }       //�˴�Ӧ��ֻ����һ�����ݰ�������ptr_rԽ����˵�����ִ���
				int diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i];   //����0 ������     С��0 �Ҷ���
				if (ptr_i < my_len_01_num[0].size() - 1)               //��Ϊ���һ��
				{
					//���1               ��ȫƥ��
					if (diff_num == 0) {}
					//���2               �������1��λ   
					else if (abs(diff_num) == 1)
					{
						if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //�������� �����Ҷ�һ�� ����һ�غ� ����һ�� ���Զ���ٵ��Ǹ�Ϊ����Ԫ��
						else if (history_more == (ptr_i - 1) && diff_num == 1) {}
						else { erroR++; }                               //δ�������� 
						if (diff_num == 1)history_less = ptr_i;
						else history_more = ptr_i;
					}
					//���3               ���ֶ��λ�õ�ȱ�� ��ʱ������ 0010 �� ����� 00000֮��� �γ������ ���� 0000 ������� 01000
					else
					{
						if (diff_num > 0)                      //////////////////�ҵ���������
						{
							if (ptr_i < my_len_01_num[0].size() - 2)           //����������
							{
								diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i] - my_len_01_num[0][ptr_i + 1] - my_decode[ptr_i + 2];
								if (abs(diff_num) == 0)
								{
									erroR += my_len_01_num[0][ptr_i + 1];
									ptr_i += 2;          //λ����λ
								}
								if (abs(diff_num) == 1)
								{
									erroR += (my_len_01_num[0][ptr_i + 1] + 1);
									ptr_i += 2;
								}
								else      //�޷����� 
								{
									cout << "����1" << endl;
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
						else if (diff_num < 0)     //�ҵ����ݶ���
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
									cout << "����3" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								cout << "����4" << endl;
								using_BER = 0;
								ptr_i = my_len_01_num[0].size();
							}
						}
					}
				}
				else   //Ϊ���һ�� 
				{
					if (Second_header == my_decode.size() - 1)  //δ�ҵ��ڶ���ͷ
					{
						if (diff_num >= 0) {}
						else { erroR += abs(diff_num); }
					}
					else
					{
						if (ptr_r != real_len_01_num[0].size() - 1)       //���ִ���
						{
							cout << "����5" << endl;
							using_BER = 0;
						}
						else  //��������    ����
						{
							if (abs(diff_num) == 1)
							{
								if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //�������� �����Ҷ�һ�� ����һ�غ� ����һ�� ���Զ���ٵ��Ǹ�Ϊ����Ԫ��
								else if (history_more == (ptr_i - 1) && diff_num == 1) {}
								else { erroR++; }                               //δ�������� 
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

		////////////////////////////////////////����Second_header���沿��
		my_len_01_num[0].clear(); my_len_01_num[1].clear();
		if (Second_header != my_decode.size() - 1 && using_BER != 0)
		{
			for (int i = Second_header; i < my_decode.size(); i++)  //ptr ָ�����my_decode��index
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
				int diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i];   //����0 ������     С��0 �Ҷ���
				if (ptr_i < my_len_01_num[0].size() - 1)               //��Ϊ���һ��
				{
					//���1               ��ȫƥ��
					if (diff_num == 0) {}
					//���2               �������1��λ   
					else if (abs(diff_num) == 1)
					{
						if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //�������� �����Ҷ�һ�� ����һ�غ� ����һ�� ���Զ���ٵ��Ǹ�Ϊ����Ԫ��
						else if (history_more == (ptr_i - 1) && diff_num == 1) {}
						else { erroR++; }                               //δ�������� 
						if (diff_num == 1)history_less = ptr_i;
						else history_more = ptr_i;
					}
					//���3               ���ֶ��λ�õ�ȱ�� ��ʱ������ 0010 �� ����� 00000֮��� �γ������ ���� 0000 ������� 01000
					else
					{
						if (diff_num > 0)                      //////////////////�ҵ���������
						{
							if (ptr_i < my_len_01_num[0].size() - 2)           //����������
							{
								diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i] - my_len_01_num[0][ptr_i + 1] - my_decode[ptr_i + 2];
								if (abs(diff_num) == 0)
								{
									erroR += my_len_01_num[0][ptr_i + 1];
									ptr_i += 2;          //λ����λ
								}
								if (abs(diff_num) == 1)
								{
									erroR += (my_len_01_num[0][ptr_i + 1] + 1);
									ptr_i += 2;
								}
								else      //�޷����� 
								{
									cout << "����6" << endl;
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
						else if (diff_num < 0)     //�ҵ����ݶ���
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
								cout << "����7" << endl;
								using_BER = 0;
								ptr_i = my_len_01_num[0].size();
							}
						}
					}
				}
				else   //Ϊ���һ�� 
				{
					if (diff_num >= 0) {}
					else { erroR += abs(diff_num); }
				}
			}
		}

		////////////////////////////////////////����First_headerǰ�沿��
		my_len_01_num[0].clear(); my_len_01_num[1].clear();
		real_len_01_num[0].clear(); real_len_01_num[1].clear();
		///////////////////////////////////////��my_decodeǰ�벿�ֵ�len��num 
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

			for (int i = First_header-1; i >= 0; i--)  //ptr ָ�����my_decode��index
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
				if (ptr_r >= real_len_01_num[0].size()) { cout << "����10" << endl; using_BER = 0; ptr_i = my_len_01_num[0].size(); break; }
				int diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i];   //����0 ������     С��0 �Ҷ���
				if (ptr_i < my_len_01_num[0].size() - 1)               //��Ϊ���һ��
				{
					//���1               ��ȫƥ��
					if (diff_num == 0) {}
					//���2               �������1��λ   
					else if (abs(diff_num) == 1)
					{
						if (history_less == (ptr_i - 1) && diff_num == -1) {}                         //�������� �����Ҷ�һ�� ����һ�غ� ����һ�� ���Զ���ٵ��Ǹ�Ϊ����Ԫ��
						else if (history_more == (ptr_i - 1) && diff_num == 1) {}
						else { erroR++; }                               //δ�������� 
						if (diff_num == 1)history_less = ptr_i;
						else history_more = ptr_i;
					}
					//���3               ���ֶ��λ�õ�ȱ�� ��ʱ������ 0010 �� ����� 00000֮��� �γ������ ���� 0000 ������� 01000
					else
					{
						if (diff_num > 0)                      //////////////////�ҵ���������
						{
							if (ptr_i < my_len_01_num[0].size() - 2)           //����������
							{
								diff_num = real_len_01_num[0][ptr_r] - my_len_01_num[0][ptr_i] - my_len_01_num[0][ptr_i + 1] - my_decode[ptr_i + 2];
								if (abs(diff_num) == 0)
								{
									erroR += my_len_01_num[0][ptr_i + 1];
									ptr_i += 2;          //λ����λ
								}
								if (abs(diff_num) == 1)
								{
									erroR += (my_len_01_num[0][ptr_i + 1] + 1);
									ptr_i += 2;
								}
								else      //�޷����� 
								{
									cout << "����11" << endl;
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
						else if (diff_num < 0)     //�ҵ����ݶ���
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
									cout << "����12" << endl;
									using_BER = 0;
									ptr_i = my_len_01_num[0].size();
								}
							}
							else
							{
								cout << "����13" << endl;
								using_BER = 0;
								ptr_i = my_len_01_num[0].size();
							}
						}
					}
				}
				else   //Ϊ���һ�� 
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
		cout << "�����޷��������ݣ����ڼ��±����ֶ��鿴" << endl;
		ofstream open_txt("error_data.txt", ios::app);
		for (int i = 0; i < my_decode.size(); i++) open_txt << my_decode[i];
		open_txt << endl;
		open_txt.close();
	}
}

/*
int main()                         //�����ݰ�   10֡ͶƱ
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
		resize(src, src, Size(1080, 960), (0, 0), (0, 0), INTER_LINEAR);                              //��linear �� nearest ��ʵ��𲻴�
		//cvtColor(src, src, COLOR_BGR2GRAY);
		//debubble(src);
		//oneline = src(Rect((src.cols - 1), 0, 1, src.rows));
		split(src, split_src);               //RGB����
		debubble(split_src[channel]); //RGB����
		oneline = split_src[channel](Rect((src.cols - 1), 0, 1, src.rows));//RGB����
		oneline = oneline.t();
		my_eva.Sum_EVA(oneline);
		eyes_compare();
	}
	vote_data();
	for(int i = 0;i<5;i++) cout << endl;
	cout << "���ս��Ϊ" << endl;
	eyes_compare();
	return 0;
}

*/

int main()                  //��BER
{
	EVA my_eva;
	Msrcr my_MSR;
	Mat src, oneline, split_src[3];
	Mat src1;
	int header_box[3];              //��һλΪFirst_header �ڶ�λΪSecond_header ����λΪ�Ƿ��ҵ�header
	VideoCapture cap;
	//cap.open("stack.mp4");
	cap.open("new.mp4");
	ofstream open_txt("error_data.txt", ios::trunc);                  //����ļ�
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
		//split(src, split_src);               //RGB����
		//cv::sort(split_src[channel], split_src[channel], SORT_EVERY_ROW + SORT_ASCENDING);
		//oneline = split_src[channel](Rect((src.cols - 2), 0, 1, src.rows));
		oneline = oneline.t();
		my_eva.Sum_EVA(oneline, header_box, 0);
		eyes_compare();
		count_error_rate(header_box[0], header_box[1], header_box[2]);
	}
	cout << "BERΪ" << error_num / sum_num << endl;
	return 0;
}



