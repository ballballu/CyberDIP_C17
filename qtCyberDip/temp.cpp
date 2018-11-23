/////**
////* Object matching using function matchTemplate
////*/
////
////#include "opencv2/highgui/highgui.hpp"
////#include "opencv2/imgproc/imgproc.hpp"
////#include <iostream>
////#include <stdio.h>
////using namespace std;
////using namespace cv;
////
/////// 全局变量 ///
////Mat srcImg;			//原始图像
////Mat templImg;		//模板图像
////Mat resultImg;		//匹配结果图像
////
////const char* imageWindow = "Source Image";		//原始图像显示窗口
////const char* resultWindow = "Result Window";		//匹配结果图像显示窗口
////
////int matchMethod;		//匹配方法index
////int maxTrackbar = 5;	//滑动条范围（与匹配方法个数对应）
////
////						/// 函数声明 ///
////void MatchingMethod(int, void*);		//匹配函数
////
////int main(int argc, char** argv)
////{
////	// 加载原始图像和模板图像
////	srcImg = imread("2111.png", 1);
////	templImg = imread("mo1.png", 1);
////
////	// 创建显示窗口
////	namedWindow(imageWindow, CV_WINDOW_AUTOSIZE);
////	namedWindow(resultWindow, CV_WINDOW_AUTOSIZE);
////
////	// 创建滑动条
////	char* trackbarLabel =
////		"Method: \n \
////	  0: SQDIFF \n \
////	  1: SQDIFF NORMED \n \
////	  2: TM CCORR \n \
////	  3: TM CCORR NORMED \n \
////	  4: TM COEFF \n \
////	  5: TM COEFF NORMED";
////	//参数：滑动条名称 显示窗口名称 匹配方法index 滑动条范围 回调函数
////	createTrackbar(trackbarLabel, imageWindow, &matchMethod, maxTrackbar, MatchingMethod);
////
////	MatchingMethod(0, 0);
////
////	waitKey(0);
////	return 0;
////}
////
/////// 函数定义 ///
////void MatchingMethod(int, void*)		//匹配函数
////{
////	// 深拷贝用于显示
////	Mat displayImg;
////	srcImg.copyTo(displayImg);
////
////	// 创建匹配结果图像，为每个模板位置存储匹配结果
////	// 匹配结果图像大小为：(W-w+1)*(H-h+1)
////	int result_cols = srcImg.cols - templImg.cols + 1;
////	int result_rows = srcImg.rows - templImg.rows + 1;
////	resultImg.create(result_cols, result_rows, CV_32FC1);
////
////	// 进行匹配并归一化
////	matchTemplate(srcImg, templImg, resultImg, matchMethod);
////	normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());
////
////	// 使用minMaxLoc找出最佳匹配
////	double minVal, maxVal;
////	Point minLoc, maxLoc, matchLoc;
////	minMaxLoc(resultImg, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
////
////	// 对于CV_TM_SQDIFF和 CV_TM_SQDIFF_NORMED这两种方法，最小值为最佳匹配；对于别的方法最大值为最佳匹配
////	if (matchMethod == CV_TM_SQDIFF || matchMethod == CV_TM_SQDIFF_NORMED)
////	{
////		matchLoc = minLoc;
////	}
////	else
////	{
////		matchLoc = maxLoc;
////	}
////
////	// 在原始图像和匹配结果图像中以最佳匹配点为左上角标出最佳匹配框
////	rectangle(displayImg, matchLoc, Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows), Scalar::all(0), 2, 8, 0);
////	rectangle(resultImg, matchLoc, Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows), Scalar::all(0), 2, 8, 0);
////
////	imshow(imageWindow, displayImg);
////	imshow(resultWindow, resultImg);
////
////	return;
////}
////
////
///////// 全局变量 ///
//////Mat srcImg;			//原始图像
//////Mat templImg;		//模板图像
//////Mat resultImg;		//匹配结果图像
//////
//////					/// 函数声明 ///
//////
//////					// 加载原始图像和模板图像
//////srcImg = imread("111.png", 1);
//////templImg = imread("mo1.png", 1);
//////
//////// 深拷贝用于显示
//////Mat displayImg;
//////srcImg.copyTo(displayImg);
//////
//////// 创建匹配结果图像，为每个模板位置存储匹配结果
//////// 匹配结果图像大小为：(W-w+1)*(H-h+1)
//////int result_cols = srcImg.cols - templImg.cols + 1;
//////int result_rows = srcImg.rows - templImg.rows + 1;
//////resultImg.create(result_cols, result_rows, CV_32FC1);
//////
//////// 进行匹配并归一化
//////matchTemplate(srcImg, templImg, resultImg, CV_TM_CCOEFF);				// 对于CV_TM_SQDIFF和 CV_TM_SQDIFF_NORMED这两种方法，最小值为最佳匹配；对于别的方法最大值为最佳匹配
//////normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());
//////
//////// 使用minMaxLoc找出最佳匹配
//////double minVal, maxVal;
//////Point minLoc, maxLoc, matchLoc;
//////minMaxLoc(resultImg, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
//////
//////// 在原始图像和匹配结果图像中以最佳匹配点为左上角标出最佳匹配框
//////rectangle(displayImg, matchLoc, Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows), Scalar::all(0), 2, 8, 0);
//////rectangle(resultImg, matchLoc, Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows), Scalar::all(0), 2, 8, 0);
//////
//////imshow("Source Image", displayImg);				//原始图像显示窗口
//////imshow("Result Window", resultImg);				//匹配结果图像显示窗口
//////
//////waitKey(0);
//#include "usrGameController.h"
//
//#include "cv.h"
//#include "highgui.h"
//#include "opencv2/opencv.hpp"
//using namespace cv;
//using namespace std;
//
/////////2017.12.12
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include <iostream>
//#include <stdio.h>
//using namespace std;
//using namespace cv;
//
//int x()
//{
//	//图像细化
//		Mat srcImg2 = imread("2222.png", 1);
//		cv::Mat src;
//		//将原图像转换为二值图像  
//		cv::threshold(srcImg2, src, 128, 1, cv::THRESH_BINARY);
//	if (src.empty()) return 0;//图像为空，直接返回
//							//cv::threshold(src, src, 128, 1, CV_THRESH_BINARY);//转为0或1的图像
//
//	int ImgHeight = src.rows;
//	int ImgWidth = src.cols;
//
//	int count = 0;  //记录迭代次数
//	while (true)
//	{
//		count++;
//		if ( count > 50) //限制次数并且迭代次数到达
//			break;
//
//		vector<pair<int, int>> mFlag; //用于标记需要删除的点
//									  //对点标记
//		for (int i = 0; i < ImgHeight; ++i)
//		{
//			for (int j = 0; j < ImgWidth; ++j)
//			{
//				//如果满足四个条件，进行标记
//				//  p9 p2 p3
//				//  p8 p1 p4
//				//  p7 p6 p5
//				int p1 = src.at
//
//					<uchar>(i, j);
//				int p2 = (i == 0) ? 0 : src.at
//
//					<uchar>(i - 1, j);
//				int p3 = (i == 0 || j == ImgWidth - 1) ? 0 : src.at
//
//					<uchar>(i - 1, j + 1);
//				int p4 = (j == ImgWidth - 1) ? 0 : src.at
//
//					<uchar>(i, j + 1);
//				int p5 = (i == ImgHeight - 1 || j == ImgWidth - 1) ? 0 : src.at
//
//					<uchar>(i + 1, j + 1);
//				int p6 = (i == ImgHeight - 1) ? 0 : src.at
//
//					<uchar>(i + 1, j);
//				int p7 = (i == ImgHeight - 1 || j == 0) ? 0 : src.at
//
//					<uchar>(i + 1, j - 1);
//				int p8 = (j == 0) ? 0 : src.at
//
//					<uchar>(i, j - 1);
//				int p9 = (i == 0 || j == 0) ? 0 : src.at
//
//					<uchar>(i - 1, j - 1);
//
//				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
//				{
//					int ap = 0;
//					if (p2 == 0 && p3 == 1) ++ap;
//					if (p3 == 0 && p4 == 1) ++ap;
//					if (p4 == 0 && p5 == 1) ++ap;
//					if (p5 == 0 && p6 == 1) ++ap;
//					if (p6 == 0 && p7 == 1) ++ap;
//					if (p7 == 0 && p8 == 1) ++ap;
//					if (p8 == 0 && p9 == 1) ++ap;
//					if (p9 == 0 && p2 == 1) ++ap;
//
//					if (ap == 1)
//					{
//						if (p2*p4*p6 == 0)
//						{
//							if (p4*p6*p8 == 0)
//							{
//								//标记
//								mFlag.push_back(make_pair(i, j));
//							}
//						}
//					}
//				}
//			}
//		}
//		//将标记的点删除
//		for (vector<pair<int, int> >::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
//		{
//			src.at
//
//				<uchar>(i->first, i->second) = 0;
//		}
//
//		//直到没有点满足，算法结束
//		if (mFlag.size() == 0) break;
//		else mFlag.clear();//将mFlag清空
//						   //cv::threshold(src, src, 0, 255, CV_THRESH_BINARY);//二值化图像
//	}
//	imshow("qu", src);
//	return 0;
//}



//n = n + 1;
//cvtColor(holdimg, holdimggrey, CV_BGR2GRAY);  //转化为灰度图像并保存,便于减小接下来的所有的运算量（只有一个通道了）
//imwrite("change.png", holdimggrey);
//if ((n % 2) == 1)
//{
//	holdimgold = imread("change.png", 1);
//}
//else
//{
//	holdimgnew = imread("change.png", 1);
//}
//
//if (n > 5)
//{
//	absdiff(holdimgnew, holdimgold, holdimgout);
//	//if(holdimgout== holdimgout2)
//	imshow("change", holdimgout);
//	//imshow("old", holdimgold);
//	//imshow("new", holdimgnew);
//
//
//	////图像细化
//	//Mat srcImg2 = imread("2222.png", 1);
//	//Mat out;
//	////将原图像转换为二值图像  
//	//cv::threshold(srcImg2, srcImg2, 128, 1, cv::THRESH_BINARY);
//
//	//out = thinImage(srcImg2);
//	//imshow("qu", out);
//
//	////////////////////////////////////////////////2017.12.13比较两幅图片
//
//
//	//mat1 与 mat2 进行比较   
//	//mat2 与 mat3 进行比较
//	Mat mat1 = holdimgout2;
//	Mat mat2 = holdimgout;
//	int nrOfElements1 = mat1.total()*mat1.elemSize();
//	int lvRet = memcmp(mat1.data, mat2.data, nrOfElements1);
//	cout << lvRet << endl;
//	//Mat mat3 = Mat::ones(100, 100, CV_8UC1);
//	//int Rows = mat1.rows;
//	//int Cols = mat1.cols * mat1.channels();
//	//int IsStopOutLoop = 0;
//	//bool bRet = true;
//	//do
//	//{
//	//	for (int i = 0; i < Rows; i++)
//	//	{
//	//		uchar *data1 = mat1.ptr<uchar>(i);
//	//		uchar *data2 = mat2.ptr<uchar>(i);
//	//		for (int j = 0; j < Cols; j++)
//	//		{
//	//			if (data1[j] != data2[j])
//	//			{
//	//				IsStopOutLoop++;
//	//				bRet = false;
//	//				break;
//	//			}
//	//		}
//	//		if (IsStopOutLoop != 0)
//	//			break;
//	//	}
//	//	//bRet = true;
//	//} while (false);
//	//if (bRet == false)
//	//{
//	//	//如果两幅图片不相等  进行相应的处理  这里就用cout模拟了
//	//	cout << "不同的" << endl;
//	//}
//	//else
//	//{
//	//	//如果两幅图片相等   进行相应的处理  这里就用cout模拟了
//	//	cout << "相同的" << endl;
//	//}
//}



//2017.12.14删去的代码 遍历九遍的
//设置模板位置，使其遍历srcImg
/*
for (int i = 0; i < N; i++)			//i是横向的 决定loc.x
{
for (int j = 0; j < N; j++)		//j是纵向的 决定loc.y
{
for (int z = 0; z < 9; z++)  //不同匹配率 z<9时相当于匹配率从0.9到0.1（9次）
{
pipeilv = 0.9 - z / 10.0;
templen = cvFloor((imagelen * pipeilv) / N);     //计算模板大小,cvFloor向下取整  (N=3时模板大小为43 N=4是模板大小为32)
loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + 46;
loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + 297;

Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));          //模板是从srcImg上截图 遍历srcImg
imwrite("moban.png", mobanpre);
Mat moban = imread("moban.png", 1);

ptloc = MatchingMethod(check, moban);  //获得模板在check(就是holdimg)上最佳匹配点的位置坐标

cenlocx[z] = ptloc.x + templen / 2;
cenlocy[z] = ptloc.y + templen / 2;

rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //在holding上画框
imshow("匹配图", check);
rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分
imshow("模板图", srcImg);
}
meanx = (cenlocx[0] + cenlocx[1] + cenlocx[2] + cenlocx[3] + cenlocx[4] + cenlocx[5] + cenlocx[6] + cenlocx[7] + cenlocx[8])/9.0;  //ij图块在不同匹配率下ptloc.x的平均值
meany = (cenlocy[0] + cenlocy[1] + cenlocy[2] + cenlocy[3] + cenlocy[4] + cenlocy[5] + cenlocy[6] + cenlocy[7] + cenlocy[8])/9.0;

errorx = (cenlocx[0] * cenlocx[0] + cenlocx[1] * cenlocx[0] + cenlocx[2] * cenlocx[2] + cenlocx[3] * cenlocx[3] + cenlocx[4] * cenlocx[4] + cenlocx[5] * cenlocx[5] + cenlocx[6] * cenlocx[6] + cenlocx[7] * cenlocx[7] + cenlocx[8] * cenlocx[8]) / 9.0 - meanx*meanx;  //求方差
errory = (cenlocy[0] * cenlocy[0] + cenlocy[1] * cenlocy[0] + cenlocy[2] * cenlocy[2] + cenlocy[3] * cenlocy[3] + cenlocy[4] * cenlocy[4] + cenlocy[5] * cenlocy[5] + cenlocy[6] * cenlocy[6] + cenlocy[7] * cenlocy[7] + cenlocy[8] * cenlocy[8]) / 9.0 - meany*meany;  //求方差

cout << meanx << endl;
cout << meany << endl << endl;
cout << errorx << endl;
cout << errory << endl << endl;

rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //在holding上画框
imshow("匹配图", check);
rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分
imshow("模板图", srcImg);

cvWaitKey(0);
}
}*/





//2017.12.14删掉的第二批代码
/*		drawloc.x = cvFloor(((1 + 0.2) / N)*imagelen) + 46;
		drawloc.y = cvFloor(((1 + 0.2) / N)*imagelen) + 297;
		rectangle(check3, drawloc, Point(drawloc.x + templen, drawloc.y + templen), Scalar::all(0), 2, 8, 0);
		loc.x = drawloc.x;
		loc.y = drawloc.y;

		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));          //模板是从srcImg上截图 遍历srcImg
		imwrite("moban.png", mobanpre);

		Mat moban = imread("moban.png", 1);

		ptloc = MatchingMethod(check, moban);  //获得模板在check(就是holdimg)上最佳匹配点的位置坐标

		rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //在holding上画框
		imshow("匹配图", check);
		rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分
		imshow("模板图", srcImg);

		cvWaitKey(0);
*/	



/*
loc.x = 200;
loc.y = 340;
Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));          //模板是从srcImg上截图 遍历srcImg
imwrite("moban.png", mobanpre);

Mat moban = imread("moban.png", 1);

ptloc = MatchingMethod(check, moban);  //获得模板在check(就是holdimg)上最佳匹配点的位置坐标

rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //在holding上画框
imshow("匹配图", check);
rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //在srcImg上画框,框出做模板的部分
imshow("模板图", srcImg);
*/

//if ((numpic < pinumber - 1) && (eva[numpic] == 0))
//{
//	cout << "跳过" << numpic + 1 << endl;
//	numpic = numpic + 1;
//}
//else if ((numpic < pinumber - 1) && (eva[numpic] == 1))
//{
//	cout << "处理" << numpic + 1 << endl;
//	eva[numpic] = superEagleEye(img, srcImg, numpic);
//	numpic = numpic + 1;    //图块编号加1 对下一个图块进行处理
//}
//else
//{
//	circletime = circletime + 1;
//	numpic = 0;
//}