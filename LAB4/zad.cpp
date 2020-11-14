#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

void findAndCutDocument(Mat in, Mat &out) {
	Mat gray;

	//Prepare frame
	cvtColor(in, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(5,5), 0);
    Canny(gray, gray, 75, 200);

    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

	//Find contours
    findContours(gray, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
	sort(
		contours.begin(),
		contours.end(),
		[](auto &a, auto &b) {
			return contourArea(a, false) > contourArea(b, false);
	 	}
    );

    vector<vector<Point>> approx(contours.size());
	size_t i, j;

	//Contour approximation
	for(i = 0; i < contours.size(); i++) {
		double peri = arcLength(contours.at(i), true);
		approxPolyDP(contours.at(i), approx.at(i), 0.02 * peri, true);
	}
	
	//Find rect (document)
	vector<vector<Point>> docContour;
	for(i = 0; i < approx.size(); i++) {
		drawContours(in, approx, i, Scalar(0,0,255), 3);
		if(approx.at(i).size() == 4) {
			docContour.push_back(approx.at(i));
			break;
		}
	}
	
	//If document exists, cut it out and transform it
	if (docContour.size() > 0) {
		sort(docContour.begin(), docContour.end(),
			[](auto &a, auto &b) {
				return contourArea(a, true) > contourArea(b, true);
			}
		);

		Mat dst(Size(300,200), CV_8UC3);
		vector<Point2f> _src = {{0,0}, {dst.cols, 0}, {dst.cols, dst.rows}, {0, dst.rows}};
		vector<Point2f> _dst;
		
		for (auto p: docContour.at(0)) {
			_dst.push_back(Point2f(p.x, p.y));
		}

		auto wrapMat = getPerspectiveTransform(_dst, _src);
		warpPerspective(in, dst, wrapMat, Size(dst.cols, dst.rows));
		flip(dst, dst, +1);
		out = dst.clone();
	}
}

void rotateDocument(Mat in, Mat &out) {        
	int scanHalfX = in.cols / 2;
	int scanHalfY = in.rows / 2;

	int cx, cy;

	int loMarker[3] = {116, 56, 206};
	int hiMarker[3] = {142, 141, 255};

	Mat hsv;

	vector<vector<Point>> contours;

	cvtColor(in, hsv, COLOR_BGR2HSV);
	inRange(
		hsv,
		Scalar(loMarker[0], loMarker[1], loMarker[2]),
		Scalar(hiMarker[0], hiMarker[1], hiMarker[2]),
		hsv
	);

	findContours(hsv, contours, RETR_LIST, CHAIN_APPROX_TC89_KCOS);
	sort(
		contours.begin(),
		contours.end(),
		[](auto &a, auto &b) {
			return contourArea(a, false) > contourArea(b, false);
		}
	);

	if(contours.size() > 1) {
		Rect rect = Rect();
		for(auto c:contours) {
			Rect br = boundingRect(c);
			int area = br.area();
			if(area > rect.area()) {
				rect = br;
			}
		}
		cx = rect.x + rect.width / 2;
		cy = rect.y + rect.height / 2;


		if(cx < scanHalfX && cy < scanHalfY) {
			//Do nothing
		} else
		if (cx > scanHalfX && cy < scanHalfY) {
			rotate(in, out, ROTATE_90_CLOCKWISE);
		} else
		if (cx < scanHalfX && cy > scanHalfY) {
			rotate(in, out, ROTATE_90_COUNTERCLOCKWISE);
		} else
		if (cx > scanHalfX && cy > scanHalfY) {
			rotate(in, out, ROTATE_180);
		}
	}
}

int main() {
    char key;
	VideoCapture cap(0);
	Mat frame, scan;

	do {
		if(cap.read(frame)) {
            key = (waitKey(1000.0/60.0)&0x0ff);

			findAndCutDocument(frame, scan);
			rotateDocument(scan, scan);

            imshow("Scan", scan);
            imshow("Edge", frame);

		    if (key == 27) break;
        }
	} while (true);

	return 0;
}