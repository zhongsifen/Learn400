//
//  FaceAvatar.hpp
//  FaceX
//
//  Created by SIFEN ZHONG on 12/8/15.
//
//

#ifndef FaceX_FaceAvatar_hpp
#define FaceX_FaceAvatar_hpp

#include "FaceX.hpp"
#include "myAvatar.hpp"

class FaceAvatar : myAvatar
{
public:
	int status;
	int index;
public:
	FaceAvatar();
	FaceAvatar(bool binary);
	FaceAvatar(std::string filename);
	bool get_index(int& index);
	bool set_index(int& index);
	bool get_a(cv::Mat& a);
	bool load(std::ifstream& s);
	bool load_binary(std::ifstream& s);
	bool load_binary(std::string filename);
	bool save(std::ofstream& s);
	bool load(std::string filename);
	bool save(std::string filename);
	bool init(cv::Mat& f, cv::Mat& shape);
	bool animate(cv::Mat& draw, cv::Mat& shape);
	bool add(cv::Mat& f, cv::Mat& shape, cv::Mat& eyes);
	
	bool animate_p(cv::Mat& draw, cv::Mat& shape);
};

#endif
