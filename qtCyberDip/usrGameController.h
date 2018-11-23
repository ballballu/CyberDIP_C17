//#ifdef VIA_OPENCV
//
//#ifndef USRGAMECONTROLLER_H
//#define USRGAMECONTROLLER_H
//
//#include "qtcyberdip.h"
//#include <cmath>
//
//#define WIN_NAME "Frame"
//
////��Ϸ������
//class usrGameController
//{
//private:
//	deviceCyberDip* device;
//public:
//	//���캯�������б����ĳ�ʼ����Ӧ�ڴ����
//	usrGameController(void* qtCD);
//	//�������������ձ���������Դ
//	~usrGameController();
//	//����ͼ������ÿ���յ�ͼ��ʱ�������
//	int usrProcessImage(cv::Mat& img);
//
//	// modified by zcc
//	void ThunderMove(cv::Point, cv::Point);
//
//	//���������ֻ�ͼƬimg��ԭͼsrcImg, ͼ����numpic���ܵ�ѭ������
//	//�����Ѷ�nandu�������nandu=0��ͼ��ܺã����û�е���ƶ��ĺ���;
//	int EagleEye(cv::Mat&, cv::Mat, int, int);
//
//	// before superEagleEye, after EagleEye
//	int UltiEagleEye(cv::Mat&, cv::Mat, int, int);
//
//	// before finalEagleEye, after UltiEagleEye
//	int superEagleEye(cv::Mat&, cv::Mat, int);
//	
//	// the last for use
//	int finalEagleEye(cv::Mat&, cv::Mat, int);
//
//	// ended by zcc
//};
//
////������Ϊ��ʵ����ʾЧ�������ӵ�����
////���ص�����
//void  mouseCallback(int event, int x, int y, int flags, void*param);
////������Ϊ��ʵ�ֿ�����ʾЧ�������ӵ�����
//
//#endif
//#endif

#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

#include "qtcyberdip.h"
#include <cmath>

#define WIN_NAME "Frame"

//��Ϸ������
class usrGameController
{
private:
	deviceCyberDip* device;
public:
	//���캯�������б����ĳ�ʼ����Ӧ�ڴ����
	usrGameController(void* qtCD);
	//�������������ձ���������Դ
	~usrGameController();
	//����ͼ������ÿ���յ�ͼ��ʱ�������
	int usrProcessImage(cv::Mat& img);


	// ended by zcc
};


#endif
#endif