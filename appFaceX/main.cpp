//
//  main.cpp
//  appFaceX
//
//  Created by SIFEN ZHONG on 13/9/15.
//
//

#include "FaceX.hpp"
#include "FaceTrack/FaceTrack.hpp"

#include <iostream>

int main(int argc, const char * argv[]) {
	bool ret = false;
	char key = '\0';
	
	FaceTrack track("");
	if (track.status < 1) return -1;

	cv::VideoCapture cap;
	cap.open(0);		if (!cap.isOpened()) return -1;
	int m1 = 640, m2 = 480;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,  m1);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, m2);
	cv::Mat f, g, h, w;
	cv::Rect face;
	cv::Mat shape;
	int health;
	
	do {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		equalizeHist(g, g);
		w = f.clone();

		cv::imshow("facex", w);
		key = cv::waitKey(5);
	} while (key != 'q');
	
	return 0;
}
