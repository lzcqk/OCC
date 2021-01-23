#include "Msrcr.h"


/********************************************************************************

���߶�Retinexͼ����ǿ����

srcΪ������ͼ��

sigmaΪ��˹ģ����׼��

*********************************************************************************/

void Msrcr::SingleScaleRetinex(const cv::Mat& src, cv::Mat& dst, int sigma)
{

	cv::Mat doubleI, gaussianI, logI, logGI, logR;



	src.convertTo(doubleI, CV_64FC3, 1.0, 1.0);                    //ת����Χ������ͼ��Ԫ������1.0��֤cvlog����

	cv::GaussianBlur(doubleI, gaussianI, cv::Size(0, 0), sigma);    //SSR�㷨�ĺ���֮һ����˹ģ������sizeΪ��ʱ��ͨ��sigma�Զ����м���

	cv::log(doubleI, logI);

	cv::log(gaussianI, logGI);

	logR = logI - logGI;                                           //Retinex��ʽ��Log(R(x,y))=Log(I(x,y))-Log(Gauss(I(x,y)))

	cv::normalize(logR, dst, 0, 255, cv::NORM_MINMAX, CV_8UC3);     //SSR�㷨�ĺ���֮��,�������� (�ƺ���������ʱ��û��˭�Ὣ Log[R(x,y)]����Exp�����������ֱ�ӵõ�R(x,y))

}

/********************************************************************************

��߶�Retinexͼ����ǿ����

srcΪ������ͼ��

kΪ�߶Ȳ���

wΪȨ�ز���

sigmaΪ��˹ģ����׼��

*********************************************************************************/
Mat Msrcr::MultiScaleRetinex(const cv::Mat& src, double w[], double sigmas[])
{
	cv::Mat doubleI, logI,dst;

	cv::Mat logR = cv::Mat::zeros(src.size(), CV_64FC3);

	src.convertTo(doubleI, CV_64FC3, 1.0, 1.0);                    //ת����Χ������ͼ��Ԫ������1.0��֤cvlog����

	cv::log(doubleI, logI);

	for (int i = 0; i < 3 ;i++)
	{//Retinex��ʽ��Log(R(x,y)) += w_k(Log(I(x,y))-Log(Gauss_k(I(x,y))))
		cv::Mat tempGI;

		cv::GaussianBlur(doubleI, tempGI, cv::Size(0, 0), sigmas[i]);

		cv::Mat templogGI;

		cv::log(tempGI, templogGI);

		logR += w[i] * (logI - templogGI);

	}
	cv::normalize(logR, dst, 0, 255, cv::NORM_MINMAX, CV_8UC3);  //SSR�㷨�ĺ���֮��,�������� (�ƺ���������ʱ��û��˭�Ὣ Log[R(x,y)]����Exp�����������ֱ�ӵõ�R(x,y))
	return dst;
}

/********************************************************************************

��߶�Retinexͼ����ǿ����

src     Ϊ������ͼ��

k       Ϊ�߶Ȳ���

w       ΪȨ�ز���

sigma   Ϊ��˹ģ����׼��

alpha   ���泣��

beta    �ܿصķ�����ǿ��

gain    ����

offset  ƫ��

*********************************************************************************/

Mat Msrcr::MultiScaleRetinexCR(const cv::Mat& src,double w[], double sigmas[], int alpha, int beta)
{
	cv::Mat doubleIl, logI,dst;
	cv::Mat logMSR = cv::Mat::zeros(src.size(), CV_64FC3);
	src.convertTo(doubleIl, CV_64FC3, 1.0, 1.0);                    //ת����Χ������ͼ��Ԫ������1.0��֤cvlog����
	cv::log(doubleIl, logI);
	for (int i = 0; i < 3; i++)

	{//Retinex��ʽ��Log(R(x,y)) += w_k(Log(I(x,y))-Log(Gauss_k(I(x,y))))
		cv::Mat tempGI;

		cv::GaussianBlur(doubleIl, tempGI, cv::Size(0, 0), sigmas[i]);

		cv::Mat templogGI;

		cv::log(tempGI, templogGI);

		logMSR += w[i] * (logI - templogGI);

	}

	std::vector<cv::Mat> logMSRc(3);

	cv::split(logMSR, logMSRc);
	cv::Mat doubleI;

	src.convertTo(doubleI, CV_64FC3);
	std::vector<cv::Mat> doubleIc(3);
	cv::split(doubleI, doubleIc);
	cv::Mat sumDoubleIc = cv::Mat::zeros(doubleI.size(), CV_64FC1);

	for (int i = 0; i < doubleI.rows; i++)
	{
		for (int j = 0; j < doubleI.cols; j++)
		{
			sumDoubleIc.ptr<double>(i)[j] = doubleI.ptr<cv::Vec3d>(i)[j][0] + doubleI.ptr<cv::Vec3d>(i)[j][1] + doubleI.ptr<cv::Vec3d>(i)[j][2];
		}
	}
	std::vector<cv::Mat> divideDoubleIc(3);
	std::vector<cv::Mat> Cc(3);
	std::vector<cv::Mat> MSRCRc(3);
	cv::Mat tempResult;
	for (int i = 0; i < 3; i++)
	{
		cv::divide(doubleIc[i], sumDoubleIc, divideDoubleIc[i]);
		divideDoubleIc[i].convertTo(divideDoubleIc[i], CV_64FC1, 1.0, 1.0);
		cv::log(alpha * divideDoubleIc[i], Cc[i]);
		Cc[i] *= beta;
		MSRCRc[i] = Cc[i].mul(logMSRc[i]);
	}
	cv::merge(MSRCRc, tempResult);

	cv::normalize(tempResult, dst, 0, 255, cv::NORM_MINMAX, CV_8UC3);
	return dst;
}