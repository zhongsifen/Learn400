// FaceDetect.cpp

#include "FaceDetect.hpp"
using namespace cv;

FaceDetect::FaceDetect()
{
	this->status = 0;

	bool ret = false;

	std::string filename(RES_FACEX + "haarcascade_frontalface_alt.xml");
	ret = this->load(filename);		if (!ret) return;
	
	this->status = 1;
}

bool
FaceDetect::load(std::string filename)
{
	if (this->status < 0) return false;

	bool ret = false;
	
	ret = this->cascade.load(filename);		if (!ret) return false;

	this->status = 1;

	return true;
}

bool
FaceDetect::detect(cv::Mat& g, cv::Rect& face, cv::Rect& head)
{
	if (this->status < 1) return false;

	std::vector<Rect> list;
	this->cascade.detectMultiScale(g, list);
	int n = (int)list.size();		if (n < 1) return false;
	face = list[0];
	for (int i=1; i<n; i++) {
		if (list[i].area() > face.area()) {
			face = list[i];
		}
	}
	this->head(face, head);
	
	this->status = 2;

	return true;
}

bool
FaceDetect::detect_roi(cv::Mat& g, cv::Rect& roi, cv::Rect& face)
{
	if (this->status < 1) return false;

	cv::Mat h = g(roi);
	
	std::vector<Rect> list;
	this->cascade.detectMultiScale(h, list);
	int n = (int)list.size();		if (n < 1) return false;
	face = list[0];
	for (int i=1; i<n; i++) {
		if (list[i].area() > face.area()) {
			face = list[i];
		}
	}

	face.x += roi.x;
	face.y += roi.y;
	
	this->status = 2;
	
	return true;
}

bool
FaceDetect::detect_list(cv::Mat& g, std::vector<cv::Rect>& list)
{
	if (this->status < 1) return false;
	
	this->cascade.detectMultiScale(g, list);
	int n = (int)list.size();		if (n < 1) return false;
	
	this->status = 2;
	
	return true;
}

bool
FaceDetect::head(cv::Rect& face, cv::Rect& head)
{
	int h = face.height;
	head.x = face.x;
	head.y = face.y - h * 5/16;
	head.width  = face.width;
	head.height = face.height + h * 7/16;
	
	return true;
}
