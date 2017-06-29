//
//  main.cpp
//  appFaceAvatar
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
	
	while (true) {
		ret = cap.read(f);		if (!ret) continue;
		cv::cvtColor(f, g, cv::COLOR_BGR2GRAY);
		w = f.clone();

		ret = track.detect(g, face);		if (!ret) continue;
		ret = track.shape(g, face, shape, score);		if (!ret) continue;

		track.show(w, shape, score);
		cv::imshow("facex", w);
		key = cv::waitKey(5);
		if (key == 'a') {
			avatar.add(f, shape, eyes);
			avatar.save(RES_FACEX + "zhong.avatar");
			break;
		}
		if (key == 's') {
			track.reset();
		}
	};
	
	cap.release();
	
	return 0;
}
