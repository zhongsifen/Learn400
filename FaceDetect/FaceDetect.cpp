// FaceDetect.cpp

#include "FaceDetect.hpp"
using namespace cv;

bool
FaceDetect::load(std::string filename)
{
	this->cascade.load(filename);
	
	return true;
}

bool
FaceDetect::detect(cv::Mat& g, cv::Rect& face)
{
	std::vector<Rect> list;
	this->cascade.detectMultiScale(g, list);
	int n = (int)list.size();		if (n < 1) return false;
	face = list[0];
	for (int i=1; i<n; i++) {
		if (list[i].area() > face.area()) {
			face = list[i];
		}
	}
	
	return true;
}

bool
FaceDetect::detect_list(cv::Mat& g, std::vector<cv::Rect>& list)
{
	this->cascade.detectMultiScale(g, list);
	int n = (int)list.size();		if (n < 1) return false;
	
	return true;
}
