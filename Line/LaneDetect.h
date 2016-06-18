#pragma once

#include "stdafx.h"
#include "LaneDetect.h"
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<vector>
#include<math.h>
#include<opencv\cxcore.h>

using namespace std;


struct LaneCan{
	int w;
	int h;
	int val;
	LaneCan(int w1, int h1, int val1)
		: w(w1), h(h1), val(val1){};
};


struct _line{
	int _x; int _y;
	int	 x;	int y;
	_line() : x(0), y(0), _x(0), _y(0){};
	_line(int a, int b, int c, int d)
		: _x(a), _y(b), x(c), y(d){
	};
};


class CLaneDectect
{
public:
	void LaneDetect(IplImage* src, IplImage* dst);
private:
	void processing(IplImage* src, IplImage* dst);
	void candidate(IplImage* diff, IplImage* dst);
	void cal(vector<_line>& first, vector<_line>& second, _line &f, _line& s);
};

