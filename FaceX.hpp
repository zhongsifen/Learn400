// FaceX.hpp

#ifndef _facex_h_
#define _facex_h_ 1

#include "_opencv2.hpp"	
#include <iostream>
#include <fstream>

//#define it at<int>
#define db at<double>

const cv::Scalar COLOR_0000FF = cv::Scalar(0x00, 0x00, 0xFF);
const cv::Scalar COLOR_00FF00 = cv::Scalar(0x00, 0xFF, 0x00);
const cv::Scalar COLOR_FF0000 = cv::Scalar(0xFF, 0x00, 0x00);

const static std::string RES_FACEX("/Users/zhongsifen/res/facex/");
const static std::string DATA_FACEX("/Users/zhongsifen/Data/facex/");

class FaceX
{
public:
	static bool read_mat(std::ifstream& s, cv::Mat& a);
	static bool write_mat(std::ofstream& s, cv::Mat& a);
	static bool show_rect(cv::Mat& im, cv::Rect& rect, cv::Scalar color);
	static bool show_points(cv::Mat& im, std::vector<cv::Point>& points, cv::Scalar color);
	static bool show_points(cv::Mat& im, cv::Mat& points, cv::Scalar color);
	static bool show_shape(cv::Mat& im, cv::Mat& shape, int health=10);
};

#endif // _facex_h_
