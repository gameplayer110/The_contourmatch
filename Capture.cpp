
#include "stdafx.h"
#include "List.h"
#include "Capture.h"

int cvupdateCodeBook(uchar *p, codeBook &c, unsigned *cbBounds, int numChannels)
{
	if (c.numEntries == 0) c.t = 0;
	// 码本中码元为零时初始化时间为0  
	c.t += 1;   // Record learning event  
				// 每调用一次加一,即每一帧图像加一  

				//SET HIGH AND LOW BOUNDS  
	int n;
	unsigned int high[3], low[3];
	for (n = 0; n < numChannels; n++)
	{
		high[n] = *(p + n) + *(cbBounds + n);
		// *(p+n) 和 p[n] 结果等价,经试验*(p+n) 速度更快  
		if (high[n] > 255) high[n] = 255;
		low[n] = *(p + n) - *(cbBounds + n);
		if (low[n] < 0) low[n] = 0;
		// 用p 所指像素通道数据,加减cbBonds中数值,作为此像素阀值的上下限  
	}

	//SEE IF THIS FITS AN EXISTING CODEWORD  
	int matchChannel;
	int i;
	for (i = 0; i < c.numEntries; i++)
	{
		// 遍历此码本每个码元,测试p像素是否满足其中之一  
		matchChannel = 0;
		for (n = 0; n < numChannels; n++)
			//遍历每个通道  
		{
			if ((c.cb[i]->learnLow[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->learnHigh[n])) //Found an entry for this channel  
																						   // 如果p 像素通道数据在该码元阀值上下限之间  
			{
				matchChannel++;
			}
		}
		if (matchChannel == numChannels)        // If an entry was found over all channels  
												// 如果p 像素各通道都满足上面条件  
		{
			c.cb[i]->t_last_update = c.t;
			// 更新该码元时间为当前时间  
			// adjust this codeword for the first channel  
			for (n = 0; n < numChannels; n++)
				//调整该码元各通道最大最小值  
			{
				if (c.cb[i]->max[n] < *(p + n))
					c.cb[i]->max[n] = *(p + n);
				else if (c.cb[i]->min[n] > *(p + n))
					c.cb[i]->min[n] = *(p + n);
			}
			break;
		}
	}

	// ENTER A NEW CODE WORD IF NEEDED  
	if (i == c.numEntries)  // No existing code word found, make a new one  
							// p 像素不满足此码本中任何一个码元,下面创建一个新码元  
	{
		code_element **foo = new code_element*[c.numEntries + 1];
		// 申请c.numEntries+1 个指向码元的指针  
		for (int ii = 0; ii < c.numEntries; ii++)
			// 将前c.numEntries 个指针指向已存在的每个码元  
			foo[ii] = c.cb[ii];

		foo[c.numEntries] = new code_element;
		// 申请一个新的码元  
		if (c.numEntries) delete[] c.cb;
		// 删除c.cb 指针数组  
		c.cb = foo;
		// 把foo 头指针赋给c.cb  
		for (n = 0; n < numChannels; n++)
			// 更新新码元各通道数据  
		{
			c.cb[c.numEntries]->learnHigh[n] = high[n];
			c.cb[c.numEntries]->learnLow[n] = low[n];
			c.cb[c.numEntries]->max[n] = *(p + n);
			c.cb[c.numEntries]->min[n] = *(p + n);
		}
		c.cb[c.numEntries]->t_last_update = c.t;
		c.cb[c.numEntries]->stale = 0;
		c.numEntries += 1;
	}

	// OVERHEAD TO TRACK POTENTIAL STALE ENTRIES  
	for (int s = 0; s < c.numEntries; s++)
	{
		// This garbage is to track which codebook entries are going stale  
		int negRun = c.t - c.cb[s]->t_last_update;
		// 计算该码元的不更新时间  
		if (c.cb[s]->stale < negRun)
			c.cb[s]->stale = negRun;
	}

	// SLOWLY ADJUST LEARNING BOUNDS  
	for (n = 0; n < numChannels; n++)
		// 如果像素通道数据在高低阀值范围内,但在码元阀值之外,则缓慢调整此码元学习界限  
	{
		if (c.cb[i]->learnHigh[n] < high[n])
			c.cb[i]->learnHigh[n] += 1;
		if (c.cb[i]->learnLow[n] > low[n])
			c.cb[i]->learnLow[n] -= 1;
	}

	return(i);
}

uchar cvbackgroundDiff(uchar *p, codeBook &c, int numChannels, int *minMod, int *maxMod)
{
	// 下面步骤和背景学习中查找码元如出一辙  
	int matchChannel;
	//SEE IF THIS FITS AN EXISTING CODEWORD  
	int i;
	for (i = 0; i < c.numEntries; i++)
	{
		matchChannel = 0;
		for (int n = 0; n < numChannels; n++)
		{
			if ((c.cb[i]->min[n] - minMod[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->max[n] + maxMod[n]))
				matchChannel++; //Found an entry for this channel  
			else
				break;
		}
		if (matchChannel == numChannels)
			break; //Found an entry that matched all channels  
	}
	if (i == c.numEntries)
		// p像素各通道值满足码本中其中一个码元,则返回白色  
		return(255);

	return(0);
}


int cvclearStaleEntries(codeBook &c)
{
	int staleThresh = c.t >> 1;           // 设定刷新时间  
	int *keep = new int[c.numEntries]; // 申请一个标记数组  
	int keepCnt = 0;                    // 记录不删除码元数目  
										//SEE WHICH CODEBOOK ENTRIES ARE TOO STALE  
	for (int i = 0; i<c.numEntries; i++)
		// 遍历码本中每个码元  
	{
		if (c.cb[i]->stale > staleThresh)
			// 如码元中的不更新时间大于设定的刷新时间,则标记为删除  
			keep[i] = 0; //Mark for destruction  
		else
		{
			keep[i] = 1; //Mark to keep  
			keepCnt += 1;
		}
	}

	// KEEP ONLY THE GOOD  
	c.t = 0;                        //Full reset on stale tracking  
									// 码本时间清零  
	code_element **foo = new code_element*[keepCnt];
	// 申请大小为keepCnt 的码元指针数组  
	int k = 0;
	for (int ii = 0; ii < c.numEntries; ii++)
	{
		if (keep[ii])
		{
			foo[k] = c.cb[ii];
			foo[k]->stale = 0;       //We have to refresh these entries for next clearStale  
			foo[k]->t_last_update = 0;
			k++;
		}
	}
	//CLEAN UP  
	delete[] keep;
	delete[] c.cb;
	c.cb = foo;
	// 把foo 头指针地址赋给c.cb   
	int numCleared = c.numEntries - keepCnt;
	// 被清理的码元个数  
	c.numEntries = keepCnt;
	// 剩余的码元地址  
	return(numCleared);
}


CvSeq *GetAreaMaxContour(CvSeq *contour)
{
	double tem_area(0), max_area(0);
	CvSeq* tem_seq(contour);
	CvSeq* max_seq(NULL);
	for (; tem_seq != NULL; tem_seq = tem_seq->h_next)
	{
		tem_area = fabs(cvContourArea(tem_seq));
		if (tem_area > max_area)
		{
			max_area = tem_area;
			max_seq = tem_seq;
		}
	}
	return max_seq;
}

CvSeq* GetContours(IplImage* src_img)
{
	CvMemStorage* storage1(cvCreateMemStorage());
	CvSeq* first_contour1;
	cvFindContours(
		src_img,
		storage1,
		&first_contour1,
		sizeof(CvContour),
		CV_RETR_LIST
		);
	//cvReleaseMemStorage(&storage1);

	return first_contour1;
}

CvSeq* GetRelContours(IplImage* src_img) //去除不必要轮廓
{
	CvSeq* src_seq = GetContours(src_img);
	CvSeq* rel_seq = NULL;
	CvSeq* tem_seq = src_seq;
	CvSeq* rel_rel_seq = NULL;
	int num(1);
	if (tem_seq != NULL)
	{
		while (tem_seq != NULL)   //当tem_seq不为空时，才能进行判断赋值。当tem_seq->h_next不为空，则最后一个数无法提取
		{
			if (fabs(cvContourArea(tem_seq) > 1000))
			{
				rel_seq = tem_seq;
				if (num == 1)
				{
					rel_rel_seq = rel_seq;
				}
				rel_seq = rel_seq->h_next;    //这一步执行完，rel_seq可能有后续轮廓，且此时的rel_seq值不是所符合要求的值
				num++;
			}
			tem_seq = tem_seq->h_next;
		}
		if (rel_seq != NULL)
		{
			rel_seq = NULL;              //尾部清空
		}
		if (rel_rel_seq != NULL)
		{
			rel_rel_seq->h_prev = NULL;  //OK，头部清空
		}
	}
	return rel_rel_seq;  //得到包含的全是所需轮廓的CvSeq*
}

bool Keyopen(list<double>* check)
{
	if (check->Length() < 2)
	{
		return false;              //如果获得的数据都小于2了，基本数据就还没收集完全了。但得提防有一点，就是一组数据确实都是小于2的
								   //，但由于程序以为数据没收集完全，就将几组数据当一组数据了。最好得写个代码测试下能获取多少组数据
	}                              //重点，根据轮廓面积不小于200原则，面积为0的数据将不会录入！！！！好吧，必须得测试了
	if (check->Getvalue(check->Length() - 1) - check->Getvalue(check->Length()) > 200)  //轮廓面积开始减少了，说明零件经开始离开数据收集完全
	{
		double area1 = check->Getvalue(check->Length() - 1);
		double area2 = check->Getvalue(check->Length());
		//上面两行方便debug，没什么其他作用。
		return true;
	}
	return false;
}

CvSeq* AvailContour(list<double>* check, list<CvSeq*>* contours)
{
	double maxarea(0);
	double temarea(0);
	CvSeq* maxcontour(0);
	for (int i = 1; i <= check->Length(); i++)
	{
		temarea = check->Getvalue(i);
		if (temarea > maxarea)
		{
			maxarea = temarea;
			maxcontour = contours->Getvalue(i);     //根据主程序的一一对应原则，得到最大面积的轮廓（基本是合适的）
		}
	}
	return maxcontour;
}

IplImage* GetImageRect(IplImage* orgImage, int num)
{
	//从图像orgImage中提取一块（rectInImage）子图像imgRect
	IplImage *result = cvCreateImage(cvSize(orgImage->width / 2, orgImage->height), IPL_DEPTH_8U, 1);
	CvRect text;
	if (num == 1)
	{
		text = cvRect(0, 0, orgImage->width / 2, orgImage->height);
	}
	else
	{
		text.x = orgImage->width / 2;
		text.y = 0;
		text.height = orgImage->height;
		text.width = orgImage->width / 2;
	}

	cvSetImageROI(orgImage, text);
	cvCopy(orgImage, result, 0);
	cvResetImageROI(orgImage);

	return result;
}

IplImage* LiGetImageRect(IplImage* orgImage)
{
	//从图像orgImage中提取一块（rectInImage）子图像imgRect
	IplImage *result = cvCreateImage(cvSize(orgImage->width - 120, 300), IPL_DEPTH_8U, 1);
	CvRect text;
	text.x = 90;
	text.y = 20;
	text.height = 300;
	text.width = orgImage->width - 120;
	cvSetImageROI(orgImage, text);
	cvCopy(orgImage, result, 0);
	cvResetImageROI(orgImage);

	return result;
}
