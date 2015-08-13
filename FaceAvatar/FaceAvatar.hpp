//
//  FaceAvatar.hpp
//  FaceX
//
//  Created by SIFEN ZHONG on 12/8/15.
//
//

#ifndef FaceX_FaceAvatar_hpp
#define FaceX_FaceAvatar_hpp

#include "myAvatar.hpp"
using namespace AVATAR;

class FaceAvatar : myAvatar
{
public:
	bool load(std::string filename, cv::Mat& a);
	bool init(cv::Mat& f, cv::Mat& shape);
	bool animate(cv::Mat& draw, cv::Mat& shape, cv::Mat& f);
};

#endif
