#pragma once
#include "stdafx.h"
#include "LaneDetect.h"
#include<opencv\cv.h>
#include<opencv\highgui.h>
#include<vector>
#include<math.h>
#include<opencv\cxcore.h>
using namespace std;

class CLaneDectect{
public:
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
		bool operator ==(const _line& other) const{
			if (_x == other._x && _y == other._y
				&& _x == other._x&&x == other.y) return true;
			else
				return false;
		}
	};
	void LaneDetect(IplImage* src, IplImage* dst);

private:
	int width, height;
	_line cur_out_line_1, cur_out_line_2, prev_out_line_1, prev_out_line_2;
	vector<_line> candidate_line_1, candidate_line_2;
	void processing(IplImage* src, IplImage* dst);
	void cal(vector<_line> first, vector<_line> second, _line &f, _line& s, int h, int w);
	void reline(_line &first, _line &second);
};

