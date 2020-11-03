#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

using namespace std;
using namespace cv;

int main()
{
	int loRed[3] = {0, 0, 220};
	int hiRed[3] = {4, 27, 255};
	int area;

	char key;

	VideoCapture cap(0);

	Mat frame, frameMask;



	do {
		if(cap.read(frame)) {
			flip(frame, frame, 1); //Flips frame

			cvtColor(frame, frameMask, COLOR_BGR2HSV);
			inRange(
				frameMask,
				Scalar(loRed[0], loRed[1], loRed[2]),
				Scalar(hiRed[0], hiRed[1], hiRed[2]),
				frameMask
			);

			vector<vector<Point>> contours;
			findContours(frameMask, contours, RETR_LIST, CHAIN_APPROX_TC89_KCOS);
			sort(
				contours.begin(),
				contours.end(),
				[](auto &a, auto &b) {
					return contourArea(a, false) > contourArea(b, false);
			 	}
			);

			if(contours.size() > 1){
				Rect table[2] = {Rect(), Rect()};
           		for(auto c:contours){
               		Rect r = boundingRect(c);
               		area = r.area();
               		if(area > table[0].area()){
                   		table[0] = r;
               		}else if (area > table[1].area()){
                   		table[1] = r;
               		}
            	}

	       		for(int i = 0; i < 2; i++){
            		rectangle(frame, table[i], {255, 0, 0}, 2);
        		}

        		if(table[0].y <= table[1].y + 50 && table[1].y <= table[0].y + 50){
        			line(
						frame,
        				{table[0].x + table ->width /2, table[0].y + table->height/ 2},
        				{table[1].x + table ->width /2, table[1].y + table->height/ 2},
        				{150, 150, 0},
						3
					);
        		}
        	}
		}
		key = (cv::waitKey(1000.0/60.0)&0x0ff);
		if (key == 27) break;
        imshow("Plush detector", frame);
	} while (true);
	return 0;
}