// facex.cpp

#include "FaceX.hpp"
#include <vector>

bool FaceX::read_mat(std::ifstream& s, cv::Mat& a)
{
	int r,c,t; s >> r >> c >> t;
	a = cv::Mat(r,c,t);
	switch (a.type()) {
		case CV_64FC1: {
			cv::MatIterator_<double> i1 = a.begin<double>(),i2 = a.end<double>();
			while(i1 != i2)s >> *i1++;
		} break;
		case CV_32FC1: {
			cv::MatIterator_<float> i1 = a.begin<float>(),i2 = a.end<float>();
			while(i1 != i2)s >> *i1++;
		} break;
		case CV_32SC1:
		{
			cv::MatIterator_<int> i1 = a.begin<int>(),i2 = a.end<int>();
			while(i1 != i2)s >> *i1++;
		} break;
		case CV_8UC1:
		{
			cv::MatIterator_<uchar> i1 = a.begin<uchar>(),i2 = a.end<uchar>();
			while(i1 != i2)s >> *i1++;
		} break;
		default: {
			return false;
		}
	}
	
	return true;
}

bool FaceX::write_mat(std::ofstream& s, cv::Mat& a)
{
	s << a.rows << " " << a.cols << " " << a.type() << " ";
	switch(a.type()){
  case CV_64FC1:
		{
			s.precision(10); s << std::scientific;
			cv::MatIterator_<double> i1 = a.begin<double>(),i2 = a.end<double>();
			while(i1 != i2)s << *i1++ << " ";
		}break;
  case CV_32FC1:
		{
			cv::MatIterator_<float> i1 = a.begin<float>(),i2 = a.end<float>();
			while(i1 != i2)s << *i1++ << " ";
		}break;
  case CV_32SC1:
		{
			cv::MatIterator_<int> i1 = a.begin<int>(),i2 = a.end<int>();
			while(i1 != i2)s << *i1++ << " ";
		}break;
  case CV_8UC1:
		{
			cv::MatIterator_<uchar> i1 = a.begin<uchar>(),i2 = a.end<uchar>();
			while(i1 != i2)s << *i1++ << " ";
		}break;
		default: {
			return false;
		}
	}
	
	return true;
}

bool
FaceX::show_rect(cv::Mat& im, cv::Rect& rect, const cv::Scalar color)
{
	rectangle(im, rect, color);
	
	return true;
}

bool
FaceX::show_points(cv::Mat& im, std::vector<cv::Point>& points, cv::Scalar color)
{
	const int radius=2;
	const int thickness=1;
	
	int size = (int)points.size();
	for (int i = 0; i < size; i++) {
		cv::circle(im, points[i], radius, color, thickness);
	}
	
	return true;
}

bool
FaceX::show_points(cv::Mat& im, cv::Mat& points, cv::Scalar color)
{
	const int radius=2;
	const int thickness=1;
	
	int n = (int)points.total() / 2;
	double* s = (double*)points.data;
	for (int i = 0; i < n; i++) {
		cv::circle(im, cv::Point(s[i], s[n+i]), radius, color, thickness);
	}
	
	return true;
}

bool
FaceX::show_shape(cv::Mat& im, cv::Mat& shape, int health)
{
	const cv::Scalar color= health >= 10 ? cv::Scalar(0x00, 0x00,0xFF) : cv::Scalar(0xFF, 0x00, 0x00);
	const int radius=2;
	const int thickness=1;
	
	int n = (int)shape.total() / 2;
	std::vector<cv::Point> points(n);
	double* s = (double*)shape.data;
	for (int i = 0; i < n; i++) {
		points[i] = cv::Point(s[i], s[n+i]);
		cv::circle(im, cv::Point(s[i], s[n+i]), radius, color, thickness);
	}
	
	return true;
}
