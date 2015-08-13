// FaceDetect.hpp

#ifndef _FaceDetect_
#define _FaceDetect_ 1

#include "FaceX.hpp"
//#include <opencv2/core/core.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/objdetect/objdetect.hpp>

class FaceDetect
{
protected:
	cv::CascadeClassifier cascade;
	
public:
	bool load(std::string filename);
	bool detect(cv::Mat& g, cv::Rect& face);
	bool detect_list(cv::Mat& g, std::vector<cv::Rect>& list);
}
;

#endif // _FaceDetect_
