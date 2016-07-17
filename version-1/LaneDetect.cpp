#include "stdafx.h"
#include "LaneDetect.h"
#include "LineDlg.h"
#include<cmath>
#define change 1
#define safe 0

void CLaneDectect::LaneDetect(IplImage*src, IplImage*dest)
{
	cvSmooth(src, src,CV_MEDIAN,3);
	width = src->width, height = src->height;
	//cvDilate(src, src);
	IplImage* cvt_hsv = cvCreateImage(cvGetSize(src), src->depth, src->nChannels);
	IplImage* result = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage* diff = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	cvCvtColor(src, cvt_hsv, CV_RGB2HSV);
	processing(cvt_hsv, result);
	cvSobel(result, diff, 1, 1, 3);
	// to eliminate the error point using the width.
	//IplImage* dst = cvCreateImage(cvGetSize(diff), diff->depth, diff->nChannels);
	//cvZero(dst);
	//candidate(diff, dst);

	CvMemStorage* storage = cvCreateMemStorage(0);
	float theta = 0, distance = 0;

	CvSeq *lines = cvHoughLines2(diff, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 30, 1, 1);

	for (int i = 0; i < lines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		if ((line[1].x - line[0].x) == 0) 
			continue;
		else
			theta = (float)atan((line[1].y - line[0].y) / (line[1].x - line[0].x));

		distance = (float)sqrt(pow(line[1].y - line[0].y, 2) + pow(line[1].x - line[0].x, 2));

		if (distance > 5){
			if (theta > (float)CV_PI / 10 && theta < 0.5*(float)CV_PI){
				candidate_line_1.push_back(_line(line[0].x, line[0].y, line[1].x, line[1].y));
				cvCircle(dest, cvPoint(line[0].x, line[0].y), 7, CV_RGB(255, 0, 0), 3, 0);
				cvCircle(dest, cvPoint(line[1].x, line[1].y), 7, CV_RGB(255, 0, 0), 3, 0);
			}

			if (theta < -(float)CV_PI / 10 && theta > -0.5*(float)CV_PI){
				candidate_line_2.push_back(_line(line[0].x, line[0].y, line[1].x, line[1].y));
				cvCircle(dest, cvPoint(line[0].x, line[0].y), 7, CV_RGB(0, 0, 255), 3, 0);
				cvCircle(dest, cvPoint(line[1].x, line[1].y), 7, CV_RGB(0, 0, 255), 3, 0);
			}
		}
	}

	cal(candidate_line_1, candidate_line_2, cur_out_line_1, cur_out_line_2, height, width);
	reline(cur_out_line_1, cur_out_line_2);

	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 1.0, 1.0);
	char test[100];

	if (abs(width / 2 - abs(cur_out_line_1.x + cur_out_line_2.x) / 2) < 30)
	{
		sprintf_s(test, "Good %d %d / %d %d", cur_out_line_2.x, cur_out_line_2.y, cur_out_line_1.x, cur_out_line_1.y);
		cvPutText(dest, test, cvPoint(50, 50), &font, CV_RGB(0, 0, 0));
	}
	else{
		sprintf_s(test, "Bad  %d %d / %d %d", cur_out_line_2.x, cur_out_line_2.y, cur_out_line_1.x, cur_out_line_1.y);
		cvPutText(dest, test, cvPoint(50, 50), &font, CV_RGB(0, 0, 0));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	cvLine(src, cvPoint(cur_out_line_1.x, cur_out_line_1.y), cvPoint(cur_out_line_1._x, cur_out_line_1._y), CV_RGB(255, 0, 0), 8, 0);
	cvLine(src, cvPoint(cur_out_line_2.x, cur_out_line_2.y), cvPoint(cur_out_line_2._x, cur_out_line_2._y), CV_RGB(0, 255, 0), 8, 0);
	//cvLine(src, cvPoint((my_line.x + my_line2.x) / 2, height / 2), cvPoint(width / 2, height), CV_RGB(0, 0, 255), 8, 0);
	//cvLine(src, cvPoint(my_line.x, height / 2), cvPoint(my_line2.x, height / 2), CV_RGB(0, 255, 0), 8, 0);
	cvReleaseMemStorage(&storage);

	candidate_line_1.clear(), candidate_line_2.clear();

	cvShowImage("src", src);
	cvShowImage("result", dest);
	cvShowImage("diff", diff);

	cvReleaseImage(&cvt_hsv);
	cvReleaseImage(&result);
	cvReleaseImage(&diff);
}

// hsv에서 원하는 영역만 선택하는 과정
void CLaneDectect::processing(IplImage* src, IplImage*dst)
{
	float sat = 0, val = 0;
	CvScalar value;
	for (int row = 0; row < src->height; row++)
	{
		for (int col = 0; col < src->width; col++)
		{
			value = cvGet2D(src, row, col);
			sat += (float)value.val[1]; // S
			val += (float)value.val[2]; // V
		}
	}
	sat = sat / (src->height*src->width), val = val / (src->height*src->width);
	for (int row = 0; row < src->height; row++)
	{
		for (int col = 0; col < src->width; col++)
		{
			value = cvGet2D(src, row, col);
			// 흰색선 추출
			if (value.val[2] > val && value.val[1] < sat*0.4) cvSet2D(dst, row, col, cvScalar(255));
			else cvSet2D(dst, row, col, cvScalar(0));

			//노란색선 추출
			if (value.val[2] > val*0.4 && value.val[1] > sat) cvSet2D(dst, row, col, cvScalar(255));
			else cvSet2D(dst, row, col, cvScalar(0));
		}
	}
}

void CLaneDectect::cal(vector<_line> in_1, vector<_line> in_2, _line& out_1, _line& out_2, int h, int w){
	_line l;
	out_1 = _line(0, 0, 0, 0), out_2 = _line(0, 0, 0, 0);

	float gradiant, t_x, t_y, x = w / 2, y = h, len;
	for (int cnt = 0; cnt < in_1.size(); ++cnt){
		l = in_1[cnt];
		gradiant = (l.y - l._y) / (l.x - l._x);

		t_x = (l.x - l._x)*x + (l.y - l._y)*y + l._x*gradiant*(l.y - l._y) - l._y*(l.y - l._y);
		t_x = t_x / (l.x - l._x + gradiant*(l.y - l._y));
		t_y = gradiant*(t_x - l._x) + l._y;

		len = sqrt(pow(t_x - x, 2) + pow(t_y - y, 2));
		if (-80 < t_y && t_y < h + 80){
			if (200 < len && len < 300){
				out_1 = _line((gradiant*l._x - l._y) / gradiant, 0, (h + gradiant*l._x - l._y) / gradiant, h);
			}
		}
	}

	for (int cnt = 0; cnt < in_2.size(); ++cnt){
		l = in_2[cnt];
		gradiant = (l.y - l._y) / (l.x - l._x);

		t_x = (l.x - l._x)*x + (l.y - l._y)*y + l._x*gradiant*(l.y - l._y) - l._y*(l.y - l._y);
		t_x /= (l.x - l._x + gradiant*(l.y - l._y));
		t_y = gradiant*(t_x - l._x) + l._y;

		len = sqrt(pow(t_x - x, 2) + pow(t_y - y, 2));
		if (-80 < t_y && t_y < h + 80){
			if (200 < len && len < 300){
				out_2 = _line((gradiant*l._x - l._y) / gradiant, 0, (h + gradiant*l._x - l._y) / gradiant, h);
			}
		}
	}
}

void CLaneDectect::reline(_line &first, _line &second){
	if (second == _line(0, 0, 0, 0)){
		if (!(first == _line(0, 0, 0, 0))){
			prev_out_line_2 = _line(width - first._x, first._y, width - first.x, first.y);
		}
		second = prev_out_line_2;
	}
	else
		prev_out_line_2 = second;
	
	if (first == _line(0, 0, 0, 0)){
		if (!(second == _line(0, 0, 0, 0))){
			prev_out_line_1 = _line(width - second._x, second._y, width - second.x, second.y);
		}
		first = prev_out_line_1;
	}
	else
		prev_out_line_1 = first;
}
