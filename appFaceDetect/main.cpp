//
//  main.cpp
//  appFaceDetect
//
//  Created by SIFEN ZHONG on 6/8/15.
//
//

#include "FaceX.hpp"
#include "FaceDetect/FaceDetect.hpp"
#include <iostream>
using namespace cv;

int main(int argc, const char * argv[]) {
	bool ret = false;
	char key = '\0';
	
	VideoCapture vide;
	vide.open(0);		if (!vide.isOpened()) return -1;
	int m1 = 640;
	int m2 = 480;
	vide.set(CV_CAP_PROP_FRAME_WIDTH,  m1);
	vide.set(CV_CAP_PROP_FRAME_HEIGHT, m2);
	const int n1 = 288;
	const int n2 = 288;
	Rect roi(Point((m1 - n1) / 2, (m2 - n2) / 2), Size(n1, n2));
	Mat f, g, h, w;
	Rect face;
	Rect head;
	
	FaceDetect detect;
	do {
		vide.read(f);
		
		cvtColor(f, g, COLOR_BGR2GRAY);
		w = f.clone();
		
//		ret = detect.detect_roi(g, roi, face);
		ret = detect.detect(g, face, head);
		
		if (ret) {
			FaceX::show_rect(w, face, COLOR_0000FF);
			FaceX::show_rect(w, head, COLOR_00FF00);
			std::cout << face << std::endl;
		}
		else {
			FaceX::show_rect(w, roi, COLOR_FF0000);
		}
		
		imshow("facex", w);
		key = waitKey(5);
		if (key == 'v') {
			std::cout << face.size() << std::endl;
		}
	} while (key != 'q');
	
	std::cout << "Hello, World!\n";
	return 0;
}
