#pragma once
#include "cv.h"
#include "highgui.h"
#define BOLT 1
#define SPACER 2
#define NUT 3
class ContourMatch
{
public:
	ContourMatch(CvSeq* value);
	double Match();                       //��ȡvalue
	int SimMatch();                    //�򵥷ֱ�����
	double  Matchforbolt();               //ƥ����˨����
	double  Matchforspacer();             //ƥ���Ƭ����
	double  Matchfornut();                //ƥ����ĸ����
	~ContourMatch();
private:
	CvSeq* contour;
};
