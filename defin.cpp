#include"defin.h"

int local_max = 255;                   //temp_value
int local_min = 0;
int tempp1, tempp2;			//temp_place
vector<Point2i> maxpv;         //  place and value 
vector<Point2i> minpv;;
vector<int> decode;              //���ڹ۲������  00000000000000000000000000011111�����ӵĽ�� ��û�����Ϲ����ٸ�0��1Ϊһ��bit
vector<int> my_decode;         //���ڴ���ҵĽ�����    001100101 �����ӵĽ���Ѿ��ó������յĽ��
vector<int> D_with_header;      //
vector<int> num_of_lianxv[2];          //��¼������0��1�ж��ٸ�������ѡ����ٸ�0����һ��bit        [0]����Ϊ0�ĸ���������1�ĸ���  [1]����Ϊ��0 ���� 1
vector<int>inter_fix[10];        
int temp;                                 //���ڴ����ֵ
int length[50] = {};                //��������bit�Ĺ�ϵ
float pxlength = 0;                    //��������bit�Ĺ�ϵ���и��ʵ�ֵ
float noise = 0;                      //������������ֵ  ���������ж�  
int frequt[256] = {};             //�����Ƶ�ʵ�������0��1��grayscalar
float thre = 0;                       //  0,1����λ���м���ֵ
int sca;                                 //ͼƬ�仯���� 
//int header[] = { 1,0,1,0,1,0,1,0,1,0 };            //����ͷ����Ϣ       ���Ϊ���ɵĽṹ������ 0��1��0��1�Ͳ���ʹ�ô��ֽṹ
int header_length = 10;                                     //����ͷ����Ϣ        ���ɽṹʹ������
int inter_num = 0;
float error_num = 0;
long float sum_num = 0;
