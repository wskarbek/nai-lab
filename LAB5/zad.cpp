#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <ctime>
#include <unistd.h>

using namespace std;
using namespace cv;

void detect(const Mat& frame, CascadeClassifier& cascade) {
  Mat frame_gray;
  cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
  equalizeHist(frame_gray, frame_gray);
  
  std::vector<Rect> detectedRect;
  cascade.detectMultiScale(frame_gray, detectedRect);
  for (auto& det : detectedRect)
    rectangle(frame, det, Scalar(255, 255, 0), 2);
  imshow("Display", frame);
}

int main() {
  const String cascade_name("cars.xml");
  CascadeClassifier cascade;
  
  if (!cascade.load(cascade_name)) {
    cout << "Error loading cars cascade\n";
    return -1;
  }
  
  VideoCapture capture;
  capture.open("vid.mkv");
  if (!capture.isOpened()) {
    cout << "Error opening video capture";
    return -1;
  }
  Mat frame, smol;
  while (capture.read(frame)) {
    if (frame.empty()) {
      cout << "No more frames!\n";
      break;
    }
    resize(frame, smol, Size(1240, 720));
    detect(smol, cascade);
    if (waitKey(10) == 27)
      break;
  }
  return 0;
}