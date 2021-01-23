#include"defin.h"

int local_max = 255;                   //temp_value
int local_min = 0;
int tempp1, tempp2;			//temp_place
vector<Point2i> maxpv;         //  place and value 
vector<Point2i> minpv;;
vector<int> decode;              //用于观察解码结果  00000000000000000000000000011111这样子的结果 还没有整合过多少个0，1为一个bit
vector<int> my_decode;         //用于存放我的解码结果    001100101 这样子的结果已经得出了最终的结果
vector<int> D_with_header;      //
vector<int> num_of_lianxv[2];          //记录连续的0，1有多少个，便于选择多少个0代表一个bit        [0]保存为0的个数，或者1的个数  [1]保存为是0 还是 1
vector<int>inter_fix[10];        
int temp;                                 //用于存放阈值
int length[50] = {};                //找像素与bit的关系
float pxlength = 0;                    //找像素与bit的关系最有概率的值
float noise = 0;                      //正常噪声波动值  将来进行判断  
int frequt[256] = {};             //找最大频率的数代表0，1的grayscalar
float thre = 0;                       //  0,1代表位的中间阈值
int sca;                                 //图片变化比率 
//int header[] = { 1,0,1,0,1,0,1,0,1,0 };            //保存头部信息       如果为规律的结构，比如 0，1，0，1就不该使用此种结构
int header_length = 10;                                     //保存头部信息        规律结构使用这种
int inter_num = 0;
float error_num = 0;
long float sum_num = 0;
