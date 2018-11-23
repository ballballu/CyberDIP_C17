#include "usrGameController.h"
#include "cv.h"
#include "highgui.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <Windows.h>
using namespace cv;
using namespace std;

usrGameController::usrGameController(void* qtCD)
{
	qDebug() << "usrGameController online.";
	device = new deviceCyberDip(qtCD);//设备代理类
}
usrGameController::~usrGameController()
{
	cv::destroyAllWindows();
	if (device != nullptr)
	{
		delete device;
	}
	qDebug() << "usrGameController offline.";
}

//adjustment for elastic pointerhead
#define OFFSET 10
//determine whether to adjust
bool isToAdjust = 0;
//adjustment for elastic pointerhead
#define ADJUST 20
//small delay for adjustment
#define SMALL_STEP_DELAY 200
//delay time of every pointer movement
#define STEP_DELAY 1500
//the minimum distance for finalEagLeEye
double min_distance = 1;
double error_distance_max = 2;

//---------------- hyperparameters ----------------//
//Adaptation-oneplus A5000     363*744
int delay = 1500;
int imagelen = 255;		//完整的拼图大小，也恰好等于拼图中间区域的大小
//实际拼图中心区域的左上角坐标，在直接截图的情况下
Point validregion = Point(55, 247);
//用作模板的照片（事先保存好的照片）的实际拼图区域
Point validoldregion = Point(55, 343);
//area of interest when game_step = 1 (x, y, width, height)
#define JIGTY_AREA 140, 71, 87, 64
//coordinates of the free jigsaw pack with 70% OFF (that sucks)
Point Map_sucks = Point(101, 406);
//coordinates of the free jigsaw pack without 70% OFF
Point Map = Point(264, 232);
//center of the screen (for jigsaw maps numbered 7 and 8)
int center_x = 180;
int center_y = 650;
//distance of the screen flip (for jigsaw maps numbered 7 and 8)
int screen_flip = 400;
//coordinates of the jigsaw maps in the free pack
Point MapPoint[8] = {
	Point(100, 250),
	Point(270, 250),
	Point(100, 405),
	Point(270, 405),
	Point(100, 560),
	Point(270, 560),
	Point(100, 610),
	Point(270, 610) };
//coordinate x of the scale bar (2 to 12)
int order[11] = { 51,77,104,130,156,182,208,234,260,286,312 };
//coordinate y of the scale bar
int scale_y = 491;
//coordinates of the 'start game' button
Point startgame = Point(185, 580);


//----------------- about pregame -----------------//
//1: at the first page of the game
//2: after finishing selecting game map
//3: select game scale
//4: game begins
int game_step = 1;
//sequence number of the jigsaw
int NumOfMap = -1;
//拼图阶次
int N = 0;
//check whether the game is ready to go
bool isReady = 0;



//----------------- about image -------------------//
//原始图像
Mat srcImg;
//模板图像
Mat templImg;
//匹配结果图像
Mat resultImg;



//------------- about jigsaw pieces ---------------//
//拼图图块数量
int pinumber;
//创建一个数组,数组元素的数量为拼图图块数量,记录每一个拼图图现在的难度星级
static vector<int> eva(1);
//模板的大小（选择正方形区域，边长与拼图阶次和识别率有关）
//识别率是一个放大调节的系数，因为拼图图块边缘凹凸不平，有可能需要多次调整模板大小去扫描
int templen;
//图块编号，给每一个图块编号，按照先从上到下，再从左到右的顺序编号，左上角最小(0)，右下角最大(N*N-1)
//numpic 从0开始 到N*N-1结束
int numpic = 0;



//----------- about image processing --------------//
//用来评价 现在检测完的图块难度 为0说明图块很好,可以被移动
//为1说明图块很刁钻,数值越大说明越刁钻
int nandu;
//匹配率 也就是模板边长与理想拼图图块（没有凹凸，都是一模一样的正方形）边长的比值
double pipeilv = 0.9;
//i是横向的 决定loc.x  也就是图块的列数 图块在第几列
//j是纵向的 决定loc.y
int i = 0, j = 0;
//传给机器的 orloc是待移动图块中心点的坐标，actloc是待移动图块应该移动到的坐标
Point orloc, actloc;
//遍历次数，前三次都用鹰眼，三次之后可以认为没有图块互相遮挡的情况了，如果还有图块剩余，第四次开始用超级鹰眼！
int circletime = 0;
//offset for elastic pointerhead
int offset_x, offset_y, offset;
//the round for checking the same piece
int round_check = 0;
//the round of displaying process
int display_round = 0;



//----------- function declarations ---------------//
//匹配函数  第一个传入的是原始图像 第二个是模板图像
Point MatchingMethod(Mat, Mat);
//template matching function, returning match rate
double templatematch_gamestart(cv::Mat, cv::Mat);
//constraint function, avoid crashing
int constraint(int x, int border_min, int border_max);



//处理图像 
int usrGameController::usrProcessImage(cv::Mat& img)      //img就是读到的完整图像 大小是306*641左右
{
	cv::Size imgSize(img.cols, img.rows - UP_CUT);
	if (imgSize.height <= 0 || imgSize.width <= 0)
	{
		qDebug() << "[!] Invalid image. Size:" << imgSize.width <<"x"<<imgSize.height;
		return -1;
	}

	Mat holdimg2;
	img.copyTo(holdimg2);

	//determine: the state of the game
	//1: at the first page of the game
	//2: after finishing selecting game map
	//3: select game scale
	//4: game begins
	if (game_step == 1)
	{
		//set area of interest
		cv::Mat game_step1_tmp = holdimg2(cv::Rect(JIGTY_AREA));
		imwrite("game_step1_target.png",game_step1_tmp);
		cv::Mat game_step1_target = imread("game_step1_target.png");
		cv::Mat game_step1_template = imread("game_step1_template.png");

		double game_step1_value = templatematch_gamestart(game_step1_target, game_step1_template);
		//cout << game_step1_value << endl;
		if (game_step1_value < 0.06)
		{
			device->comMoveTo((Map.x - LEFT_CUT)*RANGE_X / WIDTH, (Map.y - UP_CUT)*RANGE_Y / HEIGHT);
			device->comHitOnce();
			cout << "[*] template matching complete!" << endl;
			Sleep(delay);
			game_step = 2;
			return 0;
		}
		else
			return 0;
	}
	if (game_step == 2)
	{
		if (NumOfMap < 1 || NumOfMap > 8)
		{
			cout << "[?] 请输入拼图序号： ";
			cin >> NumOfMap;
			return 0;
		}
		else
		{
			string map = "source_image_" + to_string(NumOfMap) + ".png";
			srcImg = imread(map, 1);

			if (NumOfMap <= 6)
			{
				device->comMoveTo((MapPoint[NumOfMap - 1].x - LEFT_CUT)*RANGE_X / WIDTH, (MapPoint[NumOfMap - 1].y - UP_CUT)*RANGE_Y / HEIGHT);
				device->comHitOnce();
				Sleep(delay);
				game_step = 3;
			}
			else if (NumOfMap >= 7)
			{
				device->comMoveTo((center_x - LEFT_CUT)*RANGE_X / WIDTH, (center_y - UP_CUT)*RANGE_Y / HEIGHT);
				device->comHitDown();
				device->comMoveTo((center_x - LEFT_CUT)*RANGE_X / WIDTH, (center_y - screen_flip - UP_CUT)*RANGE_Y / HEIGHT);
				device->comHitUp();
				device->comMoveTo((MapPoint[NumOfMap - 1].x - LEFT_CUT)*RANGE_X / WIDTH, (MapPoint[NumOfMap - 1].y - UP_CUT)*RANGE_Y / HEIGHT);
				device->comHitOnce();
				Sleep(delay);
				game_step = 3;
			}
			cout << "[*] selecting map completed!" << endl;
			return 0;
		}
	}
	if (game_step == 3)
	{
		cout << "[?] 请输入拼图阶次： ";
		cin >> N;

		pinumber = N*N;
		//resize the vector
		eva.resize(pinumber);

		device->comMoveTo((order[N - 2] - LEFT_CUT) * RANGE_X / WIDTH, (scale_y - UP_CUT)*RANGE_Y / HEIGHT);
		device->comHitOnce();
		device->comMoveTo((startgame.x - LEFT_CUT)*RANGE_X / WIDTH, (startgame.y - UP_CUT)*RANGE_Y / HEIGHT);
		device->comHitOnce();
		cout << "[*] selecting map scale completed!" << endl;
		Sleep(delay);
		game_step = 4;
		return 0;
	}
	if (game_step == 4)
	{
		if (!isReady)
		{
			cout << "[*] game starts!" << endl;
			cout << endl;
		}

		isReady = 1;
	}

	if (isReady)
	{

		if (circletime == 0)            //第一次循环全部都要遍历的
		{
			if (numpic == 0)
				cout << "[*] EagleEye: lap" << circletime << endl;

			eva[numpic] = EagleEye(img, srcImg, numpic, circletime);
			if (numpic < pinumber - 1)  //因为numpic是从0开始的，应该到等于pinumber-1结束，且现在if语句写在主程序尾，所以是小于pinnumber-1，因为numpic=pinumber-2后还会再加1，执行一次主程序
			{
				numpic = numpic + 1;    //图块编号加1 对下一个图块进行处理
				//cout << "[" << circletime << "-1] 处理" << numpic << endl;
			}
			else
			{
				//cout << "[" << circletime << "-1] 处理" << numpic + 1 << endl;
				cout << "[*] EagleEye: lap" << circletime<<" completed!" << endl;
				cout << endl;
				circletime = circletime + 1;
				numpic = 0;
			}
		}
		else if (circletime < 4)        //这样两次鹰眼（算第一次遍历的）之后就用调整匹配率
		{
			if(numpic == 0)
				cout << "[*] EagleEye: lap" << circletime << endl;

			if ((numpic < pinumber) && (eva[numpic] == 0))				//先检查图块对应的难度，若为0就可以跳过这一块了
			{
				cout << "[" << circletime << "-0] 跳过" << numpic + 1 << endl;
				numpic = numpic + 1;
			}
			else if ((numpic < pinumber) && (eva[numpic] == 1))
			{
				//cout << "[" << circletime << "-1] 处理" << numpic + 1 << endl;
				eva[numpic] = EagleEye(img, srcImg, numpic, circletime);
				numpic = numpic + 1;
			}
			else
			{
				cout << "[*] EagleEye: lap" << circletime << " completed!" << endl;
				cout << endl;
				circletime = circletime + 1;
				numpic = 0;
			}
		}
		else if (circletime < 6)        //这样四次鹰眼（算第一次遍历的）之后就用ulti鹰眼  
		{
			if (numpic == 0)
				cout << "[*] UltiEagleEye: lap" << circletime - 3 << endl;

			if ((numpic < pinumber) && (eva[numpic] == 0))
			{
				cout << "[" << circletime << "-0] 跳过" << numpic + 1 << endl;
				numpic = numpic + 1;
			}
			else if ((numpic < pinumber) && (eva[numpic] == 1))
			{
				//cout << "[" << circletime << "-1] 处理" << numpic + 1 << endl;
				eva[numpic] = UltiEagleEye(img, srcImg, numpic, circletime);
				numpic = numpic + 1;
			}
			else
			{
				cout << "[*] UltiEagleEye: lap" << circletime - 3 << " completed!" << endl;
				cout << endl;
				circletime = circletime + 1;
				numpic = 0;
			}
		}
		else
		{
			if (numpic == 0 && display_round == 0)
			{
				cout << "[*] finalEagleEye: lap" << circletime - 5 << endl;
				display_round = 1;
			}

			if (numpic < pinumber)
			{
				numpic = finalEagleEye(img, srcImg, numpic);
			}
			else
			{
				cout << "[*] finalEagleEye: lap" << circletime - 5 << " completed!" << endl;
				cout << endl;
				circletime = circletime + 1;
				display_round = 0;
				numpic = 0;
			}
		}
	}

	return 0;
}



//----------------- function definitions ------------------//
//模板匹配函数
//2017.12.12在找到了最佳匹配点后（其位置为matchLoc），把匹配点位置转换为拼图中区域点坐标，并在pt上显示
Point MatchingMethod(Mat srcImgb, Mat templImgb)
{
	// 深拷贝用于显示
	Mat displayImg;
	srcImgb.copyTo(displayImg);

	// 创建匹配结果图像，为每个模板位置存储匹配结果
	// 匹配结果图像大小为：(W-w+1)*(H-h+1)
	int result_cols = srcImgb.cols - templImgb.cols + 1;
	int result_rows = srcImgb.rows - templImgb.rows + 1;
	resultImg.create(result_cols, result_rows, CV_32FC1);

	// 进行匹配并归一化
	// 对于CV_TM_SQDIFF和 CV_TM_SQDIFF_NORMED这两种方法，最小值为最佳匹配；对于别的方法最大值为最佳匹配
	matchTemplate(srcImgb, templImgb, resultImg, CV_TM_SQDIFF_NORMED);
	normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());

	// 使用minMaxLoc找出最佳匹配
	double minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;
	minMaxLoc(resultImg, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

	matchLoc = minLoc;
	return matchLoc;
}
//template matching function used before game;
double templatematch_gamestart(cv::Mat targetImage, cv::Mat templateImage)
{
	int result_cols = targetImage.cols - templateImage.cols + 1;
	int result_rows = targetImage.rows - templateImage.rows + 1;

	cv::Mat result = cv::Mat(result_cols, result_rows, CV_32FC1);
	cv::matchTemplate(targetImage, templateImage, result, CV_TM_SQDIFF_NORMED);
	double minVal = 0;
	double maxVal = 0;
	cv::Point minLoc, maxLoc, matchLoc;
	cv::minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
	//cout << "minval=" << minVal << endl;
	//cv::rectangle(targetImage, cv::Rect(matchLoc, cv::Size(templateImage.cols, templateImage.rows)), Scalar(0, 0, 255), 2, 8, 0);
	return minVal;
}
int usrGameController::EagleEye(Mat& img,Mat srcImg,int numpic,int circletime)
{
	//深拷贝
	Mat holdimg;
	img.copyTo(holdimg);
	imwrite("check.png", holdimg);
	Mat check = imread("check.png", 1);

	//loc为srcImg上截图的部分的左上角坐标，ptloc为holdimg里匹配到的部分的左上角的坐标,
	Point loc, ptloc;
	//cenlocx/y为匹配到的部分的中心点的坐标 采用数组 每一次匹配率后都会得到一个坐标
	double cenlocx[4];
	double cenlocy[4];
	//x y和的均值，各自的方差和方差之和
	//error（在不同匹配率下location的稳定程度（方差））——数组元素值越大说明这个图块越不好，位置越刁钻)
	double meanx, meany, errorx, errory, error;

	i = numpic / N;    //图块所在的列数 从0开始到N-1列
	j = numpic % N;    //图块所在的行数 从0开始到N-1行

	nandu = 1;

	for (int z = 0; z < 4; z++)  //不同匹配率 z<9时相当于匹配率从0.9到0.1（9次）
	{
		if (circletime < 2)
		{
			pipeilv = 0.8 - z / 5.0;    //0.8 0.7 0.6 0.5
		}
		else if(circletime<3)
		{
			pipeilv = 0.6 - z / 10.0;    //0.6 0.5 0.4 0.3
		}
		else
		{
			pipeilv = 0.4 - z / 10.0;     //0.4 0.3 0.2
		}
		//pipeilv = 0.8 - z / 5.0;
		
		//计算模板大小,cvFloor向下取整  (N=3时模板大小为43 N=4是模板大小为32)
		templen = cvFloor((imagelen * pipeilv) / N);
		loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
		loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

		//模板是从srcImg上截图 遍历srcImg
		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
		imwrite("moban.png", mobanpre);
		Mat moban = imread("moban.png", 1);

		//获得模板在check(就是holdimg)上最佳匹配点的位置坐标
		ptloc = MatchingMethod(check, moban);

		cenlocx[z] = ptloc.x + templen / 2;
		cenlocy[z] = ptloc.y + templen / 2;
	}

	//ij图块在不同匹配率下ptloc.x的平均值
	meanx = (cenlocx[0] + cenlocx[1] + cenlocx[2] + cenlocx[3]) / 4.0;
	meany = (cenlocy[0] + cenlocy[1] + cenlocy[2] + cenlocy[3]) / 4.0;

	errorx = (cenlocx[0] * cenlocx[0] + cenlocx[1] * cenlocx[1] + cenlocx[2] * cenlocx[2] + cenlocx[3] * cenlocx[3]) / 4.0 - meanx*meanx;  //求方差
	errory = (cenlocy[0] * cenlocy[0] + cenlocy[1] * cenlocy[1] + cenlocy[2] * cenlocy[2] + cenlocy[3] * cenlocy[3]) / 4.0 - meany*meany;  //求方差
	error = errorx + errory;

	actloc.x = cvFloor(((i + 0.5) / N)*imagelen) + validoldregion.x;
	actloc.y = cvFloor(((j + 0.5) / N)*imagelen) + validregion.y;

	//预警模块 要移动的拼图图块不应该在正中间的区域，所以若在，直接error置为100
	if ((meanx > validregion.x) && (meanx < imagelen + validregion.x)&& (meany > validregion.y) && (meany < imagelen + validregion.y))
		error = 100;

	if (error < 5.0)
	{
		nandu = 0;
		orloc.x = cvFloor(meanx);
		orloc.y = cvFloor(meany);
		//画图，显示orloc和actloc的位置，并连线
		if(circletime<2)
		{
			//circle(check, orloc, 4, 1, 2);
			//circle(check, actloc, 4, 1, 2);
			//line(check, orloc, actloc, 1, 2);
		}	
		else 
		{
			//circle(check, orloc, 2, Scalar(0, 0, 255), 2);
			//circle(check, actloc, 2, Scalar(0, 0, 255), 2);
			//line(check, orloc, actloc, Scalar(0, 0, 255), 2);
		}

		ThunderMove(orloc, actloc);
	}
	else
	{
		orloc.x = cvFloor(meanx);
		orloc.y = cvFloor(meany);
		//circle(check, orloc, 4, 1, 2);
		//circle(check, actloc, 4, 1, 2);
	}
	//显示处理信息模块
	//cout << "图块编号" << numpic << endl;
	//cout << "图块位置 " << j << ' ' << i << endl;
	//cout << "图块误差" << error << endl;
	//cout << "图块难度 "<< nandu << endl << endl;

	//imshow("匹配图", check);
	//rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen),Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分(这里面是匹配率最小的，也就是pipeilv=0.2时的框大小)
	//imshow("模板图", srcImg);

	//cvWaitKey(0);//等效于机器完成移动，传递会一个信息 这之前程序等待
	return nandu;
}
int usrGameController::UltiEagleEye(cv::Mat& img, Mat srcImg, int numpic ,int circletime)
{
	//深拷贝
	Mat holdimg;
	img.copyTo(holdimg);
	imwrite("check.png", holdimg);
	Mat check = imread("check.png", 1);

	//loc为srcImg上截图的部分的左上角坐标，ptloc为holdimg里匹配到的部分的左上角的坐标,
	Point loc, ptloc;
	//cenlocx/y为匹配到的部分的中心点的坐标 采用数组 每一次匹配率后都会得到一个坐标
	double cenlocx[3];
	double cenlocy[3];
	//x y和的均值，各自的方差和方差之和
	//error（在不同匹配率下location的稳定程度（方差））——数组元素值越大说明这个图块越不好，位置越刁钻)
	double error;

	i = numpic / N;    //图块所在的列数 从0开始到N-1列
	j = numpic % N;    //图块所在的行数 从0开始到N-1行

	nandu = 1;

	for (int z = 0; z < 3; z++)  //三次匹配率 从0.4到0.1——最新升级，升级为从0.6-0.4
	{
		pipeilv = 0.4 - z * 0.1;   //0.4 0.3 0.2
		
		//计算模板大小,cvFloor向下取整  (N=3时模板大小为43 N=4是模板大小为32)
		templen = cvFloor((imagelen * pipeilv) / N);
		loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
		loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

		//模板是从srcImg上截图 遍历srcImg
		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
		imwrite("moban.png", mobanpre);
		Mat moban = imread("moban.png", 1);

		//获得模板在check(就是holdimg)上最佳匹配点的位置坐标
		ptloc = MatchingMethod(check, moban);

		cenlocx[z] = ptloc.x + templen / 2;
		cenlocy[z] = ptloc.y + templen / 2;
	}
	
	//采用对每一个非常刁钻的图块 用0.4 0.3 0.2三次匹配，若两次的位置很接近，就取这两次的平均值，若三次都不接近就输出XXXX
	double x1, x2, x3;
	x1 = sqrt(pow(cenlocx[0] - cenlocx[1], 2) + pow(cenlocy[0] - cenlocy[1], 2));
	x2 = sqrt(pow(cenlocx[0] - cenlocx[2], 2) + pow(cenlocy[0] - cenlocy[2], 2));
	x3 = sqrt(pow(cenlocx[1] - cenlocx[2], 2) + pow(cenlocy[1] - cenlocy[2], 2));

	error = 0;

	if (x1 < 5)
	{
		orloc.x = cvFloor((cenlocx[0] + cenlocx[1]) / 2);
		orloc.y = cvFloor((cenlocy[0] + cenlocy[1]) / 2);
	}
	else if (x2 < 5)
	{
		orloc.x = cvFloor((cenlocx[0] + cenlocx[2]) / 2);
		orloc.y = cvFloor((cenlocy[0] + cenlocy[2]) / 2);
	}
	else if (x3 < 5)
	{
		orloc.x = cvFloor((cenlocx[1] + cenlocx[2]) / 2);
		orloc.y = cvFloor((cenlocy[1] + cenlocy[2]) / 2);
	}
	else
		error = 100;

	actloc.x = cvFloor(((i + 0.5) / N)*imagelen) + validoldregion.x;
	actloc.y = cvFloor(((j + 0.5) / N)*imagelen) + validregion.y;
	
	//预警模块
	//std: distance between target piece and target location
	if ((orloc.x > validregion.x) && (orloc.x < imagelen + validregion.x)&& (orloc.y > validregion.y) && (orloc.y < imagelen + validregion.y))
		error = 100;

	if (error < 5.0)
	{
		nandu = 0;
		//画图，显示orloc和actloc的位置，并连线
		//circle(check, orloc, 2, Scalar(0, 0, 255), 2);
		//circle(check, actloc, 2, Scalar(0, 0, 255), 2);
		//line(check, orloc, actloc, Scalar(0, 0, 255), 2);

		ThunderMove(orloc, actloc);
	}
	else
	{
		//circle(check, orloc, 4, 1, 2);
		//circle(check, actloc, 4, 1, 2);
	}

	//imshow("匹配图", check);
	//rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分(这里面是匹配率最小的，也就是pipeilv=0.2时的框大小)
	//imshow("模板图", srcImg);

	//cvWaitKey(0);//等效于机器完成移动，传递会一个信息 这之前程序等待
	return nandu;
}

//the final checking process
int usrGameController::finalEagleEye(cv::Mat& img, Mat srcImg, int numpic)
{
	//深拷贝
	Mat holdimg, sourceImg;
	img.copyTo(holdimg);
	imwrite("check.png", holdimg);
	Mat check = imread("check.png", 1);
	srcImg.copyTo(sourceImg);
	imwrite("source.png", sourceImg);
	Mat source = imread("source.png", 1);

	//loc为srcImg上截图的部分的左上角坐标，ptloc为holdimg里匹配到的部分的左上角的坐标,
	Point loc, ptloc, ptlocR;
	//cenlocx/y为匹配到的部分的中心点的坐标 采用数组 每一次匹配率后都会得到一个坐标
	double cenlocx[3];
	double cenlocy[3];
	//x y和的均值，各自的方差和方差之和
	//error（在不同匹配率下location的稳定程度（方差））——数组元素值越大说明这个图块越不好，位置越刁钻)
	double meanx, meany, errorx, errory, error;

	i = numpic / N;    //图块所在的列数 从0开始到N-1列
	j = numpic % N;    //图块所在的行数 从0开始到N-1行
	
	

	if (circletime < 8)
	{
		for (int z = 0; z < 3; z++)
		{
			pipeilv = 0.3 - z / 10.0;
			//cout << '\t' << "匹配率：" << pipeilv << endl;

			templen = cvFloor((imagelen * pipeilv) / N);
			loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
			loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

			Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
			imwrite("moban.png", mobanpre);
			Mat moban = imread("moban.png", 1);

			ptloc = MatchingMethod(check, moban);

			Mat mobanpreR = check(Rect(ptloc.x, ptloc.y, templen, templen));
			imwrite("mobanR.png", mobanpreR);
			Mat mobanR = imread("mobanR.png", 1);

			ptlocR = MatchingMethod(srcImg, mobanR);

			double error_distance = sqrt(pow(loc.x - ptlocR.x, 2) + pow(loc.y - ptlocR.y, 2));

			meanx = ptloc.x + templen / 2;
			meany = ptloc.y + templen / 2;

			actloc.x = cvFloor(((i + 0.5) / N)*imagelen) + validoldregion.x;
			actloc.y = cvFloor(((j + 0.5) / N)*imagelen) + validregion.y;

			orloc.x = cvFloor(meanx);
			orloc.y = cvFloor(meany);

			circle(check, orloc, 2, Scalar(0, 0, 0), 2);
			circle(check, actloc, 2, Scalar(0, 0, 255), 2);
			line(check, orloc, actloc, Scalar(0, 0, 255), 2);

			rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);
			rectangle(source, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);
			circle(source, ptlocR, templen*0.7, Scalar(0, 0, 255), 2);

			imshow("匹配图", check);
			imshow("模板图", source);

			if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0) && error_distance < error_distance_max * imagelen / (N * 7.0))
			{
				if (display_round == 1)
				{
					cout << "[" << circletime << "-1] 检测阶段处理" << numpic + 1 << endl;
					display_round = 2;
				}

				ThunderMove(orloc, actloc);
			}
			else if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0) && error_distance >= error_distance_max * imagelen / (N * 7.0))
			{
				cout << "[" << circletime << "-0] 检测阶段跳过" << numpic + 1 << endl;
				cout << '\t' << "匹配效果不理想！" << endl;
			}
			else
			{
				//cout << "[" << circletime << "-0] 检测阶段跳过" << numpic + 1 << endl;
				break;
			}

		}

		round_check = 0;
		display_round = 1;
		numpic += 1;
	}
	else if (circletime < 10)
	{
		for (int z = 0; z < 3; z++)
		{
			pipeilv = 0.8 - 3.0 * z / 10.0;     // 0.8 0.5 0.2
		
			//计算模板大小,cvFloor向下取整  (N=3时模板大小为43 N=4是模板大小为32)
			templen = cvFloor((imagelen * pipeilv) / N);
			loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
			loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

			//模板是从srcImg上截图 遍历srcImg
			Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
			imwrite("moban.png", mobanpre);
			Mat moban = imread("moban.png", 1);

			//获得模板在check(就是holdimg)上最佳匹配点的位置坐标
			ptloc = MatchingMethod(check, moban);

			cenlocx[z] = ptloc.x + templen / 2;
			cenlocy[z] = ptloc.y + templen / 2;

			circle(check, orloc, 2, Scalar(0, 0, 0), 2);
			circle(check, actloc, 2, Scalar(0, 0, 255), 2);
			line(check, orloc, actloc, Scalar(0, 0, 255), 2);
			imshow("匹配图", check);
			imshow("模板图", source);

			// rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //在check上画框
			// rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分(这里面是匹配率最小的，也就是pipeilv=0.2时的框大小)
		}
		double x1, x2, x3;
		x1 = sqrt(pow(cenlocx[0] - cenlocx[1], 2) + pow(cenlocy[0] - cenlocy[1], 2));
		x2 = sqrt(pow(cenlocx[0] - cenlocx[2], 2) + pow(cenlocy[0] - cenlocy[2], 2));
		x3 = sqrt(pow(cenlocx[1] - cenlocx[2], 2) + pow(cenlocy[1] - cenlocy[2], 2));

		error = 0;

		if (x1 < 5)
		{
			orloc.x = cvFloor((cenlocx[0] + cenlocx[1]) / 2);
			orloc.y = cvFloor((cenlocy[0] + cenlocy[1]) / 2);
		}
		else if (x2 < 5)
		{
			orloc.x = cvFloor((cenlocx[0] + cenlocx[2]) / 2);
			orloc.y = cvFloor((cenlocy[0] + cenlocy[2]) / 2);
		}
		else if (x3 < 5)
		{
			orloc.x = cvFloor((cenlocx[1] + cenlocx[2]) / 2);
			orloc.y = cvFloor((cenlocy[1] + cenlocy[2]) / 2);
		}
		else
			error = 100;

		meanx = (cenlocx[0] + cenlocx[1] + cenlocx[2]) / 3.0;
		meany = (cenlocy[0] + cenlocy[1] + cenlocy[2]) / 3.0;

		actloc.x = cvFloor(((i + 0.5) / N)*imagelen) + validoldregion.x;
		actloc.y = cvFloor(((j + 0.5) / N)*imagelen) + validregion.y;

		if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0) && error == 0)
		{
			if (display_round == 1)
			{
				cout << "[" << circletime << "-1] 检测阶段处理" << numpic + 1 << endl;
				display_round = 2;
			}
			ThunderMove(orloc, actloc);
			//显示处理信息模块
			//cout << "图块编号" << numpic << endl;
			//cout << "图块位置 " << j << ' ' << i << endl;
			round_check += 1;
			if (round_check == 3)
			{
				numpic++;
				round_check = 0;
				display_round = 1;
			}
		}
		else if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0) && error == 100)
		{
			cout << "[" << circletime << "-0] 检测阶段跳过" << numpic + 1 << endl;
			cout << '\t' << "匹配效果不理想！" << endl;
			numpic++;
		}
		else
		{
			//cout << "[" << circletime << "-0] 检测阶段跳过" << numpic + 1 << endl;
			numpic += 1;
		}
	}
	else if (circletime < 12)
	{
		pipeilv = 0.6;

		templen = cvFloor((imagelen * pipeilv) / N);
		loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
		loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

		//模板是从srcImg上截图 遍历srcImg
		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
		imwrite("moban.png", mobanpre);
		Mat moban = imread("moban.png", 1);

		//获得模板在check(就是holdimg)上最佳匹配点的位置坐标
		ptloc = MatchingMethod(check, moban);

		meanx = ptloc.x + templen / 2;
		meany = ptloc.y + templen / 2;

		actloc.x = cvFloor(((i + 0.5) / N)*imagelen) + validoldregion.x;
		actloc.y = cvFloor(((j + 0.5) / N)*imagelen) + validregion.y;

		orloc.x = cvFloor(meanx);
		orloc.y = cvFloor(meany);

		circle(check, orloc, 2, Scalar(0, 0, 0), 2);
		circle(check, actloc, 2, Scalar(0, 0, 255), 2);
		line(check, orloc, actloc, Scalar(0, 0, 255), 2);
		imshow("匹配图", check);
		imshow("模板图", source);

		if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0))
		{
			cout << "[" << circletime << "-1] 检测阶段处理" << numpic + 1 << endl;
			ThunderMove(orloc, actloc);
		}
		round_check += 1;

		if (round_check == 3)
		{
			numpic++;
			round_check = 0;
			display_round = 1;
		}
	}
	else
	{
		//wadiao
	}
	
	return numpic;
}
// definition of function ThunderMove
void usrGameController::ThunderMove(cv::Point origin_loc, cv::Point target_loc)
{
	//cout << "original location: " << origin_loc.x << '\t' << origin_loc.y << endl;
	//cout << "target   location: " << target_loc.x << '\t' << target_loc.y << endl;
	offset = sqrt(pow(origin_loc.x - target_loc.x, 2) + pow(origin_loc.y - target_loc.y, 2));
	offset_x = OFFSET*(target_loc.x - origin_loc.x) / offset;
	offset_y = OFFSET*(target_loc.y - origin_loc.y) / offset;
	if (abs(orloc.x - actloc.x) < 2 && abs(orloc.y - actloc.y) < 2)
	{
		offset_x = 0;
		offset_y = 0;
	}
	//cout << "offset      value: " << offset_x << '\t' << offset_y << endl;
	//system("PAUSE");
	device->comMoveTo(constraint(origin_loc.x - LEFT_CUT, 0, WIDTH)*RANGE_X / WIDTH, constraint(origin_loc.y - UP_CUT, 0, HEIGHT)*RANGE_Y / HEIGHT);
	device->comHitDown();
	device->comMoveTo(constraint(target_loc.x + offset_x - LEFT_CUT, 0, WIDTH)*RANGE_X / WIDTH, constraint(target_loc.y + offset_y - UP_CUT, 0, HEIGHT)*RANGE_Y / HEIGHT);
	Sleep(STEP_DELAY);
	device->comHitUp();

	if (isToAdjust)
	{
		//top right corner
		device->comHitDown();
		device->comMoveTo(constraint(target_loc.x + offset_x + ADJUST - LEFT_CUT, 0, WIDTH)*RANGE_X / WIDTH, constraint(target_loc.y + offset_y - ADJUST - UP_CUT, 0, HEIGHT)*RANGE_Y / HEIGHT);
		Sleep(SMALL_STEP_DELAY);
		device->comHitUp();

		//top left corner
		device->comHitDown();
		device->comMoveTo(constraint(target_loc.x + offset_x - ADJUST - LEFT_CUT, 0, WIDTH)*RANGE_X / WIDTH, constraint(target_loc.y + offset_y - ADJUST - UP_CUT, 0, HEIGHT)*RANGE_Y / HEIGHT);
		Sleep(SMALL_STEP_DELAY);
		device->comHitUp();

		//bottom left corner
		device->comHitDown();
		device->comMoveTo(constraint(target_loc.x + offset_x - ADJUST - LEFT_CUT, 0, WIDTH)*RANGE_X / WIDTH, constraint(target_loc.y + offset_y + ADJUST - UP_CUT, 0, HEIGHT)*RANGE_Y / HEIGHT);
		Sleep(SMALL_STEP_DELAY);
		device->comHitUp();

		//bottom right corner
		device->comHitDown();
		device->comMoveTo(constraint(target_loc.x + offset_x + ADJUST - LEFT_CUT, 0, WIDTH)*RANGE_X / WIDTH, constraint(target_loc.y + offset_y + ADJUST - UP_CUT, 0, HEIGHT)*RANGE_Y / HEIGHT);
		Sleep(SMALL_STEP_DELAY);
		device->comHitUp();
	}
}
//constraint
int constraint(int x, int border_min, int border_max)
{
	if (x < border_min)x = border_min;
	else if (x > border_max)x = border_max;
	return x;
}
#endif
