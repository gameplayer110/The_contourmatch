// Text.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//cout <<"ni�ð�" <<100 << endl;
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
	list<double>* check = new list<double>;   //����������б�
	list<CvSeq*>* seqlist = new list<CvSeq*>;
	int         imageLen;
	int         nChannels = CHANNELS;
	int         min1Mod[CHANNELS];
	int         max1Mod[CHANNELS];
	int         time(40);//��ʱ
	int value[16] = { 0,0,
		1,1 };
	int cols = 2; int rows = 2; int X = 1; int Y = 1;
	IplConvKernel* element = cvCreateStructuringElementEx(cols, rows, X, Y, CV_SHAPE_CUSTOM, value);

	//////////////////////////////////////////////////////////////////////////  
	// ��ʼ��������  
	//GetDlgItem(IDC_EDIT_DI2)->SetWindowText(YB1);
	//GetDlgItem(IDC_EDIT_DI3)->SetWindowText(YB2);
	cvNamedWindow("Raw");
	cvNamedWindow("CodeBook");
	cvNamedWindow("text1");
	cvNamedWindow("text2");

	capture = cvCreateCameraCapture(0);
	if (!capture)
	{
		/*CString a = "������������ͷඣ���~";
		MessageBox(a);
		return;*/
	}

	rawImage = cvQueryFrame(capture);
	yuvImage = cvCreateImage(cvGetSize(rawImage), 8, 3);
	// ��yuvImage ����һ����rawImage �ߴ���ͬ,8λ3ͨ��ͼ��  
	ImaskCodeBook = cvCreateImage(cvGetSize(rawImage), IPL_DEPTH_8U, 1);
	// ΪImaskCodeBook ����һ����rawImage �ߴ���ͬ,8λ��ͨ��ͼ��  
	cvSet(ImaskCodeBook, cvScalar(255));
	// ���õ�ͨ����������Ԫ��Ϊ255,����ʼ��Ϊ��ɫͼ��  

	imageLen = rawImage->width * rawImage->height;
	cB = new codeBook[imageLen];
	// �õ���ͼ��������Ŀ����һ����һ���뱾,�Ա��ÿ�����ؽ��д���  

	for (int i = 0; i < imageLen; i++)
		// ��ʼ��ÿ����Ԫ��ĿΪ0  
		cB[i].numEntries = 0;
	for (int i = 0; i < nChannels; i++)
	{
		cbBounds[i] = 10;   // ����ȷ����Ԫ��ͨ���ķ�ֵ  

		min1Mod[i] = 20;   // ���ڱ�����ֺ�����  
		max1Mod[i] = 20;   // ������ֵ�Դﵽ��õķָ�  
	}


	//////////////////////////////////////////////////////////////////////////  
	// ��ʼ������Ƶÿһ֡ͼ��  
	for (int i = 0;; i++)
	{
		cvCvtColor(rawImage, yuvImage, CV_BGR2YCrCb);
		// ɫ�ʿռ�ת��,��rawImage ת����YUVɫ�ʿռ�,�����yuvImage  
		// ��ʹ��ת��Ч����Ȼ�ܺ�  
		// yuvImage = cvCloneImage(rawImage);  

		if (i <= 30)
			// 30֡�ڽ��б���ѧϰ  
		{
			pColor = (uchar *)(yuvImage->imageData);
			// ָ��yuvImage ͼ���ͨ������  
			for (int c = 0; c < imageLen; c++)
			{
				cvupdateCodeBook(pColor, cB[c], cbBounds, nChannels);
				// ��ÿ������,���ô˺���,��׽��������ر仯ͼ��  
				pColor += 3;
				// 3 ͨ��ͼ��, ָ����һ������ͨ������  
			}
			if (i == 30)
				// ��30 ֡ʱ�������溯��,ɾ���뱾�г¾ɵ���Ԫ  
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
																  // ָ��ImaskCodeBook ͨ���������е���Ԫ��  
			for (int c = 0; c < imageLen; c++)
			{
				maskPixelCodeBook = cvbackgroundDiff(pColor, cB[c], nChannels, min1Mod, max1Mod);
				// �ҿ������ʱ��Ȼ����,��ʼ�����codeBook �Ǻ�  
				*pMask++ = maskPixelCodeBook;
				pColor += 3;
				// pColor ָ�����3ͨ��ͼ��  
			}
			for (int j = 0; j < 10; j++)
			{
				cvMorphologyEx(ImaskCodeBook, ImaskCodeBook, ImaskCodeBook, element, CV_MOP_OPEN, 1);
				cvMorphologyEx(ImaskCodeBook, ImaskCodeBook, ImaskCodeBook, element, CV_MOP_CLOSE, 1);
			}
			while (i == 1 * time)
			{
				textImage = LiGetImageRect(ImaskCodeBook);     //��ͼƬ�Ĵ�С�������вü�
				textImage1 = GetImageRect(textImage, 1);
				textImage2 = GetImageRect(textImage, 2);

				CvSeq* availseq(NULL);
				CvSeq* rel_seq = GetRelContours(ImaskCodeBook);
				CvSeq* maxseq = GetAreaMaxContour(rel_seq);
				
				if (maxseq != NULL)
				{
					double maxarea = cvContourArea(maxseq);
					check->InsertLast(maxarea);                //��ģ��ǵ��Ǵ�β�����룬֮ǰд��ͷ�����룬���GG��
					seqlist->InsertLast(rel_seq);
				}
				if (Keyopen(check))
				{
					availseq = AvailContour(check, seqlist);
				}
				if (availseq != NULL)
				{ 
					ContourMatch match(availseq);            //ȡ����ӵ������������������У������ǵ�һ��������������
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

