// CSIRO has filed various patents which cover the Software.

// CSIRO grants to you a license to any patents granted for inventions
// implemented by the Software for academic, research and non-commercial
// use only.

// CSIRO hereby reserves all rights to its inventions implemented by the
// Software and any patents subsequently granted for those inventions
// that are not expressly granted to you.  Should you wish to license the
// patents relating to the Software for commercial use please contact
// CSIRO IP & Licensing, Gautam Tendulkar (gautam.tendulkar@csiro.au) or
// Nick Marsh (nick.marsh@csiro.au)

// This software is provided under the CSIRO OPEN SOURCE LICENSE
// (GPL2) which can be found in the LICENSE file located in the top
// most directory of the source code.

// Copyright CSIRO 2013

#ifndef _AVATAR_Avatar_h_
#define _AVATAR_Avatar_h_

#include "_opencv2.hpp"
#include <fstream>

namespace AVATAR {
	
	class IO {
	public:
		enum { MYAVATAR = 0, MYAVATARPARAMS, DUMMY_LAST_DONT_USE };
	};
	
	class IOBinary : public IO {
	public:
		enum{ MYAVATAR = DUMMY_LAST_DONT_USE + 1, MYAVATARPARAMS };
	};
	
	typedef cv::Mat_<cv::Vec<uint8_t,3> > BGRImage;
	typedef std::vector<cv::Point_<double> > PointVector;
	
	//============================================================================
	/**
	 Base class for Avatar
  */
	class Avatar {
	public:
		virtual ~Avatar();
		
		cv::Mat _shape; /**< Current avatar's shape */
		
		virtual bool
		Animate(cv::Mat &draw,        //rgb image to draw on
				cv::Mat &image,       //image of user
				cv::Mat &shape,       //user's facial shape
				void* params=NULL)=0; //additional parameters
		virtual bool
		Initialise(cv::Mat& im,
				   cv::Mat& shape,
				   void* params=NULL)=0;
		virtual void
		Read(std::ifstream &s,        //file stream to read from
			 bool readType = true)=0;
		virtual void
		ReadBinary(std::ifstream &s,        //file stream to read from
				   bool readType = true)=0;
		virtual void
		Write(std::ofstream &s, //file stream to write to
			  bool binary = false)=0;
		// virtual void
		// Read(std::ifstream &s,        //file stream to read from
		// 	 bool readType = true)=0; //read type?
		
		// virtual void
		// Write(std::ofstream &s)=0;    //file stream to write to
		
		virtual int numberOfAvatars() const=0;    // number of avatars
		virtual void setAvatar(int idx)=0; // setAvatar
		
		virtual cv::Mat       //thumbnail image of avatar
		Thumbnail(int idx)=0; //avatar index
		
		bool
		Load(std::string fname, bool binary = false)
		{
			std::ifstream s;
			if (!binary) {
				s.open(fname);		if (!s.is_open()) return false;
				this->Read(s);
			}
			else {
				s.open(fname, std::ios::binary);	if (!s.is_open()) return false;
				this->ReadBinary(s);
			}
			s.close();
			
			return true;
		}
		
		bool
		Save(std::string fname, bool binary = false) { //file to save predictor model to
			std::ofstream s;
			if(!binary) s.open(fname);
			else s.open(fname, std::ios::binary);
			
			assert(s.is_open()); this->Write(s, binary); s.close();
			
			return true;
		}
		// void
		// Load(const char* fname){ //file containing predictor model
		//   std::ifstream s(fname); assert(s.is_open()); this->Read(s); s.close();
		//   return;
		// }
		// void
		// Save(const char* fname){ //file to save predictor model to
		//   std::ofstream s(fname); assert(s.is_open()); this->Write(s); s.close();
		//   return;
		// }
	};
	//============================================================================
	/** IO functions */
	Avatar* //contains pointer to avatar obj on return, null on failure
	LoadAvatar(std::string fname); //filename containing avatar model
	
	void*  //contains pointer to avatar param obj on return, null on failure
	LoadAvatarParams(const char* fname); //file containing avatar params
	//============================================================================
	
	// std::string DefaultAvatarParamsPathname();
	// void *LoadAvatarParams();
}
#endif
