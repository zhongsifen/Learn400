//
//  FaceTrack.h
//  FaceX
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#ifndef __FaceX__FaceTrack__
#define __FaceX__FaceTrack__

#include "FaceX.hpp"
#include "FaceDetect/FaceDetect.hpp"
#include "FaceShape/FaceShape.hpp"
#include "myFaceTracker.hpp"
using namespace FACETRACKER;

const static std::string RES_FACETRACK(RES_FACEX + "face.mytracker");

class FaceTrack : myFaceTracker
{
protected:
	FaceDetect face_detect;
	FaceShape face_shape;
	
public:
	int status;
public:
	FaceTrack();
	bool load(std::string filename);
	bool detect(cv::Mat& g, cv::Rect& face);
	bool shape(cv::Mat& g, cv::Rect& face, cv::Mat& shape, int& score);
	bool track(cv::Mat& g, cv::Mat& shape, int& score);
	bool reset();
};

#endif /* defined(__FaceX__FaceTrack__) */
