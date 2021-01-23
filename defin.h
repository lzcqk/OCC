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
extern vector<int> decode;              //���ڹ۲������  00000000000000000000000000011111�����ӵĽ�� ��û�����Ϲ����ٸ�0��1Ϊһ��bit
extern vector<int> my_decode;         //���ڴ���ҵĽ�����    001100101 �����ӵĽ���Ѿ��ó������յĽ��
extern vector<int> D_with_header;      //
extern vector<int> num_of_lianxv[2];          //��¼������0��1�ж��ٸ�������ѡ����ٸ�0����һ��bit        [0]����Ϊ0�ĸ���������1�ĸ���  [1]����Ϊ��0 ���� 1
extern vector<int> inter_fix[10];            //���ڴ��ÿ�ν���(without header) �Ľ��
extern int temp;                                 //���ڴ����ֵ
extern int length[50] ;                //��������bit�Ĺ�ϵ
extern float pxlength;                  //��������bit�Ĺ�ϵ���и��ʵ�ֵ
extern float noise ;                      //������������ֵ  ���������ж�  
extern int frequt[256] ;             //�����Ƶ�ʵ�������0��1��grayscalar
extern float thre;                       //  0,1����λ���м���ֵ
extern int sca;                                 //ͼƬ�仯���� 
//int header[] = { 1,0,1,0,1,0,1,0,1,0 };            //����ͷ����Ϣ       ���Ϊ���ɵĽṹ������ 0��1��0��1�Ͳ���ʹ�ô��ֽṹ
extern int header_length ;                                     //����ͷ����Ϣ        ���ɽṹʹ������
extern int inter_num; 
extern float error_num;
extern long float sum_num;

#endif // !_DEFIN_H_
