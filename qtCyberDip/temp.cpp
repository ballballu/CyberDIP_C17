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
/////// ȫ�ֱ��� ///
////Mat srcImg;			//ԭʼͼ��
////Mat templImg;		//ģ��ͼ��
////Mat resultImg;		//ƥ����ͼ��
////
////const char* imageWindow = "Source Image";		//ԭʼͼ����ʾ����
////const char* resultWindow = "Result Window";		//ƥ����ͼ����ʾ����
////
////int matchMethod;		//ƥ�䷽��index
////int maxTrackbar = 5;	//��������Χ����ƥ�䷽��������Ӧ��
////
////						/// �������� ///
////void MatchingMethod(int, void*);		//ƥ�亯��
////
////int main(int argc, char** argv)
////{
////	// ����ԭʼͼ���ģ��ͼ��
////	srcImg = imread("2111.png", 1);
////	templImg = imread("mo1.png", 1);
////
////	// ������ʾ����
////	namedWindow(imageWindow, CV_WINDOW_AUTOSIZE);
////	namedWindow(resultWindow, CV_WINDOW_AUTOSIZE);
////
////	// ����������
////	char* trackbarLabel =
////		"Method: \n \
////	  0: SQDIFF \n \
////	  1: SQDIFF NORMED \n \
////	  2: TM CCORR \n \
////	  3: TM CCORR NORMED \n \
////	  4: TM COEFF \n \
////	  5: TM COEFF NORMED";
////	//���������������� ��ʾ�������� ƥ�䷽��index ��������Χ �ص�����
////	createTrackbar(trackbarLabel, imageWindow, &matchMethod, maxTrackbar, MatchingMethod);
////
////	MatchingMethod(0, 0);
////
////	waitKey(0);
////	return 0;
////}
////
/////// �������� ///
////void MatchingMethod(int, void*)		//ƥ�亯��
////{
////	// ���������ʾ
////	Mat displayImg;
////	srcImg.copyTo(displayImg);
////
////	// ����ƥ����ͼ��Ϊÿ��ģ��λ�ô洢ƥ����
////	// ƥ����ͼ���СΪ��(W-w+1)*(H-h+1)
////	int result_cols = srcImg.cols - templImg.cols + 1;
////	int result_rows = srcImg.rows - templImg.rows + 1;
////	resultImg.create(result_cols, result_rows, CV_32FC1);
////
////	// ����ƥ�䲢��һ��
////	matchTemplate(srcImg, templImg, resultImg, matchMethod);
////	normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());
////
////	// ʹ��minMaxLoc�ҳ����ƥ��
////	double minVal, maxVal;
////	Point minLoc, maxLoc, matchLoc;
////	minMaxLoc(resultImg, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
////
////	// ����CV_TM_SQDIFF�� CV_TM_SQDIFF_NORMED�����ַ�������СֵΪ���ƥ�䣻���ڱ�ķ������ֵΪ���ƥ��
////	if (matchMethod == CV_TM_SQDIFF || matchMethod == CV_TM_SQDIFF_NORMED)
////	{
////		matchLoc = minLoc;
////	}
////	else
////	{
////		matchLoc = maxLoc;
////	}
////
////	// ��ԭʼͼ���ƥ����ͼ���������ƥ���Ϊ���ϽǱ�����ƥ���
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
///////// ȫ�ֱ��� ///
//////Mat srcImg;			//ԭʼͼ��
//////Mat templImg;		//ģ��ͼ��
//////Mat resultImg;		//ƥ����ͼ��
//////
//////					/// �������� ///
//////
//////					// ����ԭʼͼ���ģ��ͼ��
//////srcImg = imread("111.png", 1);
//////templImg = imread("mo1.png", 1);
//////
//////// ���������ʾ
//////Mat displayImg;
//////srcImg.copyTo(displayImg);
//////
//////// ����ƥ����ͼ��Ϊÿ��ģ��λ�ô洢ƥ����
//////// ƥ����ͼ���СΪ��(W-w+1)*(H-h+1)
//////int result_cols = srcImg.cols - templImg.cols + 1;
//////int result_rows = srcImg.rows - templImg.rows + 1;
//////resultImg.create(result_cols, result_rows, CV_32FC1);
//////
//////// ����ƥ�䲢��һ��
//////matchTemplate(srcImg, templImg, resultImg, CV_TM_CCOEFF);				// ����CV_TM_SQDIFF�� CV_TM_SQDIFF_NORMED�����ַ�������СֵΪ���ƥ�䣻���ڱ�ķ������ֵΪ���ƥ��
//////normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());
//////
//////// ʹ��minMaxLoc�ҳ����ƥ��
//////double minVal, maxVal;
//////Point minLoc, maxLoc, matchLoc;
//////minMaxLoc(resultImg, &minVal, &maxVal, &minLoc, &maxLoc, Mat());
//////
//////// ��ԭʼͼ���ƥ����ͼ���������ƥ���Ϊ���ϽǱ�����ƥ���
//////rectangle(displayImg, matchLoc, Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows), Scalar::all(0), 2, 8, 0);
//////rectangle(resultImg, matchLoc, Point(matchLoc.x + templImg.cols, matchLoc.y + templImg.rows), Scalar::all(0), 2, 8, 0);
//////
//////imshow("Source Image", displayImg);				//ԭʼͼ����ʾ����
//////imshow("Result Window", resultImg);				//ƥ����ͼ����ʾ����
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
//	//ͼ��ϸ��
//		Mat srcImg2 = imread("2222.png", 1);
//		cv::Mat src;
//		//��ԭͼ��ת��Ϊ��ֵͼ��  
//		cv::threshold(srcImg2, src, 128, 1, cv::THRESH_BINARY);
//	if (src.empty()) return 0;//ͼ��Ϊ�գ�ֱ�ӷ���
//							//cv::threshold(src, src, 128, 1, CV_THRESH_BINARY);//תΪ0��1��ͼ��
//
//	int ImgHeight = src.rows;
//	int ImgWidth = src.cols;
//
//	int count = 0;  //��¼��������
//	while (true)
//	{
//		count++;
//		if ( count > 50) //���ƴ������ҵ�����������
//			break;
//
//		vector<pair<int, int>> mFlag; //���ڱ����Ҫɾ���ĵ�
//									  //�Ե���
//		for (int i = 0; i < ImgHeight; ++i)
//		{
//			for (int j = 0; j < ImgWidth; ++j)
//			{
//				//��������ĸ����������б��
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
//								//���
//								mFlag.push_back(make_pair(i, j));
//							}
//						}
//					}
//				}
//			}
//		}
//		//����ǵĵ�ɾ��
//		for (vector<pair<int, int> >::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
//		{
//			src.at
//
//				<uchar>(i->first, i->second) = 0;
//		}
//
//		//ֱ��û�е����㣬�㷨����
//		if (mFlag.size() == 0) break;
//		else mFlag.clear();//��mFlag���
//						   //cv::threshold(src, src, 0, 255, CV_THRESH_BINARY);//��ֵ��ͼ��
//	}
//	imshow("qu", src);
//	return 0;
//}



//n = n + 1;
//cvtColor(holdimg, holdimggrey, CV_BGR2GRAY);  //ת��Ϊ�Ҷ�ͼ�񲢱���,���ڼ�С�����������е���������ֻ��һ��ͨ���ˣ�
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
//	////ͼ��ϸ��
//	//Mat srcImg2 = imread("2222.png", 1);
//	//Mat out;
//	////��ԭͼ��ת��Ϊ��ֵͼ��  
//	//cv::threshold(srcImg2, srcImg2, 128, 1, cv::THRESH_BINARY);
//
//	//out = thinImage(srcImg2);
//	//imshow("qu", out);
//
//	////////////////////////////////////////////////2017.12.13�Ƚ�����ͼƬ
//
//
//	//mat1 �� mat2 ���бȽ�   
//	//mat2 �� mat3 ���бȽ�
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
//	//	//�������ͼƬ�����  ������Ӧ�Ĵ���  �������coutģ����
//	//	cout << "��ͬ��" << endl;
//	//}
//	//else
//	//{
//	//	//�������ͼƬ���   ������Ӧ�Ĵ���  �������coutģ����
//	//	cout << "��ͬ��" << endl;
//	//}
//}



//2017.12.14ɾȥ�Ĵ��� �����ű��
//����ģ��λ�ã�ʹ�����srcImg
/*
for (int i = 0; i < N; i++)			//i�Ǻ���� ����loc.x
{
for (int j = 0; j < N; j++)		//j������� ����loc.y
{
for (int z = 0; z < 9; z++)  //��ͬƥ���� z<9ʱ�൱��ƥ���ʴ�0.9��0.1��9�Σ�
{
pipeilv = 0.9 - z / 10.0;
templen = cvFloor((imagelen * pipeilv) / N);     //����ģ���С,cvFloor����ȡ��  (N=3ʱģ���СΪ43 N=4��ģ���СΪ32)
loc.x = cvFloor(((i + (1 - pipeilv) / 2) / N)*imagelen) + 46;
loc.y = cvFloor(((j + (1 - pipeilv) / 2) / N)*imagelen) + 297;

Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));          //ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
imwrite("moban.png", mobanpre);
Mat moban = imread("moban.png", 1);

ptloc = MatchingMethod(check, moban);  //���ģ����check(����holdimg)�����ƥ����λ������

cenlocx[z] = ptloc.x + templen / 2;
cenlocy[z] = ptloc.y + templen / 2;

rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //��holding�ϻ���
imshow("ƥ��ͼ", check);
rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���
imshow("ģ��ͼ", srcImg);
}
meanx = (cenlocx[0] + cenlocx[1] + cenlocx[2] + cenlocx[3] + cenlocx[4] + cenlocx[5] + cenlocx[6] + cenlocx[7] + cenlocx[8])/9.0;  //ijͼ���ڲ�ͬƥ������ptloc.x��ƽ��ֵ
meany = (cenlocy[0] + cenlocy[1] + cenlocy[2] + cenlocy[3] + cenlocy[4] + cenlocy[5] + cenlocy[6] + cenlocy[7] + cenlocy[8])/9.0;

errorx = (cenlocx[0] * cenlocx[0] + cenlocx[1] * cenlocx[0] + cenlocx[2] * cenlocx[2] + cenlocx[3] * cenlocx[3] + cenlocx[4] * cenlocx[4] + cenlocx[5] * cenlocx[5] + cenlocx[6] * cenlocx[6] + cenlocx[7] * cenlocx[7] + cenlocx[8] * cenlocx[8]) / 9.0 - meanx*meanx;  //�󷽲�
errory = (cenlocy[0] * cenlocy[0] + cenlocy[1] * cenlocy[0] + cenlocy[2] * cenlocy[2] + cenlocy[3] * cenlocy[3] + cenlocy[4] * cenlocy[4] + cenlocy[5] * cenlocy[5] + cenlocy[6] * cenlocy[6] + cenlocy[7] * cenlocy[7] + cenlocy[8] * cenlocy[8]) / 9.0 - meany*meany;  //�󷽲�

cout << meanx << endl;
cout << meany << endl << endl;
cout << errorx << endl;
cout << errory << endl << endl;

rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //��holding�ϻ���
imshow("ƥ��ͼ", check);
rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���
imshow("ģ��ͼ", srcImg);

cvWaitKey(0);
}
}*/





//2017.12.14ɾ���ĵڶ�������
/*		drawloc.x = cvFloor(((1 + 0.2) / N)*imagelen) + 46;
		drawloc.y = cvFloor(((1 + 0.2) / N)*imagelen) + 297;
		rectangle(check3, drawloc, Point(drawloc.x + templen, drawloc.y + templen), Scalar::all(0), 2, 8, 0);
		loc.x = drawloc.x;
		loc.y = drawloc.y;

		Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));          //ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
		imwrite("moban.png", mobanpre);

		Mat moban = imread("moban.png", 1);

		ptloc = MatchingMethod(check, moban);  //���ģ����check(����holdimg)�����ƥ����λ������

		rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //��holding�ϻ���
		imshow("ƥ��ͼ", check);
		rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���
		imshow("ģ��ͼ", srcImg);

		cvWaitKey(0);
*/	



/*
loc.x = 200;
loc.y = 340;
Mat mobanpre = srcImg(Rect(loc.x, loc.y, templen, templen));          //ģ���Ǵ�srcImg�Ͻ�ͼ ����srcImg
imwrite("moban.png", mobanpre);

Mat moban = imread("moban.png", 1);

ptloc = MatchingMethod(check, moban);  //���ģ����check(����holdimg)�����ƥ����λ������

rectangle(check, ptloc, Point(ptloc.x + templen, ptloc.y + templen), Scalar::all(0), 2, 8, 0);   //��holding�ϻ���
imshow("ƥ��ͼ", check);
rectangle(srcImg, loc, Point(loc.x + templen, loc.y + templen), Scalar::all(0), 2, 8, 0);   //��srcImg�ϻ���,�����ģ��Ĳ���
imshow("ģ��ͼ", srcImg);
*/

//if ((numpic < pinumber - 1) && (eva[numpic] == 0))
//{
//	cout << "����" << numpic + 1 << endl;
//	numpic = numpic + 1;
//}
//else if ((numpic < pinumber - 1) && (eva[numpic] == 1))
//{
//	cout << "����" << numpic + 1 << endl;
//	eva[numpic] = superEagleEye(img, srcImg, numpic);
//	numpic = numpic + 1;    //ͼ���ż�1 ����һ��ͼ����д���
//}
//else
//{
//	circletime = circletime + 1;
//	numpic = 0;
//}