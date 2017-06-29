// FaceDetect.hpp

#ifndef _FaceDetect_
#define _FaceDetect_ 1

#include "FaceX.hpp"

class FaceDetect {
protected:
	int status;
	cv::CascadeClassifier cascade;
public:
	FaceDetect();
	bool load(std::string filename);
	bool detect(cv::Mat& g, cv::Rect& face, cv::Rect& head);
	bool detect_roi(cv::Mat& g, cv::Rect& roi, cv::Rect& face);
	bool detect_list(cv::Mat& g, std::vector<cv::Rect>& list);
	bool head(cv::Rect& face, cv::Rect& head);
};

#endif // _FaceDetect_
