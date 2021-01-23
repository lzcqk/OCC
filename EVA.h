#pragma once
#ifndef _EVA_H_
#define _EVA_H_

#include<opencv2/opencv.hpp>
#include<iostream>
#include<fstream>
#include"defin.h"

using namespace cv;
using namespace std;

class EVA
{
private:
	void Find_local(Mat BGRchannels);
	void Find_frequt();
	void Find_usefullocal(Mat BGRchannels);
	void Input_csv(Mat BGRchannels, int flag);
	void Find_length(int flag);
	void Find_real_data();
	void Find_D_with_header(int flag,int header_box[]);
public:
	void initial_vector();    
	void Sum_EVA(Mat BGRchannels,int header_box[],int flag = 1);            //flag用于指定是否整合？ 1 为整合   0 为不整合 求BER
};

#endif // !_EVA_H_
