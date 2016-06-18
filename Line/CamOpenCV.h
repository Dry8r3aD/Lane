#pragma once

#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>


class CCamOpenCV
{
public:
	CCamOpenCV(int cameraID = -1, int width = -1, int height = -1);
	CCamOpenCV(const char *fileName);
	virtual ~CCamOpenCV(void);

	IplImage *CaptureImage();
	BITMAPINFO *GetBitmapInfo(int cameraID = 0) { return &_bitmapInfo; }

private:
	void InitBitmapInfo(int img_width, int img_height, int pixelByte);

private:
	BITMAPINFO _bitmapInfo;
	CvCapture *_capture;
};
