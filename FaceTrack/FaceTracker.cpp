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

#include "IO.hpp"
#include "myFaceTracker.hpp"
#include "ShapeModel.hpp"
#include <opencv2/imgproc/imgproc.hpp>

using namespace FACETRACKER;
using namespace std;
//===========================================================================
FaceTrackerParams::~FaceTrackerParams()
{

}
//===========================================================================
FaceTracker::~FaceTracker()
{

}
//============================================================================
// Compute the vectors for each axis i.e. x-axis, y-axis then
// z-axis. Alternatively the pitch, yaw, roll
cv::Mat_<double>
FACETRACKER::pose_axes(const Pose &pose)
{
  cv::Mat_<double> rv(3,3);
  Euler2Rot(rv, pose.pitch, pose.yaw, pose.roll);  
  return rv;
}
