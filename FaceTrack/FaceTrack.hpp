//
//  FaceTrack.h
//  FaceX
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#ifndef __FaceX__FaceTrack__
#define __FaceX__FaceTrack__

#include "_opencv2.hpp"
#include "myFaceTracker.hpp"

using namespace FACETRACKER;

class FaceTrack : myFaceTracker
{
public:
	bool load(std::string filename);
	bool track(cv::Mat& g, cv::Mat& shape, int& health);
	bool show(cv::Mat& im, cv::Mat& shape, int& health);
	bool reset();
};

#endif /* defined(__FaceX__FaceTrack__) */
