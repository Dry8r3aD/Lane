#include "stdafx.h"
#include "CamOpenCV.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCamOpenCV::CCamOpenCV(int cameraID, int width, int height)
{
	_capture = cvCreateCameraCapture(cameraID);

	if (_capture && 0 < width && 0 < height) {
		cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_WIDTH, width);
		cvSetCaptureProperty(_capture, CV_CAP_PROP_FRAME_HEIGHT, height);
	}
}

CCamOpenCV::CCamOpenCV(const char *fileName)
{
	_capture = cvCaptureFromAVI(fileName);
}

CCamOpenCV::~CCamOpenCV(void)
{
	if (_capture) {
		cvReleaseCapture(&_capture);
	}
}

void CCamOpenCV::InitBitmapInfo(int img_width, int img_height, int pixelByte)
{
	memset(&_bitmapInfo, 0, sizeof (BITMAPINFO));

	_bitmapInfo.bmiHeader.biSize = sizeof (BITMAPINFOHEADER);
	_bitmapInfo.bmiHeader.biWidth = img_width;
	_bitmapInfo.bmiHeader.biHeight = -img_height; // top-down bitmap, negative height
	_bitmapInfo.bmiHeader.biPlanes = 1;
	_bitmapInfo.bmiHeader.biBitCount = pixelByte * 8;
	_bitmapInfo.bmiHeader.biCompression = BI_RGB;
	_bitmapInfo.bmiHeader.biSizeImage = img_width*img_height*pixelByte;
	_bitmapInfo.bmiHeader.biXPelsPerMeter = 100;
	_bitmapInfo.bmiHeader.biYPelsPerMeter = 100;
	_bitmapInfo.bmiHeader.biClrUsed = 0;
	_bitmapInfo.bmiHeader.biClrImportant = 0;
}

IplImage *CCamOpenCV::CaptureImage()
{
	if (_capture) {
		if (cvGrabFrame(_capture)) {
			IplImage *cvImage = cvRetrieveFrame(_capture);
			if (cvImage &&
				cvImage->width != _bitmapInfo.bmiHeader.biWidth &&
				cvImage->height != _bitmapInfo.bmiHeader.biHeight) {
				InitBitmapInfo(cvImage->width, cvImage->height, cvImage->widthStep / cvImage->width);
			}
			return cvImage;
		}
	}
	return NULL;
}
