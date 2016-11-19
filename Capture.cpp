
#include "stdafx.h"
#include "List.h"
#include "Capture.h"

int cvupdateCodeBook(uchar *p, codeBook &c, unsigned *cbBounds, int numChannels)
{
	if (c.numEntries == 0) c.t = 0;
	// �뱾����ԪΪ��ʱ��ʼ��ʱ��Ϊ0  
	c.t += 1;   // Record learning event  
				// ÿ����һ�μ�һ,��ÿһ֡ͼ���һ  

				//SET HIGH AND LOW BOUNDS  
	int n;
	unsigned int high[3], low[3];
	for (n = 0; n < numChannels; n++)
	{
		high[n] = *(p + n) + *(cbBounds + n);
		// *(p+n) �� p[n] ����ȼ�,������*(p+n) �ٶȸ���  
		if (high[n] > 255) high[n] = 255;
		low[n] = *(p + n) - *(cbBounds + n);
		if (low[n] < 0) low[n] = 0;
		// ��p ��ָ����ͨ������,�Ӽ�cbBonds����ֵ,��Ϊ�����ط�ֵ��������  
	}

	//SEE IF THIS FITS AN EXISTING CODEWORD  
	int matchChannel;
	int i;
	for (i = 0; i < c.numEntries; i++)
	{
		// �������뱾ÿ����Ԫ,����p�����Ƿ���������֮һ  
		matchChannel = 0;
		for (n = 0; n < numChannels; n++)
			//����ÿ��ͨ��  
		{
			if ((c.cb[i]->learnLow[n] <= *(p + n)) && (*(p + n) <= c.cb[i]->learnHigh[n])) //Found an entry for this channel  
																						   // ���p ����ͨ�������ڸ���Ԫ��ֵ������֮��  
			{
				matchChannel++;
			}
		}
		if (matchChannel == numChannels)        // If an entry was found over all channels  
												// ���p ���ظ�ͨ����������������  
		{
			c.cb[i]->t_last_update = c.t;
			// ���¸���Ԫʱ��Ϊ��ǰʱ��  
			// adjust this codeword for the first channel  
			for (n = 0; n < numChannels; n++)
				//��������Ԫ��ͨ�������Сֵ  
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
							// p ���ز�������뱾���κ�һ����Ԫ,���洴��һ������Ԫ  
	{
		code_element **foo = new code_element*[c.numEntries + 1];
		// ����c.numEntries+1 ��ָ����Ԫ��ָ��  
		for (int ii = 0; ii < c.numEntries; ii++)
			// ��ǰc.numEntries ��ָ��ָ���Ѵ��ڵ�ÿ����Ԫ  
			foo[ii] = c.cb[ii];

		foo[c.numEntries] = new code_element;
		// ����һ���µ���Ԫ  
		if (c.numEntries) delete[] c.cb;
		// ɾ��c.cb ָ������  
		c.cb = foo;
		// ��foo ͷָ�븳��c.cb  
		for (n = 0; n < numChannels; n++)
			// ��������Ԫ��ͨ������  
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
		// �������Ԫ�Ĳ�����ʱ��  
		if (c.cb[s]->stale < negRun)
			c.cb[s]->stale = negRun;
	}

	// SLOWLY ADJUST LEARNING BOUNDS  
	for (n = 0; n < numChannels; n++)
		// �������ͨ�������ڸߵͷ�ֵ��Χ��,������Ԫ��ֵ֮��,������������Ԫѧϰ����  
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
	// ���沽��ͱ���ѧϰ�в�����Ԫ���һ��  
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
		// p���ظ�ͨ��ֵ�����뱾������һ����Ԫ,�򷵻ذ�ɫ  
		return(255);

	return(0);
}


int cvclearStaleEntries(codeBook &c)
{
	int staleThresh = c.t >> 1;           // �趨ˢ��ʱ��  
	int *keep = new int[c.numEntries]; // ����һ���������  
	int keepCnt = 0;                    // ��¼��ɾ����Ԫ��Ŀ  
										//SEE WHICH CODEBOOK ENTRIES ARE TOO STALE  
	for (int i = 0; i<c.numEntries; i++)
		// �����뱾��ÿ����Ԫ  
	{
		if (c.cb[i]->stale > staleThresh)
			// ����Ԫ�еĲ�����ʱ������趨��ˢ��ʱ��,����Ϊɾ��  
			keep[i] = 0; //Mark for destruction  
		else
		{
			keep[i] = 1; //Mark to keep  
			keepCnt += 1;
		}
	}

	// KEEP ONLY THE GOOD  
	c.t = 0;                        //Full reset on stale tracking  
									// �뱾ʱ������  
	code_element **foo = new code_element*[keepCnt];
	// �����СΪkeepCnt ����Ԫָ������  
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
	// ��foo ͷָ���ַ����c.cb   
	int numCleared = c.numEntries - keepCnt;
	// ���������Ԫ����  
	c.numEntries = keepCnt;
	// ʣ�����Ԫ��ַ  
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

CvSeq* GetRelContours(IplImage* src_img) //ȥ������Ҫ����
{
	CvSeq* src_seq = GetContours(src_img);
	CvSeq* rel_seq = NULL;
	CvSeq* tem_seq = src_seq;
	CvSeq* rel_rel_seq = NULL;
	int num(1);
	if (tem_seq != NULL)
	{
		while (tem_seq != NULL)   //��tem_seq��Ϊ��ʱ�����ܽ����жϸ�ֵ����tem_seq->h_next��Ϊ�գ������һ�����޷���ȡ
		{
			if (fabs(cvContourArea(tem_seq) > 1000))
			{
				rel_seq = tem_seq;
				if (num == 1)
				{
					rel_rel_seq = rel_seq;
				}
				rel_seq = rel_seq->h_next;    //��һ��ִ���꣬rel_seq�����к����������Ҵ�ʱ��rel_seqֵ����������Ҫ���ֵ
				num++;
			}
			tem_seq = tem_seq->h_next;
		}
		if (rel_seq != NULL)
		{
			rel_seq = NULL;              //β�����
		}
		if (rel_rel_seq != NULL)
		{
			rel_rel_seq->h_prev = NULL;  //OK��ͷ�����
		}
	}
	return rel_rel_seq;  //�õ�������ȫ������������CvSeq*
}

bool Keyopen(list<double>* check)
{
	if (check->Length() < 2)
	{
		return false;              //�����õ����ݶ�С��2�ˣ��������ݾͻ�û�ռ���ȫ�ˡ����������һ�㣬����һ������ȷʵ����С��2��
								   //�������ڳ�����Ϊ����û�ռ���ȫ���ͽ��������ݵ�һ�������ˡ���õ�д������������ܻ�ȡ����������
	}                              //�ص㣬�������������С��200ԭ�����Ϊ0�����ݽ�����¼�룡�������ðɣ�����ò�����
	if (check->Getvalue(check->Length() - 1) - check->Getvalue(check->Length()) > 200)  //���������ʼ�����ˣ�˵���������ʼ�뿪�����ռ���ȫ
	{
		double area1 = check->Getvalue(check->Length() - 1);
		double area2 = check->Getvalue(check->Length());
		//�������з���debug��ûʲô�������á�
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
			maxcontour = contours->Getvalue(i);     //�����������һһ��Ӧԭ�򣬵õ��������������������Ǻ��ʵģ�
		}
	}
	return maxcontour;
}

IplImage* GetImageRect(IplImage* orgImage, int num)
{
	//��ͼ��orgImage����ȡһ�飨rectInImage����ͼ��imgRect
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
	//��ͼ��orgImage����ȡһ�飨rectInImage����ͼ��imgRect
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
