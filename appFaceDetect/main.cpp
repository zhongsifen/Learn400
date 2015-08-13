//
//  main.cpp
//  appFaceDetect
//
//  Created by SIFEN ZHONG on 6/8/15.
//
//

#include "FaceDetect.hpp"
#include <iostream>
using namespace cv;

int main(int argc, const char * argv[]) {
	VideoCapture vide;
	vide.open(0);		if (!vide.isOpened()) return -1;
	int m1 = 640;
	int m2 = 480;
	vide.set(CV_CAP_PROP_FRAME_WIDTH,  m1);
	vide.set(CV_CAP_PROP_FRAME_HEIGHT, m2);
	const int n1 = 288;
	const int n2 = 288;
	Rect roi(Point((m1 - n1) / 2, (m2 - n2) / 2), Size(n1, n2));
	Mat f, g, h;
	Rect face;
	
	bool ret;
	char key;
	
	FaceDetect detect;
	ret = detect.load(RES_FACEX + "haarcascade_frontalface_alt.xml");		if (!ret) return -1;
	do {
		vide.read(f);
		
		cvtColor(f, g, COLOR_BGR2GRAY);
		equalizeHist(g, g);
		Mat h = g(roi);
		ret = detect.detect(h, face);
		
		if (ret) {
			face.x += roi.x;
			face.y += roi.y;

			rectangle(f, roi, Scalar(0x00, 0xFF, 0x00));
			rectangle(f, face, Scalar(0xFF, 0x00, 0x00));
		}
		else {
			rectangle(f, roi, Scalar(0x00, 0x00, 0xFF));
		}
		
		imshow("facex", f);
		key = waitKey(5);
		if (key == 'v') {
			std::cout << face.size() << std::endl;
		}
	} while (key != 'q');
	
	std::cout << "Hello, World!\n";
	return 0;
}
