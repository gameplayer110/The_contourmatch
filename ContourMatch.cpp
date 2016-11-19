#include "stdafx.h"
#include "ContourMatch.h"
#include "Capture.h"

ContourMatch::ContourMatch(CvSeq* value)
{
	contour = value;
}

double ContourMatch::Match()
{
	double type;
	switch (SimMatch())
	{
	case BOLT:
		type = Matchforbolt();
	case SPACER:
		type = Matchforspacer();
	case NUT:
		type = Matchfornut();
	default:
		break;
	}
	return type;
}

int ContourMatch::SimMatch()
{
	if (contour->h_next == NULL)
	{
		return BOLT;
	}
	else
	{
		CvSeq* maxseq = GetAreaMaxContour(contour);
		CvPoint2D32f center;
		float radius;
		int a = cvMinEnclosingCircle(maxseq, &center, &radius);
		double maxarea = cvContourArea(maxseq);
		double matchingarea = radius*radius*3.141592653;
		if (maxarea / matchingarea > 0.9)
		{
			return SPACER;
		}
		else
		{
			return NUT;
		}
	}
}

double ContourMatch::Matchforbolt()
{
	return 0;
}

double ContourMatch::Matchforspacer()
{
	return 0;
}

double ContourMatch::Matchfornut()
{
	return 0;
}

ContourMatch::~ContourMatch()
{
}
