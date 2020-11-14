#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

int main() {
    char key;

	VideoCapture cap(0);

	Mat frame, gray, scan;

	do {
		if(cap.read(frame)) {
            key = (waitKey(1000.0/60.0)&0x0ff);
			
            cvtColor(frame, gray, COLOR_BGR2GRAY);
            GaussianBlur(gray, gray, Size(5,5), 0);
            Canny(gray, gray, 75, 200);

            vector<vector<Point>> contours;
            vector<Vec4i> hierarchy;

			//double ratio = frame.rows / 500.0;

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

			for(i = 0; i < contours.size(); i++) {
				double peri = arcLength(contours.at(i), true);
				approxPolyDP(contours.at(i), approx.at(i), 0.02 * peri, true);
			}

			for(i = 0; i < approx.size(); i++) {
				drawContours(frame, approx, i, Scalar(0,0,255), 3);
				if(approx.at(i).size() == 4) break;
			}

			vector<vector<Point>> docContour;
			docContour.push_back(approx.at(i));

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
				warpPerspective(frame, dst, wrapMat, Size(dst.cols, dst.rows));
				flip(dst, dst, +1);
				imshow("DST", dst);
			}
            
            imshow("Edge", frame);

		    if (key == 27) break;
        }
	} while (true);
	return 0;
}