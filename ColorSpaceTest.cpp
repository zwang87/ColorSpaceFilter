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

	erosion_size = 1;
	dilation_size = 1;
	erosion_type = MORPH_ELLIPSE;
	dilation_type = MORPH_ELLIPSE;

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
	if(!capture.open(0))
	{
		cout << "No camera opened!"<< endl;
		return 0;
	}
	//set frame size
	capture.set(CV_CAP_PROP_FRAME_WIDTH, FRAME_WIDTH);
	capture.set(CV_CAP_PROP_FRAME_HEIGHT, FRAME_HEIGHT);

	char cmd;
	int frame_count = 0;
	int time_count = GetTickCount();
	int pre_timeCount = 0;
	while(_running)
	{
		frame_count++;
		if(frame_count % 10 == 0)
		{
			pre_timeCount = time_count;
			time_count = GetTickCount();
			cout << "fps: " << 10000.0/(time_count - pre_timeCount) << endl;
		}
		capture.read(pCapImage);
		Mat medianblur;
		//get image in hsv color space
		cvtColor(pCapImage, hsvImage, COLOR_BGR2HSV);
		medianBlur(hsvImage, hsvImage, 3);
		cvtColor(pCapImage, yCbCrImage, COLOR_BGR2YCrCb);
		
		//threshold the yCbCr image
		inRange(hsvImage, Scalar(H_MIN, S_MIN, V_MIN), Scalar(H_MAX, S_MAX, V_MAX), hsvThresholdImage);
		
		//threshold the HSV image
		inRange(yCbCrImage, Scalar(Y_MIN, Cb_MIN, Cr_MIN), Scalar(Y_MAX, Cb_MAX, Cr_MAX), yCbCrThresholdImage);

		//combine multiple images in same window
		combinedImage = GetCombinedImage(pCapImage, hsvImage, yCbCrImage);

		//morphological operations: Erosion & Dilation, make shapes nicely visible in image
		Erosion(hsvThresholdImage, hsv_erosion_img, erosion_type, 1);
		Dilation(hsv_erosion_img, hsv_dilation_img, dilation_type, 1);

		Erosion(yCbCrThresholdImage, yCbCr_erosion_img, erosion_type, 5);
		Dilation(yCbCr_erosion_img, yCbCr_dilation_img, dilation_type, 5);
		//show windows
		//imshow("test", medianblur);
		imshow(yCbCrTrackbarWindowName, yCbCr_dilation_img);
		imshow(hsvTrackbarWindowName, hsv_dilation_img);
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

void CSTest::Erosion(Mat& input, Mat& output, int erosion_type, int erosion_size)
{
	Mat element = getStructuringElement(erosion_type, Size(2*erosion_size + 1, 2*erosion_size + 1),
									Point(erosion_size, erosion_size));
	erode(input, output, element);
	//erode(input, output, element);
}

void CSTest::Dilation(Mat& input, Mat& output, int dilation_type, int dilation_size)
{
	Mat element = getStructuringElement(dilation_type, Size(2*dilation_size + 1, 2*dilation_size + 1),
									Point(dilation_size, dilation_size));
	dilate(input, output, element);
	//dilate(input, output, element);
}

int main(int argc, char** argv)
{
	CSTest cstest;
	cstest.CreateHSVTrackbars();
	cstest.CreateYCbCrTrackbars();
	cstest.StartCapture();

	return 0;
}