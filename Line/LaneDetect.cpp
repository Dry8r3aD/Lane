#include "stdafx.h"
#include "LaneDetect.h"
#include "LineDlg.h"

#define safe 0
#define change 1

int width;
int height;
static _line storage_past_line;
static _line storage_past_line_2;
static _line previous_line;
static _line previous_line2;
void CLaneDectect::LaneDetect(IplImage*src, IplImage*dest)
{
	
	IplImage* erod = cvCreateImage(cvGetSize(src), src->depth, 3);
	width = src->width;
	height = src->height;
	cvSmooth(src, src);
	//cvDilate(src, src);

	IplImage* cvt_hsv = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	IplImage* result = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *diff = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);


	_line my_line;
	_line my_line2;
	vector<_line> try_line;
	vector<_line> try_line2;

	cvCvtColor(src, cvt_hsv, CV_RGB2HSV);
	processing(cvt_hsv, result);

	//IplImage* tmp = cvCreateImage(cvGetSize(cvt_hsv), IPL_DEPTH_8U, 3);
	//cvCvtColor(cvt_hsv, tmp, CV_HSV2RGB);
	//cvCvtColor(tmp, result, CV_RGB2GRAY);
	//cvReleaseImage(&tmp);


	cvSobel(result, diff, 1, 1, 3);

	// to eliminate the error point using the width.
	IplImage* dst = cvCreateImage(cvGetSize(diff), diff->depth, diff->nChannels);
	cvZero(dst);
	candidate(diff, dst);

	CvMemStorage* storage = cvCreateMemStorage(0);
	float theta = 0;
	float distance = 0;
	CvSeq *lines = cvHoughLines2(diff, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 30, 1, 1);

	try_line.reserve(1000);
	try_line2.reserve(1000);

	for (int i = 0; i < lines->total; i++) {
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		if ( (line[1].x - line[0].x) == 0)
		{
			theta=CV_PI;
		}
		else{
			theta = atan((line[1].y - line[0].y) / (line[1].x - line[0].x));
		}
		distance = sqrt((line[1].y - line[0].y)*(line[1].y - line[0].y) + (line[1].x - line[0].x)*(line[1].x - line[0].x));
		if (distance>10)
		{
			if ( (theta>CV_PI / 8 && theta < 1.2*CV_PI/2) )
			{
				try_line.push_back(_line(line[0].x, line[0].y, line[1].x, line[1].y));
				cvCircle(dest, cvPoint(line[0].x, line[0].y), 7, CV_RGB(255, 0, 0), 3, 0);
				cvCircle(dest, cvPoint(line[1].x, line[1].y), 7, CV_RGB(255, 0, 0), 3, 0);
			}
			if ((theta<-CV_PI / 8 && theta>-1.2*CV_PI/2))
			{
				try_line2.push_back(_line(line[0].x, line[0].y, line[1].x, line[1].y));
				cvCircle(dest, cvPoint(line[0].x, line[0].y), 7, CV_RGB(255, 0, 255), 3, 0);
				cvCircle(dest, cvPoint(line[1].x, line[1].y), 7, CV_RGB(255, 0, 255), 3, 0);
			}
		}
	}
	cal(try_line, try_line2, my_line, my_line2);
///////////////////////////////////////////////////////////////////////////////////////////
	//my_line = storage_past_line;
	//my_line2 = storage_past_line_2;
////////////////////////////////////////////////////////////////////////////////////////////////////////////	
	static int image_count = 0;
	static int timer = 0;
	if (abs(previous_line.x - my_line.x) < 10 && abs(previous_line2.x - my_line2.x) < 10 && width*5/6>my_line2.x && my_line.x>width/6)
	{
		image_count++;
	}
	previous_line = my_line;
	previous_line2 = my_line2;
	if (image_count == 5)
	{
		storage_past_line = my_line;
		storage_past_line_2 = my_line2;
		image_count = 0;
	}else{
		my_line = storage_past_line;
		my_line2 = storage_past_line_2;
	}

	if (timer > 19)
	{
		timer = 0;
		image_count = 0;
	}

	

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0);
	char test[100];
	if (abs(width / 2 - abs(my_line.x + my_line2.x)/2) < 30)
	{
		sprintf_s(test, "Good / image_count = %d ",image_count);
		cvPutText(dest, test, cvPoint(50, 50), &font, CV_RGB(0, 0, 0));
	}
	else{
		sprintf_s(test, "Bad / image_count = %d ",image_count);
		cvPutText(dest, test, cvPoint(50, 50), &font, CV_RGB(0, 0, 0));
	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cvLine(src, cvPoint(my_line.x, my_line.y), cvPoint(my_line._x, my_line._y), CV_RGB(255, 0, 0), 8, 0);
	cvLine(src, cvPoint(my_line2.x, my_line2.y), cvPoint(my_line2._x, my_line2._y), CV_RGB(100, 0, 0), 8, 0);
	cvLine(src, cvPoint((my_line.x+my_line2.x)/2, height / 2), cvPoint(width / 2, height), CV_RGB(0, 0, 255), 8, 0);
	cvLine(src, cvPoint(my_line.x, height / 2), cvPoint(my_line2.x, height / 2), CV_RGB(0,255,0),8, 0);
	cvReleaseMemStorage(&storage);
	

	vector<_line>().swap(try_line);
	vector<_line>().swap(try_line2);
	//cvShowImage("dst", dst);
	cvShowImage("src", src);
	cvShowImage("result", dest);
	cvShowImage("diff", diff);
	cvReleaseImage(&cvt_hsv);
	cvReleaseImage(&result);
	cvReleaseImage(&diff);
	cvReleaseImage(&dst);
}


// hsv에서 원하는 영역만 선택하는 과정
void CLaneDectect::processing(IplImage* src, IplImage*dst)
{
	int sat = 0, val = 0, max = 0, min = 10;

	for (int row = 0; row < src->height; row++)
	{
		for (int col = 0; col < src->width; col++)
		{
			CvScalar value;
			value = cvGet2D(src, row, col);
			sat += value.val[1];
			val += value.val[2];
			if (max < value.val[2])
			{
				max = value.val[2];
			}
			if (min>value.val[1])
			{
				min = value.val[1];
			}
		}
	}
	sat = sat / (src->height*src->width);
	val = val / (src->height*src->width);
	if (min == 0) min = 10;

	for (int row = 0; row < src->height; row++)
	{
		for (int col = 0; col < src->width; col++)
		{
			CvScalar value;
			value = cvGet2D(src, row, col);
			if ((value.val[2]>val) || (value.val[2]>max*0.8))
			{
				cvSet2D(dst, row, col, cvScalar(255));
			}
			else{
				cvSet2D(dst, row, col, cvScalar(0, 0, 0));
			}
		}
	}
}

// to eliminate the unnessery point.
void CLaneDectect::candidate(IplImage* src_x, IplImage* dst)
{
	#define pPixel(img,y,x)	(img->imageData + (y)*img->widthStep + img->nChannels*img->depth/8*(x))
	vector<LaneCan> tmp;
	tmp.reserve(src_x->width);
	for (int h = 0; h < src_x->height; h++)
	{
		tmp.clear();
		char *x = (char *)pPixel(src_x, h, 0);
		char *d = (char *)pPixel(dst, h, 0);
		for (int w = 1; w < src_x->width - 1; w++)
		{
			if (abs(x[w])>0)
			{
				tmp.push_back(LaneCan(w, h, x[w]));
			}

		}
		for (int i = 0; i < (int)tmp.size() - 1; i++)
		{
			//int width = tmp[j].w - tmp[i].w;
			if (abs(tmp[i].val)>128)
			{
				d[tmp[i].w] = 255;
			}
		}
	}
}


void CLaneDectect::cal(vector<_line>& first, vector<_line>& second, _line& my_line, _line& my_line2)
{
	int N1 = first.size(), N2 = second.size();
	int criteria = width / 2;
	int upper, lower, count = 0;

	my_line.y = height / 2, my_line2.y = height / 2;
	my_line._y = height, my_line2._y = height;

	for (int num = 0; num < N1; num++)
	{
		upper = (height - first[num].y)*(first[num].x - first[num]._x) / (first[num].y - first[num]._y) + first[num].x;
		lower = (height / 2 - first[num].y)*(first[num]._x - first[num].x) / (first[num]._y - first[num].y) + first[num].x;
		if (upper > criteria*1.3 && upper <criteria*1.7)
		{
			count++;
			my_line.x += lower;
			my_line._x += upper;
		}

	}

	if (count != 0)
	{
		my_line.x = my_line.x / count;
		my_line._x = my_line._x / count;
		count = 0;
	}


	for (int num = 0; num < N2; num++)
	{
		upper = (height - second[num].y)*(second[num].x - second[num]._x) / (second[num].y - second[num]._y) + second[num].x;
		lower = (height / 2 - second[num].y)*(second[num]._x - second[num].x) / (second[num]._y - second[num].y) + second[num].x;
		if (upper < criteria*0.7 && upper>criteria*0.2)
		{
			count++;
			my_line2.x += lower;
			my_line2._x += upper;
		}
	}
	if (count != 0)
	{
		my_line2.x = my_line2.x / count;
		my_line2._x = my_line2._x / count;
	}
}
