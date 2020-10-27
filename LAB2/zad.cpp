#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>
#include <sys/wait.h>

int main(int argc, char** argv) {
	cv::VideoCapture cap(0);
	int camWidth, camHeight, rangeMinH, rangeMinS, rangeMinV, rangeMaxH, rangeMaxS, rangeMaxV;
	char key;

	camWidth = 320;
	camHeight = 200;

	if (argc == 3) {
		camWidth = atoi(argv[1]);
		camHeight = atoi(argv[2]);
	}
	if(!cap.isOpened()) {
		std::cerr << "error opening frames source" << std::endl;
		return -1;
	}
	std::cout << "Video size: " << cap.get( cv::CAP_PROP_FRAME_WIDTH ) << "x" << cap.get( cv::CAP_PROP_FRAME_HEIGHT ) << std::endl;
		
    cv::Mat frame, hsv, hsvno;

	cv::namedWindow("HSV", cv::WINDOW_AUTOSIZE);

	cv::createTrackbar("min H", "In range", &rangeMinH, 255);
	cv::createTrackbar("max H", "In range", &rangeMaxH, 255);
	cv::createTrackbar("min S", "In range", &rangeMinS, 255);
	cv::createTrackbar("max S", "In range", &rangeMaxS, 255);
	cv::createTrackbar("min V", "In range", &rangeMinV, 255);
	cv::createTrackbar("max V", "In range", &rangeMaxV, 255);

	do {
   		if ( cap.read( frame ) ) {

			cv::resize(frame, frame, cv::Size(camWidth, camHeight));

			cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
			cv::cvtColor(frame, hsvno, cv::COLOR_BGR2HSV);

			cv::GaussianBlur(hsv, hsv, cv::Size(15, 15), 0);

			cv::inRange(hsv, cv::Scalar(rangeMinH, rangeMinS, rangeMinV), cv::Scalar(rangeMaxH, rangeMaxS, rangeMaxV), hsv);
			cv::imshow( "Frame", frame );
			cv::imshow( "In range", hsv);
			cv::imshow( "HSV", hsvno);

			key = (cv::waitKey(1000.0/60.0)&0x0ff);

			if (key == 27) break;

			if (key == 'x') {
				auto r = cv::selectROI("Frame", frame);
				cv::Mat roi = frame(r);
				cv::imshow("ROI", roi);
				cv::imwrite("roi.bmp", roi);
			}			
    	}
	} while (true);

	cv::waitKey(0);

    return 0;
}