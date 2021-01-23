#pragma once
#ifndef _DEFIN_H_
#define _DEFIN_H_
#include<opencv2/opencv.hpp>
#include<iostream>

using namespace cv;
using namespace std;

extern int local_max;                   //temp_value
extern int local_min;
extern int tempp1, tempp2;			//temp_place
extern vector<Point2i> maxpv;         //  place and value 
extern vector<Point2i> minpv;;
extern vector<int> decode;              //用于观察解码结果  00000000000000000000000000011111这样子的结果 还没有整合过多少个0，1为一个bit
extern vector<int> my_decode;         //用于存放我的解码结果    001100101 这样子的结果已经得出了最终的结果
extern vector<int> D_with_header;      //
extern vector<int> num_of_lianxv[2];          //记录连续的0，1有多少个，便于选择多少个0代表一个bit        [0]保存为0的个数，或者1的个数  [1]保存为是0 还是 1
extern vector<int> inter_fix[10];            //用于存放每次解码(without header) 的结果
extern int temp;                                 //用于存放阈值
extern int length[50] ;                //找像素与bit的关系
extern float pxlength;                  //找像素与bit的关系最有概率的值
extern float noise ;                      //正常噪声波动值  将来进行判断  
extern int frequt[256] ;             //找最大频率的数代表0，1的grayscalar
extern float thre;                       //  0,1代表位的中间阈值
extern int sca;                                 //图片变化比率 
//int header[] = { 1,0,1,0,1,0,1,0,1,0 };            //保存头部信息       如果为规律的结构，比如 0，1，0，1就不该使用此种结构
extern int header_length ;                                     //保存头部信息        规律结构使用这种
extern int inter_num; 
extern float error_num;
extern long float sum_num;

#endif // !_DEFIN_H_
