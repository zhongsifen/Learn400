//
//  main.cpp
//  appFaceAnimate
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#include "FaceX.hpp"
#include "FaceAvatar/FaceAvatar.hpp"
#include "FaceTrack/FaceTrack.hpp"
#include <iostream>

int main(int argc, const char * argv[]) {
	bool ret = false;
	char key = '\0';
	
	FaceTrack track;
//	FaceAvatar avatar(RES_FACEX + "zhong.avatar");
	FaceAvatar avatar(true);
	int index=2;
	cv::Mat a;
	avatar.set_index(index);
	avatar.get_a(a);
	cv::imshow("avatar", a);
	key = cv::waitKey();

	cv::Mat f, g, h, w;
	cv::Rect face;
	cv::Mat shape;
	int score;
	cv::Mat eyes;
	
	cv::VideoCapture cap;
	cap.open(0);		if (!cap.isOpened()) return -1;
	int m1 = 640, m2 = 480;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,  m1);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, m2);
	
//	int i=3; {
//		std::string fname(std::string(DATA_FACEX) + "cartoon/cartoon_" + (char)(i + '0') + ".png");
//		f = cv::imread(fname);
//		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
//		w = f.clone();
//		track.detect(g, face);
//		track.shape(g, face, shape, score);
//		FaceX::show_points(w, shape, COLOR_0000FF);
//		cv::imshow("avatar", w);
//		key = cv::waitKey();
//		ret = avatar.add(f, shape, eyes);		if (!ret) return -1;
//		ret = avatar.init(f, shape);		if (!ret) return -1;
//	}
	
	while (true) {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		w = f.clone();

		ret = track.detect(g, face);		if (!ret) continue;
		ret = track.shape(g, face, shape, score);		if (!ret) continue;
		FaceX::show_points(w, shape, COLOR_0000FF);
		cv::imshow("avatar", w);
		key = cv::waitKey(5);
		if (key == 'a') {
			ret = avatar.add(f, shape, eyes);		if (!ret) continue;
			avatar.get_index(index);
			avatar.get_a(a);
			cv::imshow("a", a);
			key = cv::waitKey();
			avatar.init(f, shape);
			break;
		}
		if (key == 's') {
			track.reset();
		}
	}
	
	while (true) {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, CV_BGR2GRAY);
		w = f.clone();

		ret = track.detect(g, face);		if (!ret) continue;
		ret = track.shape(g, face, shape, score);		if (!ret) continue;
		FaceX::show_points(w, shape, COLOR_0000FF);
		cv::imshow("facex", w);
		key = cv::waitKey(5);
		if (key == 'a') {
			break;
		}
	}
	
	cv::Mat draw(f.size(), f.type());
	do {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, CV_BGR2GRAY);
		w = f.clone();
		
		ret = track.track(g, shape, score);		if (!ret) continue;
		draw = f.clone();
		avatar.animate(draw, shape);
		FaceX::show_points(w, shape, COLOR_0000FF);
 		cv::imshow("facex", w);
		cv::imshow("avatar", draw);
		key = cv::waitKey(5);
	} while (key != 'q');
	
	cap.release();
	
	return 0;
}
