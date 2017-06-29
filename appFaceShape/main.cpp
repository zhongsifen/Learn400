//
//  main.cpp
//  appFaceShape
//
//  Created by SIFEN ZHONG on 11/9/15.
//
//

#include "FaceX.hpp"
#include "FaceShape/FaceShape.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
	bool ret = false;
	char key = '\0';
	
	FaceShape faceshape;
//	ret = faceshape.load(RES_FACEX + "face.mytracker");		if (!ret) return -1;

//	faceshape.save(RES_FACEX + "face.sinit.facex");
//
//	return 0;
	
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
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		h = g(roi);
		w = f.clone();
		
		ret = faceshape.detect(g, face);		if (!ret) continue;
		ret = faceshape.shape(g, face, shape, score);		if (!ret) continue;
		FaceX::show_rect(w, face, COLOR_0000FF);
//		std::cout << score << std::endl;
		if (score >= 1) {
			FaceX::show_shape(w, shape);
		}
		cv::imshow("facex", w);
		key = cv::waitKey(5);
	} while (key != 'q');
	
	cap.release();
	
	return 0;
}
