// facex.h

#ifndef _facex_h_
#define _facex_h_ 1

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

const static std::string FACEXP_RES("/Users/zhongsifen/res/facexp/zhongsifen/");

void* facex_new()
;
void facex_delete(void* p)
;
int facex_open(void* p,
			   std::string filename)
;
int facex_close(void* p)
;

int facex_detect(void* p,
				 cv::Mat& g,
				 cv::Rect& face)
;
#endif // _facex_h_
