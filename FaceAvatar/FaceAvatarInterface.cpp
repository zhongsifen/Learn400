//
//  FaceAvatarInterface.cpp
//  FaceX
//
//  Created by SIFEN ZHONG on 17/10/15.
//
//

#include "FaceAvatarInterface.hpp"

//
// shape
//

int16_t _w(double f)
{
	return (int16_t)round(f * 4.0);
}
double _f(int16_t w)
{
	return (double)w / 4.0;
}

void
_w(cv::Mat& f, int16_t w[], int& l)
{
	l = (int)f.total();
	int n = l/2;
	for (int i=0; i<n; i++) {
		w[i*2]   = _w(f.at<double>(i));
		w[i*2+1] = _w(f.at<double>(i+n));
	}
	
}

void
_f(cv::Mat& f, int16_t w[], int l)
{
	f = cv::Mat(cv::Size(l, 1), CV_64FC1);
	int n = l/2;
	for (int i=0; i<n; i++) {
		f.at<double>(i)   = _f(w[i*2]);
		f.at<double>(i+n) = _f(w[i*2+1]);
	}
	
}

bool
save_shape(std::ofstream& s, int16_t w[], int& l)
{
	s << l << std::endl;
	for (int i=0; i<l; i+=2) {
		s << w[i] << '\t' << w[i+1] << std::endl;
	}
	
	return true;
}

bool
load_shape(std::ifstream& s, int16_t w[], int& l)
{
	s >> l;
	for (int i=0; i<l; i+=2) {
		s >> w[i] >> w[i+1];
	}
	
	return true;
}

bool
save_shape(std::ofstream& s, cv::Mat& shape)
{
	int16_t w[256];
	int l = (int)shape.total();
	_w(shape, w, l);
	save_shape(s, w, l);

	return true;
}

bool
load_shape(std::ifstream& s, cv::Mat& shape)
{
	int16_t w[256];
	int l;
	load_shape(s, w, l);
	shape = cv::Mat(cv::Size(l, 1), CV_64FC1);
	_f(shape, w, l);
	
	return true;
}

bool
save_shape(std::string filename, cv::Mat& shape)
{
	std::ofstream s(filename);		if (!s.is_open()) return false;
	bool ret = save_shape(s, shape);		if (!ret) return false;
	
	return true;
}

bool
load_shape(std::string filename, cv::Mat& shape)
{
	std::ifstream s(filename);		if (!s.is_open()) return false;
	bool ret = load_shape(s, shape);		if (!ret) return false;
	
	return true;
}
