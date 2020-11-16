#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;
using namespace cv;

vector<Rect> detectedCars;

string gen_random(const int len) {
		string tmp_s;
		static const char alphanum[] =
				"0123456789"
				"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz";
		srand( (unsigned) time(NULL) * getpid());
				for (int i = 0; i < len; ++i) 
				tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
		return tmp_s;		
}

void detect(const Mat& frame, CascadeClassifier& cascade) {
	Mat frameGray;
	vector<Rect> detectedCar;
	cvtColor(frame, frameGray, COLOR_BGR2GRAY);
	equalizeHist(frameGray, frameGray);
	cascade.detectMultiScale(frameGray, detectedCar, 1.1, 3, 0, Size(50,50));
	for (auto& det : detectedCar) {
		Rect near(-1,-1,0,0);
		if (detectedCars.size() == 0) detectedCars.push_back(det);
		for (Rect car : detectedCars) {
			if(car.x - 20 < det.x && car.y < det.x) {
				near = car;
				break;
			} else
			if(car.x - 20 < det.x && car.y < det.y) {
				near = car;
				break;
			} else continue;
		}
		if (near.x < 0) detectedCars.push_back(det);
		rectangle(frame, det, Scalar(255, 255, 0), 2);
	}
	cout << "Count: " << detectedCars.size() << "\n";

	imshow("Display", frame);
}

/*void detectAndSave(const Mat& frame, CascadeClassifier& cascade) {
	Mat frame_gray;
	cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);
	
	int i = 0;
	std::vector<Rect> detectedRect;
	cascade.detectMultiScale(frame_gray, detectedRect);
	for (auto& det : detectedRect) {
		Mat toSave = frame(det);
		String toSavePath = "data/car_image_" + to_string(i++) + gen_random(5) + ".jpg";
		imwrite(toSavePath, toSave);
		cout << "Save: " << toSavePath << endl;
	}
}*/

int main() {
	CascadeClassifier cascade;
	VideoCapture capture;
	Mat frame, smol;



	capture.open("vid.mkv");
	
	if (!cascade.load("cars.xml") || !capture.isOpened()) return -1;

	while (capture.read(frame)) {
		if (frame.empty()) {
			return -1;
		}
		resize(frame, smol, Size(1240, 720));
		detect(smol, cascade);
		if (waitKey(10) == 27)
			break;
	}
	return 0;
}