//
//  main.cpp
//  appFaceTrack
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#include "FaceX.hpp"
#include "FaceTrack/FaceTrack.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
	bool ret = false;
	char key = '\0';

	FaceTrack track;
//	ret = track.load(RES_FACEX + "face.mytracker");		if (!ret) return -1;

	cv::VideoCapture cap;
	cap.open(0);		if (!cap.isOpened()) return -1;
	int m1 = 640, m2 = 480;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,  m1);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, m2);
	const int n1 = 288;
	const int n2 = 288;
	cv::Rect roi(cv::Point((m1 - n1) / 2, (m2 - n2) / 2), cv::Size(n1, n2));
	cv::Mat f, g, h, w;
	cv::Rect face;
	cv::Mat shape;
	int score;
	
	do {
		cv::Scalar color = COLOR_0000FF;
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		w = f.clone();
		
		ret = track.detect(g, face);		if (!ret) continue;
		ret = track.shape(g, face, shape, score);		if (!ret) continue;
		
		FaceX::show_rect(w, face, color);
		FaceX::show_points(w, shape, color);
		cv::imshow("facex", w);
		key = cv::waitKey(5);
	} while (key < '2');
	
	do {
		cv::Scalar color = COLOR_00FF00;
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		w = f.clone();
		
		ret = track.track(g, shape, score);		if (!ret) continue;
		
		FaceX::show_points(w, shape, color);
		cv::imshow("facex", w);
		key = cv::waitKey(5);
	} while (key < '3');
	
	cap.release();
	
	return 0;
}
