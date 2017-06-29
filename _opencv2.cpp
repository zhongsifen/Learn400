// _opencv.cpp

#include "_opencv2.hpp"

#if defined(_DEBUG)
#pragma comment (lib, "opencv_core300d.lib") 
#pragma comment (lib, "opencv_highgui300d.lib") 
#pragma comment (lib, "opencv_videoio300d.lib")
#pragma comment (lib, "opencv_imgproc300d.lib") 
#pragma comment (lib, "opencv_objdetect300d.lib") 
#else
#pragma comment (lib, "opencv_core300.lib") 
#pragma comment (lib, "opencv_highgui300.lib") 
#pragma comment (lib, "opencv_videoio300.lib")
#pragma comment (lib, "opencv_imgproc300.lib") 
#pragma comment (lib, "opencv_objdetect300.lib") 
#endif