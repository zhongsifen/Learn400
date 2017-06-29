//
//  main.cpp
//  appFaceExpression
//
//  Created by SIFEN ZHONG on 16/8/15.
//
//

#include "Image.h"
#include "GaborFilter.h"
#include "NeuralNetwork.h"

#include "FaceDetect.hpp"

string csv = string("/Users/jianweixu/Documents/Project/Face/DataSet/dataset.csv");

int main(int argc, const char * argv[])
{
	bool ret = false;
	char key = '\0';

	FaceDetect detect;

	VideoCapture cap;
	cap.open(0);		if (!cap.isOpened()) return -1;
	cap.set(CV_CAP_PROP_FRAME_WIDTH,  640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	
	Image *image = new Image(argc,csv);
	
	std::pair<Image*, Image*> workdata =image->split();
	Image *train = workdata.first;
	Image *test = workdata.second;
	
	cout<<"Original Images Load Complete"<<endl;
	NeuralNetwork *neu = new NeuralNetwork(train,test);
	neu->LRTrainer();
	neu->LRPredictor();
	
	Image *img = new Image();
	Mat f, g, h;
	Rect r;
	Rect head;
	int label;
	do {
		cap.read(f);
		cvtColor(f, g, COLOR_BGR2GRAY);
		ret = detect.detect(g, r, head);		if (!ret) continue;
		h = g(r);
		rectangle(f, r,  Scalar(0x00, 0xFF, 0x00));
		
		label = neu->LRValidate(h, img->nexpression);
		putText (f,(img->stringLabel(label)).c_str(),cvPoint(000,440), FONT_HERSHEY_COMPLEX_SMALL, 6, cvScalar(50,50,50), 1, CV_AA);

		imshow("expression", f);
		key = waitKey(5);
	} while (key != 'q');
	
	return 0;
}
