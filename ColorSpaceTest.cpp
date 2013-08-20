#include "ColorSpaceTest.h"

//construction function
CSTest::CSTest()
{
	H_MIN = 0;
	H_MAX = 255;
	S_MIN = 0;
	S_MAX = 255;
	V_MIN = 0;
	V_MAX = 255;

	Y_MIN = 0;
	Y_MAX = 255;
	Cb_MIN = 0;
	Cb_MAX = 255;
	Cr_MIN = 0;
	Cr_MAX = 255;

	_running = false;

	originWindowName = "OriginalImage";
	yCbCrTrackbarWindowName = "yCbCr";
	hsvTrackbarWindowName = "HSV";

	//create windows
	namedWindow(originWindowName, CV_WINDOW_AUTOSIZE);
	namedWindow(hsvTrackbarWindowName, CV_WINDOW_AUTOSIZE);
	namedWindow(yCbCrTrackbarWindowName, CV_WINDOW_AUTOSIZE);

}

//deconstruction function
CSTest::~CSTest()
{
	StopCapture();
}

void CSTest::CreateHSVTrackbars()	//create trackbar 
{
	/*char trackbarName[10];
	sprintf(trackbarName, "H_MIN", H_MIN);
	sprintf(trackbarName, "H_MAX", H_MAX);
	sprintf(trackbarName, "S_MIN", S_MIN);
	sprintf(trackbarName, "S_MAX", S_MAX);
	sprintf(trackbarName, "V_MIN", V_MIN);
	sprintf(trackbarName, "V_MAX", V_MAX);
	*/
	createTrackbar("H_MIN", hsvTrackbarWindowName, &H_MIN, H_MAX);
	createTrackbar("H_MAX", hsvTrackbarWindowName, &H_MAX, H_MAX);
	createTrackbar("S_MIN", hsvTrackbarWindowName, &S_MIN, S_MAX);
	createTrackbar("S_MAX", hsvTrackbarWindowName, &S_MAX, S_MAX);
	createTrackbar("V_MIN", hsvTrackbarWindowName, &V_MIN, V_MAX);
	createTrackbar("V_MAX", hsvTrackbarWindowName, &V_MAX, V_MAX);
}

void CSTest::CreateYCbCrTrackbars()
{
	createTrackbar("Y_MIN", yCbCrTrackbarWindowName, &Y_MIN, Y_MAX);
	createTrackbar("Y_MAX", yCbCrTrackbarWindowName, &Y_MAX, Y_MAX);
	createTrackbar("Cb_MIN", yCbCrTrackbarWindowName, &Cb_MIN, Cb_MAX);
	createTrackbar("Cb_MAX", yCbCrTrackbarWindowName, &Cb_MAX, Cb_MAX);
	createTrackbar("Cr_MIN", yCbCrTrackbarWindowName, &Cr_MIN, Cr_MAX);
	createTrackbar("Cr_MAX", yCbCrTrackbarWindowName, &Cr_MAX, Cr_MAX);
}


void CSTest::StopCapture()
{
	//destroy windows
	destroyAllWindows();
}

int CSTest::StartCapture()
{
	VideoCapture capture;
	_running = true;
	//capture.open(0);
	if(!capture.open(0))
	{
		cout << "No camera opened!"<< endl;
		return 0;
	}
	//set frame size
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	char cmd;
	while(_running)
	{
		capture.read(pCapImage);
		//get image in hsv color space
		cvtColor(pCapImage, hsvImage, COLOR_BGR2HSV);
		cvtColor(pCapImage, yCbCrImage, COLOR_BGR2YCrCb);
		//threshold the yCbCr image
		inRange(hsvImage, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), hsvThresholdImage);
		
		//threshold the HSV image
		inRange(yCbCrImage, Scalar(Y_MIN, Cb_MIN, Cr_MIN), Scalar(Y_MAX, Cb_MAX, Cr_MAX), yCbCrThresholdImage);

		//combine multiple images in same window
		combinedImage = GetCombinedImage(pCapImage, hsvImage, yCbCrImage);
		imshow(hsvTrackbarWindowName, hsvThresholdImage);
		imshow(yCbCrTrackbarWindowName, yCbCrThresholdImage);
		imshow(originWindowName, combinedImage);
		
		if((cmd = waitKey(30)) == 27)
		{
			_running = false;
		}
	}
	return 0;
}

Mat CSTest::GetCombinedImage(Mat img1, Mat img2, Mat img3)
{
	//multiple images in same column
	Mat output(img1.rows*2 + 5, img1.cols*2 + 5, img1.type());
	img1.copyTo(output(cv::Rect(0, 0, img1.cols, img1.rows)));
	img2.copyTo(output(cv::Rect(img1.cols + 5, 0, img2.cols, img2.rows)));
	img3.copyTo(output(cv::Rect(img1.cols * 0.5, img1.rows + 5, img3.cols, img3.rows)));
	/*
	//multiple images in same row
	Mat output(img1.rows*2 + 5, img1.cols, img1.type());
	img1.copyTo(output(cv::Rect(0, 0, img1.cols, img1.rows)));
	img2.copyTo(output(cv::Rect(0, img1.rows + 5, img2.cols, img2.rows)));
	*/
	return output;
}

int main(int argc, char** argv)
{
	CSTest cstest;
	cstest.CreateHSVTrackbars();
	cstest.CreateYCbCrTrackbars();
	cstest.StartCapture();

	return 0;
}