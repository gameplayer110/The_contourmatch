#pragma once
#include "stdafx.h"
#include "Capture.h"
#include "cv.h"
#include "highgui.h"
#include "cxcore.h"
#include "List.h"

#define CHANNELS 3        
// 设置处理的图像通道数,要求小于等于图像本身的通道数  

///////////////////////////////////////////////////////////////////////////  
// 下面为码本码元的数据结构  
// 处理图像时每个像素对应一个码本,每个码本中可有若干个码元  
// 当涉及一个新领域,通常会遇到一些奇怪的名词,不要被这些名词吓坏,其实思路都是简单的  
typedef struct ce {
	uchar   learnHigh[CHANNELS];    // High side threshold for learning  
									// 此码元各通道的阀值上限(学习界限)  
	uchar   learnLow[CHANNELS];     // Low side threshold for learning  
									// 此码元各通道的阀值下限  
									// 学习过程中如果一个新像素各通道值x[i],均有 learnLow[i]<=x[i]<=learnHigh[i],则该像素可合并于此码元  
	uchar   max[CHANNELS];          // High side of box boundary  
									// 属于此码元的像素中各通道的最大值  
	uchar   min[CHANNELS];          // Low side of box boundary  
									// 属于此码元的像素中各通道的最小值  
	int     t_last_update;          // This is book keeping to allow us to kill stale entries  
									// 此码元最后一次更新的时间,每一帧为一个单位时间,用于计算stale  
	int     stale;                  // max negative run (biggest period of inactivity)  
									// 此码元最长不更新时间,用于删除规定时间不更新的码元,精简码本  
} code_element;                     // 码元的数据结构  

typedef struct code_book {
	code_element    **cb;
	// 码元的二维指针,理解为指向码元指针数组的指针,使得添加码元时不需要来回复制码元,只需要简单的指针赋值即可  
	int             numEntries;
	// 此码本中码元的数目  
	int             t;              // count every access  
									// 此码本现在的时间,一帧为一个时间单位  
} codeBook;                         // 码本的数据结构  


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