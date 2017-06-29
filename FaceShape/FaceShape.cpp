//
//  FaceShape.cpp
//  FaceX
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#include "_FaceShape.hpp"

FaceShape::FaceShape()
{
	this->status = 0;
	
	bool ret = this->load(RES_FACETRACK);		if (!ret) return;
	
	this->status = 1;
}

bool
FaceShape::load(std::string filename)
{
	std::ifstream s(filename);		if (!s.is_open()) return false;

	if (this->status < 0) return false;
	
	bool readType = true;
	int type;
	if(readType){s >> type; assert(type == IO::MYFACETRACKER);}
	
	_clm.Read(s);
	_sinit.Read(s);
	
	this->status = 1;

	return true;
}

bool
FaceShape::detect(cv::Mat& g, cv::Rect& face)
{
	cv::Rect head;
	bool ret = this->face_detect.detect(g, face, head);		if (!ret) return false;
	
	return true;
}

bool
FaceShape::shape(cv::Mat& g, cv::Rect& face, cv::Mat& shape, int& score)
{
	if (this->status < 1) return false;
 
	score = 10;
	//set parameters
	FaceTrackParam* p = &FACETRACKPARAM;
	_sinit.InitShape(g, shape, face);
	_clm._pdm.CalcParams(shape,_clm._plocal,_clm._pglobl);
	_clm.Fit(g,p->init_wSize,p->itol,p->clamp,p->ftol);
	_clm._pdm.CalcShape2D(shape,_clm._plocal,_clm._pglobl);

	this->status = 2;
	
	return true;
}

bool
FaceShape::reset()
{
	this->status = 1;
	
	return true;
}
