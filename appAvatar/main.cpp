//
//  main.cpp
//  appAvatar
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#include "FaceX.hpp"
#include "FaceAvatar.hpp"
#include "FaceTrack.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
	bool ret = false;
	char key = '\0';
	
	FaceTrack track;
	ret = track.load(RES_FACEX + "face.mytracker");		if (!ret) return -1;
	FaceAvatar avatar;
	cv::Mat a;
	ret = avatar.load(RES_FACEX + "CI2CV.avatar.binary", a);		if (!ret) return -1;
	cv::imshow("avatar", a);
	key = cv::waitKey();

	cv::VideoCapture cap;
	cap.open(0);		if (!cap.isOpened()) return -1;
	int m1 = 640, m2 = 480;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,  m1);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, m2);
	cv::Mat f, g, h, w;
	cv::Mat shape;
	int health;
	
	void *avatar_params    = 0;
	
	do {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		equalizeHist(g, g);
		w = f.clone();
		
		track.track(g, shape, health);
		std::cout << health << std::endl;
		if (health > 0) {
			track.show(w, shape, health);
			cv::imshow("facex", w);
			key = cv::waitKey(5);
			if (key == 'a') {
				avatar.init(f, shape);
			}
			if (key == 's') {
				track.reset();
			}
		} else {
			//			tracker->Reset();
			cv::imshow("not detected", w);
			key = cv::waitKey(5);
		}
	} while (key != 'a');
	
	cv::Mat draw(f.size(), f.type());
	do {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, CV_BGR2GRAY);
		w = f.clone();
		
		track.track(g, shape, health);
		if (health > 0) {
			draw = cv::Scalar(0x3F, 0x00, 0x3F);
			avatar.animate(draw, shape, f);
			cv::imshow("Avatar", draw);
		} else {
			track.reset();
		}
		cv::imshow("facex", w);
		cv::imshow("avatar", draw);
		key = cv::waitKey(5);
	} while (key != 'q');
	
	cap.release();
	
	return 0;
}
