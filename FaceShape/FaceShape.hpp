//
//  FaceShape.h
//  FaceX
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#ifndef __FaceX__FaceShape__
#define __FaceX__FaceShape__

#include "FaceX.hpp"
#include "FaceDetect/FaceDetect.hpp"
#include "FaceShape/myFaceTracker.hpp"
using namespace FACETRACKER;

class FaceShape
{
	SInit _sinit;                  /**< Face detector and shape initialiser */
	CLM _clm;                      /**< Constrained Local Model             */
protected:
	FaceDetect face_detect;
	
public:
	int status;
public:
	FaceShape();
	bool load(std::string filename);
	bool save(std::string filename);
	bool detect(cv::Mat& g, cv::Rect& face);
	bool shape(cv::Mat& g, cv::Rect& face, cv::Mat& shape, int& score);
	bool reset();
};

#endif /* defined(__FaceX__FaceShape__) */
