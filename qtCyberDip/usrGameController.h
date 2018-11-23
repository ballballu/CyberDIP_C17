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
////游戏控制类
//class usrGameController
//{
//private:
//	deviceCyberDip* device;
//public:
//	//构造函数，所有变量的初始化都应在此完成
//	usrGameController(void* qtCD);
//	//析构函数，回收本类所有资源
//	~usrGameController();
//	//处理图像函数，每次收到图像时都会调用
//	int usrProcessImage(cv::Mat& img);
//
//	// modified by zcc
//	void ThunderMove(cv::Point, cv::Point);
//
//	//传入现在手机图片img，原图srcImg, 图块编号numpic，总的循环次数
//	//返回难度nandu，且如果nandu=0（图块很好）调用机械臂移动的函数;
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
////以下是为了实现演示效果，增加的内容
////鼠标回调函数
//void  mouseCallback(int event, int x, int y, int flags, void*param);
////以上是为了实现课堂演示效果，增加的内容
//
//#endif
//#endif

#ifdef VIA_OPENCV

#ifndef USRGAMECONTROLLER_H
#define USRGAMECONTROLLER_H

#include "qtcyberdip.h"
#include <cmath>

#define WIN_NAME "Frame"

//游戏控制类
class usrGameController
{
private:
	deviceCyberDip* device;
public:
	//构造函数，所有变量的初始化都应在此完成
	usrGameController(void* qtCD);
	//析构函数，回收本类所有资源
	~usrGameController();
	//处理图像函数，每次收到图像时都会调用
	int usrProcessImage(cv::Mat& img);


	// ended by zcc
};


#endif
#endif