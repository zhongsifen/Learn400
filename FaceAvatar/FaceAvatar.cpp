//
//  FaceAvatar.cpp
//  FaceX
//
//  Created by SIFEN ZHONG on 12/8/15.
//
//

#include "FaceAvatar.hpp"

bool
FaceAvatar::load(std::string filename, cv::Mat& a)
{
	int index = 0;
	
	bool ret = this->Load(filename, true);		if (!ret) return false;
	this->setAvatar(index);
	a = this->Thumbnail(index);
	
	return true;
}

bool
FaceAvatar::init(cv::Mat& f, cv::Mat& shape)
{
	return this->Initialise(f, shape);
}

bool
FaceAvatar::animate(cv::Mat& draw, cv::Mat& shape, cv::Mat& f)
{
	return this->Animate(draw, f, shape);
}