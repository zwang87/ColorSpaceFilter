#pragma once;
#include <iostream>
#include <string>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <Windows.h>

using namespace std;
using namespace cv;

class CSTest
{
	int H_MIN;
	int H_MAX;
	int S_MIN;
	int S_MAX;
	int V_MIN;
	int V_MAX;

	int Y_MIN;
	int Y_MAX;
	int Cb_MIN;
	int Cb_MAX;
	int Cr_MIN;
	int Cr_MAX;

	int dilation_elem;
	int dilation_size;
	int erosion_elem;
	int erosion_size;
	int erosion_type;
	int dilation_type;

	static const int FRAME_WIDTH = 640;
	static const int FRAME_HEIGHT = 480;
	static const int MAX_NUM_OBJECTS = 50;
	static const int MIN_OBJECT_AREA = 20*20;
	static const int MAX_OBJECT_AREA = FRAME_WIDTH * FRAME_HEIGHT / 1.5;

	char* originWindowName;
	char* hsvWindowName;
	char* thresholdWindowName;
	char* windowName4;
	char* hsvTrackbarWindowName;
	char* yCbCrTrackbarWindowName;

	bool _running;

public:
	CSTest();
	~CSTest();

	void onTrackbarSlide(int pos);

	void CreateHSVTrackbars();

	void CreateYCbCrTrackbars();

	void CreateErodeTrackbars();

	void CreateDilateTrackbars();

	void Erosion(Mat& input, Mat& output, int erosion_type, int erosion_size);

	void Dilation(Mat& input, Mat& output, int dilation_type, int dilation_size);

	int StartCapture();

	void StopCapture();

	Mat GetCombinedImage(Mat img1, Mat img2, Mat img3);

private:
	Mat pCapImage;
	Mat hsvImage;
	Mat hsvThresholdImage;
	Mat yCbCrThresholdImage;
	Mat combinedImage;
	Mat yCbCrImage;
	Mat hsv_erosion_img;
	Mat yCbCr_erosion_img;
	Mat hsv_dilation_img;
	Mat yCbCr_dilation_img;
};