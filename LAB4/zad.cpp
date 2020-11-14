#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

bool compareX(Point p1, Point p2) { return p1.x < p2.x; }
bool compareY(Point p1, Point p2) { return p1.y < p2.y; }
bool compareDist(pair<Point, Point> p1, pair<Point, Point> p2) { return norm(p1.first - p1.second) < norm(p2.first - p2.second); }
double _distance(Point p1, Point p2) { return sqrt(pow(p1.x - p2.x, 2)) + pow(p1.y - p2.y, 2); }

void orderPoints(vector<Point> inpts, vector<Point> &ordered)
{
	sort(inpts.begin(), inpts.end(), compareX);
	vector<Point> lm(inpts.begin(), inpts.begin()+2);
	vector<Point> rm(inpts.end()-2, inpts.end());

	sort(lm.begin(), lm.end(), compareY);
	Point tl(lm[0]);
	Point bl(lm[1]);
	vector<pair<Point, Point> > tmp;
	for(size_t i = 0; i< rm.size(); i++)
	{
		tmp.push_back(make_pair(tl, rm[i]));
	}

	sort(tmp.begin(), tmp.end(), compareDist);
	Point tr(tmp[0].second);
	Point br(tmp[1].second);

	ordered.push_back(tl);
	ordered.push_back(tr);
	ordered.push_back(br);
	ordered.push_back(bl);
}

void fourPointTransform(Mat src, Mat &dst, vector<Point> pts)
{
	vector<Point> ordered_pts;
	orderPoints(pts, ordered_pts);

	double wa = _distance(ordered_pts[2], ordered_pts[3]);
	double wb = _distance(ordered_pts[1], ordered_pts[0]);
	double mw = max(wa, wb);

	double ha = _distance(ordered_pts[1], ordered_pts[2]);
	double hb = _distance(ordered_pts[0], ordered_pts[3]);
	double mh = max(ha, hb);

	Point2f src_[] =
	{
			Point2f(ordered_pts[0].x, ordered_pts[0].y),
			Point2f(ordered_pts[1].x, ordered_pts[1].y),
			Point2f(ordered_pts[2].x, ordered_pts[2].y),
			Point2f(ordered_pts[3].x, ordered_pts[3].y),
	};
	Point2f dst_[] =
	{
			Point2f(0,0),
			Point2f(mw - 1, 0),
			Point2f(mw - 1, mh - 1),
			Point2f(0, mh - 1)
	};
	Mat m = getPerspectiveTransform(src_, dst_);
	warpPerspective(src, dst, m, Size(mw, mh));
}

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
            vector<vector<Point>> approx;
            vector<Vec4i> hierarchy;
            findContours(gray, contours, hierarchy, RETR_LIST, CHAIN_APPROX_SIMPLE);
			sort(
				contours.begin(),
				contours.end(),
				[](auto &a, auto &b) {
					return contourArea(a, false) > contourArea(b, false);
			 	}
            );

			size_t i;

			for(i = 0; i < contours.size(); i++) {
				double peri = arcLength(contours[i], true);
				approxPolyDP(contours[i], approx[i], 0.02 * peri, true);
			}
			
			//sort(approx.begin(), approx.end(), compareContourAreas);

			for(i = 0; i < approx.size(); i++) {
				drawContours(frame, approx, i, Scalar(0,0,255), 3);
				if(approx[i].size() == 4) break;
			}

			if(i < approx.size()) {
				fourPointTransform(frame, scan, approx[i]);
				cvtColor(scan, scan, COLOR_BGR2GRAY, 1);
				imshow("Scan", scan);
			}
            
            imshow("Edge", frame);

		    if (key == 27) break;
        }
	} while (true);
	return 0;
}