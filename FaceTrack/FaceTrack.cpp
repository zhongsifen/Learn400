//
//  FaceTrack.cpp
//  FaceX
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#include "FaceTrack.hpp"

bool
FaceTrack::load(std::string filename)
{
	return this->LoadW(filename);
}

bool
FaceTrack::track(cv::Mat& g, cv::Mat& shape, int& health)
{
	health = this->Track(g);		if (health < 1) return false;
	shape = this->_shape;
	
	return true;
}

bool
FaceTrack::show(cv::Mat& im, cv::Mat& shape, int& health)
{
	const cv::Scalar color= health >= 5 ? cv::Scalar(0x00, 0x00,0xFF) : cv::Scalar(0xFF, 0x00, 0x00);
	const int radius=2;
	const int thickness=1;
	
	int n = shape.total() / 2;
	double* s = (double*)shape.data;
	for (int i = 0; i < n; i++) {
		cv::circle(im, cv::Point2d(s[i], s[n+i]), radius, color, thickness);
	}
	
	return true;
}

bool
FaceTrack::reset()
{
	this->Reset();
	
	return true;
}
