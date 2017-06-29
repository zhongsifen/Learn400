//
//  FaceTrack.cpp
//  FaceX
//
//  Created by SIFEN ZHONG on 7/8/15.
//
//

#include "FaceTrack/_FaceTrack.hpp"

FaceTrack::FaceTrack()
{
	this->status = 0;
	
	bool ret = this->load(RES_FACETRACK);		if (!ret) return;
	
	this->status = 1;
}

bool
FaceTrack::load(std::string filename)
{
	if (this->status < 0) return false;
	
	std::ifstream s(filename);		if (!s.is_open()) return false;

	bool readType = true;
	int type;
	if(readType){s >> type; assert(type == IO::MYFACETRACKER);}
	
	_clm.Read(s);
	_sinit.Read(s);
	_fcheck.Read(s);
	_spred.Read(s);
	_time = -1;
	int n = _clm._pdm.nPoints();
	mu_.create(2*n,1,CV_64F); cov_.create(2*n,2*n,CV_64F);
	covi_.create(2*n,2*n,CV_64F);
	
	this->status = 1;

	return true;
}

bool
FaceTrack::detect(cv::Mat& g, cv::Rect& face)
{
	if (this->status < 1) return false;

	cv::Rect head;
	bool ret = this->face_detect.detect(g, face, head);		if (!ret) return false;
	
	this->status = 2;
	
	return true;
}

//bool
//FaceTrack::shape(cv::Mat &g, cv::Rect &face, cv::Mat &shape, int &score)
//{
//	if (this->status < 2) return false;
//	
//	bool ret = this->face_shape.shape(g, face, shape, score);		if (!ret) return false;
//	bool rsize = true;
//	rect_ = _sinit.Update(g, shape, rsize);
//
//	this->status = 3;
//	
//	return true;
//}

bool
FaceTrack::shape(cv::Mat& g, cv::Rect& face, cv::Mat& shape, int& score)
{
	if (this->status < 2) return false;
 
	score = 10;
	bool rsize = true;
	//set parameters
	FaceTrackParam* p = &FACETRACKPARAM;
	_sinit.InitShape(g, shape, face);
	_clm._pdm.CalcParams(shape,_clm._plocal,_clm._pglobl);
	_clm.Fit(g,p->init_wSize,p->itol,p->clamp,p->ftol);
	_clm._pdm.CalcShape2D(shape,_clm._plocal,_clm._pglobl);
	rect_ = _sinit.Update(g, shape, rsize);
	
	this->status = 3;
	
	return true;
}

bool
FaceTrack::track(cv::Mat& g, cv::Mat& shape, int& score)
{
	if (this->status < 3) return false;
	this->_shape = shape;
 
	//set parameters
	FaceTrackParam* p = &FACETRACKPARAM;
	
	//re-initialise and fit
	bool rsize = true;
	cv::Rect face = _sinit.ReDetect(g);
		double tx = face.x - rect_.x,ty = face.y - rect_.y;
		_clm._pglobl.db(4,0) += tx; _clm._pglobl.db(5,0) += ty;
		int n = _shape.rows/2;
		cv::Mat sx = _shape(cv::Rect(0,0,1,n)),sy = _shape(cv::Rect(0,n,1,n));
		sx += tx; sy += ty;
		rsize = false;
		_clm.Fit(g,p->track_wSize,p->itol,p->clamp,p->ftol);
	_clm._pdm.CalcShape2D(_shape,_clm._plocal,_clm._pglobl);

		score = 10;
	
	if (score < 0) return false;
	
	//update models
	rect_ = _sinit.Update(g,_shape,rsize);
	if ((rect_.width == 0) || (rect_.height == 0)) {
		_time = -1;
		return FaceTracker::TRACKER_FAILED;
	}
	
	// update the 3D shape
	_clm._pdm.CalcParams(_shape,_clm._plocal,_clm._pglobl);
	
	shape = _shape;
	
	this->status = 3;
	
	return true;
}

bool
FaceTrack::reset()
{
	this->status = 1;
	
	return true;
}
