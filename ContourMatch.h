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
	double Match();                       //»ñÈ¡value
	int SimMatch();                    //¼òµ¥·Ö±æÂÖÀª
	double  Matchforbolt();               //Æ¥ÅäÂİË¨ÂÖÀª
	double  Matchforspacer();             //Æ¥ÅäµæÆ¬ÂÖÀª
	double  Matchfornut();                //Æ¥ÅäÂİÄ¸ÂÖÀª
	~ContourMatch();
private:
	CvSeq* contour;
};
