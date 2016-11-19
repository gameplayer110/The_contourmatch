#pragma once
#include "stdafx.h"
#include "Capture.h"
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "List.h"

#define CHANNELS 3        
// ���ô����ͼ��ͨ����,Ҫ��С�ڵ���ͼ�����ͨ����  

///////////////////////////////////////////////////////////////////////////  
// ����Ϊ�뱾��Ԫ�����ݽṹ  
// ����ͼ��ʱÿ�����ض�Ӧһ���뱾,ÿ���뱾�п������ɸ���Ԫ  
// ���漰һ��������,ͨ��������һЩ��ֵ�����,��Ҫ����Щ�����Ż�,��ʵ˼·���Ǽ򵥵�  
typedef struct ce {
	uchar   learnHigh[CHANNELS];    // High side threshold for learning  
									// ����Ԫ��ͨ���ķ�ֵ����(ѧϰ����)  
	uchar   learnLow[CHANNELS];     // Low side threshold for learning  
									// ����Ԫ��ͨ���ķ�ֵ����  
									// ѧϰ���������һ�������ظ�ͨ��ֵx[i],���� learnLow[i]<=x[i]<=learnHigh[i],������ؿɺϲ��ڴ���Ԫ  
	uchar   max[CHANNELS];          // High side of box boundary  
									// ���ڴ���Ԫ�������и�ͨ�������ֵ  
	uchar   min[CHANNELS];          // Low side of box boundary  
									// ���ڴ���Ԫ�������и�ͨ������Сֵ  
	int     t_last_update;          // This is book keeping to allow us to kill stale entries  
									// ����Ԫ���һ�θ��µ�ʱ��,ÿһ֡Ϊһ����λʱ��,���ڼ���stale  
	int     stale;                  // max negative run (biggest period of inactivity)  
									// ����Ԫ�������ʱ��,����ɾ���涨ʱ�䲻���µ���Ԫ,�����뱾  
} code_element;                     // ��Ԫ�����ݽṹ  

typedef struct code_book {
	code_element    **cb;
	// ��Ԫ�Ķ�άָ��,���Ϊָ����Ԫָ�������ָ��,ʹ�������Ԫʱ����Ҫ���ظ�����Ԫ,ֻ��Ҫ�򵥵�ָ�븳ֵ����  
	int             numEntries;
	// ���뱾����Ԫ����Ŀ  
	int             t;              // count every access  
									// ���뱾���ڵ�ʱ��,һ֡Ϊһ��ʱ�䵥λ  
} codeBook;                         // �뱾�����ݽṹ  


									///////////////////////////////////////////////////////////////////////////////////  
									// int updateCodeBook(uchar *p, codeBook &c, unsigned cbBounds)  
									// Updates the codebook entry with a new data point  
									//  
									// p            Pointer to a YUV pixel  
									// c            Codebook for this pixel  
									// cbBounds     Learning bounds for codebook (Rule of thumb: 10)  
									// numChannels  Number of color channels we're learning  
									//  
									// NOTES:  
									//      cvBounds must be of size cvBounds[numChannels]  
									//  
									// RETURN  
									//  codebook index  
int cvupdateCodeBook(uchar *p, codeBook &c, unsigned *cbBounds, int numChannels);
uchar cvbackgroundDiff(uchar *p, codeBook &c, int numChannels, int *minMod, int *maxMod);
int cvclearStaleEntries(codeBook &c);
CvSeq * GetAreaMaxContour(CvSeq * contour);
CvSeq * GetContours(IplImage * src_img);
CvSeq * GetRelContours(IplImage * src_img);
bool Keyopen(list<double>* check);
CvSeq * AvailContour(list<double>* check, list<CvSeq*>* contours);
IplImage* GetImageRect(IplImage* orgImage, int num);
IplImage* LiGetImageRect(IplImage* orgImage);