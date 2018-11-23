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
	device = new deviceCyberDip(qtCD);//�豸������
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
int imagelen = 255;		//������ƴͼ��С��Ҳǡ�õ���ƴͼ�м�����Ĵ�С
//ʵ��ƴͼ������������Ͻ����꣬��ֱ�ӽ�ͼ�������
Point validregion = Point(55, 247);
//����ģ�����Ƭ�����ȱ���õ���Ƭ����ʵ��ƴͼ����
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
//ƴͼ�״�
int N = 0;
//check whether the game is ready to go
bool isReady = 0;



//----------------- about image -------------------//
//ԭʼͼ��
Mat srcImg;
//ģ��ͼ��
Mat templImg;
//ƥ����ͼ��
Mat resultImg;



//------------- about jigsaw pieces ---------------//
//ƴͼͼ������
int pinumber;
//����һ������,����Ԫ�ص�����Ϊƴͼͼ������,��¼ÿһ��ƴͼͼ���ڵ��Ѷ��Ǽ�
static vector<int> eva(1);
//ģ��Ĵ�С��ѡ�����������򣬱߳���ƴͼ�״κ�ʶ�����йأ�
//ʶ������һ���Ŵ���ڵ�ϵ������Ϊƴͼͼ���Ե��͹��ƽ���п�����Ҫ��ε���ģ���Сȥɨ��
int templen;
//ͼ���ţ���ÿһ��ͼ���ţ������ȴ��ϵ��£��ٴ����ҵ�˳���ţ����Ͻ���С(0)�����½����(N*N-1)
//numpic ��0��ʼ ��N*N-1����
int numpic = 0;



//----------- about image processing --------------//
//�������� ���ڼ�����ͼ���Ѷ� Ϊ0˵��ͼ��ܺ�,���Ա��ƶ�
//Ϊ1˵��ͼ��ܵ���,��ֵԽ��˵��Խ����
int nandu;
//ƥ���� Ҳ����ģ��߳�������ƴͼͼ�飨û�а�͹������һģһ���������Σ��߳��ı�ֵ
double pipeilv = 0.9;
//i�Ǻ���� ����loc.x  Ҳ����ͼ������� ͼ���ڵڼ���
//j������� ����loc.y
int i = 0, j = 0;
//���������� orloc�Ǵ��ƶ�ͼ�����ĵ�����꣬actloc�Ǵ��ƶ�ͼ��Ӧ���ƶ���������
Point orloc, actloc;
//����������ǰ���ζ���ӥ�ۣ�����֮�������Ϊû��ͼ�黥���ڵ�������ˣ��������ͼ��ʣ�࣬���Ĵο�ʼ�ó���ӥ�ۣ�
int circletime = 0;
//offset for elastic pointerhead
int offset_x, offset_y, offset;
//the round for checking the same piece
int round_check = 0;
//the round of displaying process
int display_round = 0;



//----------- function declarations ---------------//
//ƥ�亯��  ��һ���������ԭʼͼ�� �ڶ�����ģ��ͼ��
Point MatchingMethod(Mat, Mat);
//template matching function, returning match rate
double templatematch_gamestart(cv::Mat, cv::Mat);
//constraint function, avoid crashing
int constraint(int x, int border_min, int border_max);



//����ͼ�� 
int usrGameController::usrProcessImage(cv::Mat& img)      //img���Ƕ���������ͼ�� ��С��306*641����
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
			cout << "[?] ������ƴͼ��ţ� ";
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
		cout << "[?] ������ƴͼ�״Σ� ";
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

		if (circletime == 0)            //��һ��ѭ��ȫ����Ҫ������
		{
			if (numpic == 0)
				cout << "[*] EagleEye: lap" << circletime << endl;

			eva[numpic] = EagleEye(img, srcImg, numpic, circletime);
			if (numpic < pinumber - 1)  //��Ϊnumpic�Ǵ�0��ʼ�ģ�Ӧ�õ�����pinumber-1������������if���д��������β��������С��pinnumber-1����Ϊnumpic=pinumber-2�󻹻��ټ�1��ִ��һ��������
			{
				numpic = numpic + 1;    //ͼ���ż�1 ����һ��ͼ����д���
				//cout << "[" << circletime << "-1] ����" << numpic << endl;
			}
			else
			{
				//cout << "[" << circletime << "-1] ����" << numpic + 1 << endl;
				cout << "[*] EagleEye: lap" << circletime<<" completed!" << endl;
				cout << endl;
				circletime = circletime + 1;
				numpic = 0;
			}
		}
		else if (circletime < 4)        //��������ӥ�ۣ����һ�α����ģ�֮����õ���ƥ����
		{
			if(numpic == 0)
				cout << "[*] EagleEye: lap" << circletime << endl;

			if ((numpic < pinumber) && (eva[numpic] == 0))				//�ȼ��ͼ���Ӧ���Ѷȣ���Ϊ0�Ϳ���������һ����
			{
				cout << "[" << circletime << "-0] ����" << numpic + 1 << endl;
				numpic = numpic + 1;
			}
			else if ((numpic < pinumber) && (eva[numpic] == 1))
			{
				//cout << "[" << circletime << "-1] ����" << numpic + 1 << endl;
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
		else if (circletime < 6)        //�����Ĵ�ӥ�ۣ����һ�α����ģ�֮�����ultiӥ��  
		{
			if (numpic == 0)
				cout << "[*] UltiEagleEye: lap" << circletime - 3 << endl;

			if ((numpic < pinumber) && (eva[numpic] == 0))
			{
				cout << "[" << circletime << "-0] ����" << numpic + 1 << endl;
				numpic = numpic + 1;
			}
			else if ((numpic < pinumber) && (eva[numpic] == 1))
			{
				//cout << "[" << circletime << "-1] ����" << numpic + 1 << endl;
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
//ģ��ƥ�亯��
//2017.12.12���ҵ������ƥ������λ��ΪmatchLoc������ƥ���λ��ת��Ϊƴͼ����������꣬����pt����ʾ
Point MatchingMethod(Mat srcImgb, Mat templImgb)
{
	// ���������ʾ
	Mat displayImg;
	srcImgb.copyTo(displayImg);

	// ����ƥ����ͼ��Ϊÿ��ģ��λ�ô洢ƥ����
	// ƥ����ͼ���СΪ��(W-w+1)*(H-h+1)
	int result_cols = srcImgb.cols - templImgb.cols + 1;
	int result_rows = srcImgb.rows - templImgb.rows + 1;
	resultImg.create(result_cols, result_rows, CV_32FC1);

	// ����ƥ�䲢��һ��
	// ����CV_TM_SQDIFF�� CV_TM_SQDIFF_NORMED�����ַ�������СֵΪ���ƥ�䣻���ڱ�ķ������ֵΪ���ƥ��
	matchTemplate(srcImgb, templImgb, resultImg, CV_TM_SQDIFF_NORMED);
	normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());

	// ʹ��minMaxLoc�ҳ����ƥ��
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
	//���
	Mat holdimg;
	img.copyTo(holdimg);
	imwrite("check.png", holdimg);
	Mat check = imread("check.png", 1);

	//locΪsrcImg�Ͻ�ͼ�Ĳ��ֵ����Ͻ����꣬ptlocΪholdimg��ƥ�䵽�Ĳ��ֵ����Ͻǵ�����,
	Point loc, ptloc;
	//cenlocx/yΪƥ�䵽�Ĳ��ֵ����ĵ������ �������� ÿһ��ƥ���ʺ󶼻�õ�һ������
	double cenlocx[4];
	double cenlocy[4];
	//x y�͵ľ�ֵ�����Եķ���ͷ���֮��
	//error���ڲ�ͬƥ������location���ȶ��̶ȣ��������������Ԫ��ֵԽ��˵�����ͼ��Խ���ã�λ��Խ����)
	double meanx, meany, errorx, errory, error;

	i = numpic / N;    //ͼ�����ڵ����� ��0��ʼ��N-1��
	j = numpic % N;    //ͼ�����ڵ����� ��0��ʼ��N-1��

	nandu = 1;

	for (int z = 0; z < 4; z++)  //��ͬƥ���� z<9ʱ�൱��ƥ���ʴ�0.9��0.1��9�Σ�
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
		
		//����ģ���С,cvFloor����ȡ��  (N=3ʱģ���СΪ43 N=4��ģ���СΪ32)
		templen = cvFloor((imagelen * pipeilv) / N);
		loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
		loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

		//ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
		imwrite("moban.png", mobanpre);
		Mat moban = imread("moban.png", 1);

		//���ģ����check(����holdimg)�����ƥ����λ������
		ptloc = MatchingMethod(check, moban);

		cenlocx[z] = ptloc.x + templen / 2;
		cenlocy[z] = ptloc.y + templen / 2;
	}

	//ijͼ���ڲ�ͬƥ������ptloc.x��ƽ��ֵ
	meanx = (cenlocx[0] + cenlocx[1] + cenlocx[2] + cenlocx[3]) / 4.0;
	meany = (cenlocy[0] + cenlocy[1] + cenlocy[2] + cenlocy[3]) / 4.0;

	errorx = (cenlocx[0] * cenlocx[0] + cenlocx[1] * cenlocx[1] + cenlocx[2] * cenlocx[2] + cenlocx[3] * cenlocx[3]) / 4.0 - meanx*meanx;  //�󷽲�
	errory = (cenlocy[0] * cenlocy[0] + cenlocy[1] * cenlocy[1] + cenlocy[2] * cenlocy[2] + cenlocy[3] * cenlocy[3]) / 4.0 - meany*meany;  //�󷽲�
	error = errorx + errory;

	actloc.x = cvFloor(((i + 0.5) / N)*imagelen) + validoldregion.x;
	actloc.y = cvFloor(((j + 0.5) / N)*imagelen) + validregion.y;

	//Ԥ��ģ�� Ҫ�ƶ���ƴͼͼ�鲻Ӧ�������м�������������ڣ�ֱ��error��Ϊ100
	if ((meanx > validregion.x) && (meanx < imagelen + validregion.x)&& (meany > validregion.y) && (meany < imagelen + validregion.y))
		error = 100;

	if (error < 5.0)
	{
		nandu = 0;
		orloc.x = cvFloor(meanx);
		orloc.y = cvFloor(meany);
		//��ͼ����ʾorloc��actloc��λ�ã�������
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
	//��ʾ������Ϣģ��
	//cout << "ͼ����" << numpic << endl;
	//cout << "ͼ��λ�� " << j << ' ' << i << endl;
	//cout << "ͼ�����" << error << endl;
	//cout << "ͼ���Ѷ� "<< nandu << endl << endl;

	//imshow("ƥ��ͼ", check);
	//rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen),Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���(��������ƥ������С�ģ�Ҳ����pipeilv=0.2ʱ�Ŀ��С)
	//imshow("ģ��ͼ", srcImg);

	//cvWaitKey(0);//��Ч�ڻ�������ƶ������ݻ�һ����Ϣ ��֮ǰ����ȴ�
	return nandu;
}
int usrGameController::UltiEagleEye(cv::Mat& img, Mat srcImg, int numpic ,int circletime)
{
	//���
	Mat holdimg;
	img.copyTo(holdimg);
	imwrite("check.png", holdimg);
	Mat check = imread("check.png", 1);

	//locΪsrcImg�Ͻ�ͼ�Ĳ��ֵ����Ͻ����꣬ptlocΪholdimg��ƥ�䵽�Ĳ��ֵ����Ͻǵ�����,
	Point loc, ptloc;
	//cenlocx/yΪƥ�䵽�Ĳ��ֵ����ĵ������ �������� ÿһ��ƥ���ʺ󶼻�õ�һ������
	double cenlocx[3];
	double cenlocy[3];
	//x y�͵ľ�ֵ�����Եķ���ͷ���֮��
	//error���ڲ�ͬƥ������location���ȶ��̶ȣ��������������Ԫ��ֵԽ��˵�����ͼ��Խ���ã�λ��Խ����)
	double error;

	i = numpic / N;    //ͼ�����ڵ����� ��0��ʼ��N-1��
	j = numpic % N;    //ͼ�����ڵ����� ��0��ʼ��N-1��

	nandu = 1;

	for (int z = 0; z < 3; z++)  //����ƥ���� ��0.4��0.1������������������Ϊ��0.6-0.4
	{
		pipeilv = 0.4 - z * 0.1;   //0.4 0.3 0.2
		
		//����ģ���С,cvFloor����ȡ��  (N=3ʱģ���СΪ43 N=4��ģ���СΪ32)
		templen = cvFloor((imagelen * pipeilv) / N);
		loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
		loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

		//ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
		imwrite("moban.png", mobanpre);
		Mat moban = imread("moban.png", 1);

		//���ģ����check(����holdimg)�����ƥ����λ������
		ptloc = MatchingMethod(check, moban);

		cenlocx[z] = ptloc.x + templen / 2;
		cenlocy[z] = ptloc.y + templen / 2;
	}
	
	//���ö�ÿһ���ǳ������ͼ�� ��0.4 0.3 0.2����ƥ�䣬�����ε�λ�úܽӽ�����ȡ�����ε�ƽ��ֵ�������ζ����ӽ������XXXX
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
	
	//Ԥ��ģ��
	//std: distance between target piece and target location
	if ((orloc.x > validregion.x) && (orloc.x < imagelen + validregion.x)&& (orloc.y > validregion.y) && (orloc.y < imagelen + validregion.y))
		error = 100;

	if (error < 5.0)
	{
		nandu = 0;
		//��ͼ����ʾorloc��actloc��λ�ã�������
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

	//imshow("ƥ��ͼ", check);
	//rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���(��������ƥ������С�ģ�Ҳ����pipeilv=0.2ʱ�Ŀ��С)
	//imshow("ģ��ͼ", srcImg);

	//cvWaitKey(0);//��Ч�ڻ�������ƶ������ݻ�һ����Ϣ ��֮ǰ����ȴ�
	return nandu;
}

//the final checking process
int usrGameController::finalEagleEye(cv::Mat& img, Mat srcImg, int numpic)
{
	//���
	Mat holdimg, sourceImg;
	img.copyTo(holdimg);
	imwrite("check.png", holdimg);
	Mat check = imread("check.png", 1);
	srcImg.copyTo(sourceImg);
	imwrite("source.png", sourceImg);
	Mat source = imread("source.png", 1);

	//locΪsrcImg�Ͻ�ͼ�Ĳ��ֵ����Ͻ����꣬ptlocΪholdimg��ƥ�䵽�Ĳ��ֵ����Ͻǵ�����,
	Point loc, ptloc, ptlocR;
	//cenlocx/yΪƥ�䵽�Ĳ��ֵ����ĵ������ �������� ÿһ��ƥ���ʺ󶼻�õ�һ������
	double cenlocx[3];
	double cenlocy[3];
	//x y�͵ľ�ֵ�����Եķ���ͷ���֮��
	//error���ڲ�ͬƥ������location���ȶ��̶ȣ��������������Ԫ��ֵԽ��˵�����ͼ��Խ���ã�λ��Խ����)
	double meanx, meany, errorx, errory, error;

	i = numpic / N;    //ͼ�����ڵ����� ��0��ʼ��N-1��
	j = numpic % N;    //ͼ�����ڵ����� ��0��ʼ��N-1��
	
	

	if (circletime < 8)
	{
		for (int z = 0; z < 3; z++)
		{
			pipeilv = 0.3 - z / 10.0;
			//cout << '\t' << "ƥ���ʣ�" << pipeilv << endl;

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

			imshow("ƥ��ͼ", check);
			imshow("ģ��ͼ", source);

			if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0) && error_distance < error_distance_max * imagelen / (N * 7.0))
			{
				if (display_round == 1)
				{
					cout << "[" << circletime << "-1] ���׶δ���" << numpic + 1 << endl;
					display_round = 2;
				}

				ThunderMove(orloc, actloc);
			}
			else if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0) && error_distance >= error_distance_max * imagelen / (N * 7.0))
			{
				cout << "[" << circletime << "-0] ���׶�����" << numpic + 1 << endl;
				cout << '\t' << "ƥ��Ч�������룡" << endl;
			}
			else
			{
				//cout << "[" << circletime << "-0] ���׶�����" << numpic + 1 << endl;
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
		
			//����ģ���С,cvFloor����ȡ��  (N=3ʱģ���СΪ43 N=4��ģ���СΪ32)
			templen = cvFloor((imagelen * pipeilv) / N);
			loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
			loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

			//ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
			Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
			imwrite("moban.png", mobanpre);
			Mat moban = imread("moban.png", 1);

			//���ģ����check(����holdimg)�����ƥ����λ������
			ptloc = MatchingMethod(check, moban);

			cenlocx[z] = ptloc.x + templen / 2;
			cenlocy[z] = ptloc.y + templen / 2;

			circle(check, orloc, 2, Scalar(0, 0, 0), 2);
			circle(check, actloc, 2, Scalar(0, 0, 255), 2);
			line(check, orloc, actloc, Scalar(0, 0, 255), 2);
			imshow("ƥ��ͼ", check);
			imshow("ģ��ͼ", source);

			// rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //��check�ϻ���
			// rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���(��������ƥ������С�ģ�Ҳ����pipeilv=0.2ʱ�Ŀ��С)
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
				cout << "[" << circletime << "-1] ���׶δ���" << numpic + 1 << endl;
				display_round = 2;
			}
			ThunderMove(orloc, actloc);
			//��ʾ������Ϣģ��
			//cout << "ͼ����" << numpic << endl;
			//cout << "ͼ��λ�� " << j << ' ' << i << endl;
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
			cout << "[" << circletime << "-0] ���׶�����" << numpic + 1 << endl;
			cout << '\t' << "ƥ��Ч�������룡" << endl;
			numpic++;
		}
		else
		{
			//cout << "[" << circletime << "-0] ���׶�����" << numpic + 1 << endl;
			numpic += 1;
		}
	}
	else if (circletime < 12)
	{
		pipeilv = 0.6;

		templen = cvFloor((imagelen * pipeilv) / N);
		loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.x;
		loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + validoldregion.y;

		//ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));
		imwrite("moban.png", mobanpre);
		Mat moban = imread("moban.png", 1);

		//���ģ����check(����holdimg)�����ƥ����λ������
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
		imshow("ƥ��ͼ", check);
		imshow("ģ��ͼ", source);

		if (sqrt(pow(orloc.x - actloc.x, 2) + pow(orloc.y - actloc.y, 2)) > min_distance * imagelen / (N * 7.0))
		{
			cout << "[" << circletime << "-1] ���׶δ���" << numpic + 1 << endl;
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
