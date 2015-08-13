// facex.cpp

#include "FaceX.hpp"
using namespace cv;

//void* facex_new()
//{
//	facex_t* facex = new facex_t;
//	
//	return (void*)facex;
//}
//
//void facex_delete(void* p)
//{
//	facex_t* facex = (facex_t*)p;
//	delete facex;
//}
//
//int facex_open(void* p,
//			   std::string filename)
//{
//	facex_t* facex = (facex_t*)p;
//	int ret;
//	
//	ret = facex->cascade.load(filename);				if (!ret) return -1;
//	
//	return 0;
//}
//
//int facex_close(void* p)
//{
//	//	facex_t* facex = (facex_t*)p;
//	
//	return 0;
//}
