// Text.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
#include "List.h"
#include "Capture.h"
#include "ContourMatch.h"
#include "windows.h"
#include "MP441.h"
#include <string>
#pragma comment(lib, "MP441.lib")
using namespace std;

#define hDevice INVALID_HANDLE_VALUE

int main()
{
	//int i;
	//cout <<"ni好啊" <<100 << endl;
	//cin >> i;
	//list<int>* lis=new list<int>();
	//lis->Create();
	//lis->Getvalue(4);
	//lis->Clear();
	//cout << lis->Length()<<endl;
	CvCapture*  capture;
	IplImage*   rawImage;
	IplImage*   yuvImage;
	IplImage*   textImage1;
	IplImage*   textImage2;
	IplImage*   ImaskCodeBook;
	IplImage*   textImage;
	codeBook*   cB;
	unsigned    cbBounds[CHANNELS];
	uchar*      pColor; //YUV pointer  
	list<double>* check = new list<double>;   //最大轮廓的列表
	list<CvSeq*>* seqlist = new list<CvSeq*>;
	int         imageLen;
	int         nChannels = CHANNELS;
	int         min1Mod[CHANNELS];
	int         max1Mod[CHANNELS];
	int         time(40);//延时
	int value[16] = { 0,0,
		1,1 };
	int cols = 2; int rows = 2; int X = 1; int Y = 1;
	IplConvKernel* element = cvCreateStructuringElementEx(cols, rows, X, Y, CV_SHAPE_CUSTOM, value);

	//////////////////////////////////////////////////////////////////////////  
	// 初始化各变量  
	//GetDlgItem(IDC_EDIT_DI2)->SetWindowText(YB1);
	//GetDlgItem(IDC_EDIT_DI3)->SetWindowText(YB2);
	cvNamedWindow("Raw");
	cvNamedWindow("CodeBook");
	cvNamedWindow("text1");
	cvNamedWindow("text2");

	capture = cvCreateCameraCapture(0);
	if (!capture)
	{
		/*CString a = "忘记连接摄像头喽，亲~";
		MessageBox(a);
		return;*/
	}

	rawImage = cvQueryFrame(capture);
	yuvImage = cvCreateImage(cvGetSize(rawImage), 8, 3);
	// 给yuvImage 分配一个和rawImage 尺寸相同,8位3通道图像  
	ImaskCodeBook = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
	// 为ImaskCodeBook 分配一个和rawImage 尺寸相同,8位单通道图像  
	cvSet(ImaskCodeBook, cvScalar(255));
	// 设置单通道数组所有元素为255,即初始化为白色图像  

	imageLen = rawImage->width * rawImage->height;
	cB = new codeBook[imageLen];
	// 得到与图像像素数目长度一样的一组码本,以便对每个像素进行处理  

	for (int i = 0; i < imageLen; i++)
		// 初始化每个码元数目为0  
		cB[i].numEntries = 0;
	for (int i = 0; i < nChannels; i++)
	{
		cbBounds[i] = 10;   // 用于确定码元各通道的阀值  

		min1Mod[i] = 20;   // 用于背景差分函数中  
		max1Mod[i] = 20;   // 调整其值以达到最好的分割  
	}


	//////////////////////////////////////////////////////////////////////////  
	// 开始处理视频每一帧图像  
	for (int i = 0;; i++)
	{
		cvCvtColor(rawImage, yuvImage, CV_BGR2YCrCb);
		// 色彩空间转换,将rawImage 转换到YUV色彩空间,输出到yuvImage  
		// 即使不转换效果依然很好  
		// yuvImage = cvCloneImage(rawImage);  

		if (i <= 30)
			// 30帧内进行背景学习  
		{
			pColor = (uchar *)(yuvImage->imageData);
			// 指向yuvImage 图像的通道数据  
			for (int c = 0; c < imageLen; c++)
			{
				cvupdateCodeBook(pColor, cB[c], cbBounds, nChannels);
				// 对每个像素,调用此函数,捕捉背景中相关变化图像  
				pColor += 3;
				// 3 通道图像, 指向下一个像素通道数据  
			}
			if (i == 30)
				// 到30 帧时调用下面函数,删除码本中陈旧的码元  
			{
				for (int c = 0; c < imageLen; c++)
					cvclearStaleEntries(cB[c]);
			}
		}
		else
		{
			uchar maskPixelCodeBook;
			pColor = (uchar *)((yuvImage)->imageData); //3 channel yuv image  
			uchar *pMask = (uchar *)((ImaskCodeBook)->imageData); //1 channel image  
																  // 指向ImaskCodeBook 通道数据序列的首元素  
			for (int c = 0; c < imageLen; c++)
			{
				maskPixelCodeBook = cvbackgroundDiff(pColor, cB[c], nChannels, min1Mod, max1Mod);
				// 我看到这儿时豁然开朗,开始理解了codeBook 呵呵  
				*pMask++ = maskPixelCodeBook;
				pColor += 3;
				// pColor 指向的是3通道图像  
			}
			for (int j = 0; j < 10; j++)
			{
				cvMorphologyEx(ImaskCodeBook, ImaskCodeBook, ImaskCodeBook, element, CV_MOP_OPEN, 1);
				cvMorphologyEx(ImaskCodeBook, ImaskCodeBook, ImaskCodeBook, element, CV_MOP_CLOSE, 1);
			}
			while (i == 1 * time)
			{
				textImage = LiGetImageRect(ImaskCodeBook);     //对图片的大小数量进行裁剪
				textImage1 = GetImageRect(textImage, 1);
				textImage2 = GetImageRect(textImage, 2);

				CvSeq* availseq(NULL);
				CvSeq* rel_seq = GetRelContours(ImaskCodeBook);
				CvSeq* maxseq = GetAreaMaxContour(rel_seq);
				
				if (maxseq != NULL)
				{
					double maxarea = cvContourArea(maxseq);
					check->InsertLast(maxarea);                //妈的，记得是从尾部插入，之前写成头部插入，差点GG了
					seqlist->InsertLast(rel_seq);
				}
				if (Keyopen(check))
				{
					availseq = AvailContour(check, seqlist);
				}
				if (availseq != NULL)
				{ 
					ContourMatch match(availseq);            //取得是拥有最大轮廓的轮廓序列，并不是第一个轮廓就是最大的
					match.Match();
					//cvShowImage("text1", textImage1);
					//cvShowImage("text2", textImage2);
					check->Clear();
					seqlist->Clear();
				}
				time++;
			}
		}
		if (!(rawImage = cvQueryFrame(capture)))
			break;
		cvShowImage("Raw", rawImage);
		cvShowImage("CodeBook", ImaskCodeBook);


		if (cvWaitKey(30) == 27)
			break;
		if (i == 56 /*|| i == 63*/)
			cvWaitKey();

	}




	cvReleaseCapture(&capture);
	if (yuvImage)
		cvReleaseImage(&yuvImage);
	if (ImaskCodeBook)
		cvReleaseImage(&ImaskCodeBook);
	cvDestroyAllWindows();
	delete[] cB;
	return 0;
}

