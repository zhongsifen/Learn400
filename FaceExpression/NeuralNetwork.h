//
//  NeuralNetwork.h
//  FaceExpressionRecognition
//
//  Created by Jianwei Xu on 30/04/2013.
//  Copyright (c) 2013 Jianwei Xu. All rights reserved.
//

#ifndef __FaceExpressionRecognition__NeuralNetwork__
#define __FaceExpressionRecognition__NeuralNetwork__

#include "_opencv2.hpp"
#include <iostream>

#include "Image.h"
#include "GaborFilter.h"

using namespace cv;
using namespace std;

class NeuralNetwork
{
    Image *trainpackage;
    Image *testpackage;
    const Size ngrid = Size(29,29);
    const int npca = 50;
    Mat neuweight;
public:
	cv::ml::SVM *svm;
    cv::PCA *pca;
    
    NeuralNetwork();
    NeuralNetwork(Image*, Image*);
    
    
    void svmTrainer();
    void validationReport();
    int svmSingleImagePrediction(cv::Mat&);
    
    void LRTrainer();
    void LRPredictor();
	int LRValidate(cv::Mat&, int);

private:
    Mat softmax(Mat&, Mat&, int);
    std::pair<cv::Mat, cv::Mat> unpackage(Image*, bool);
    std::pair<cv::Mat, cv::Mat> unpackage(Image*);
    void zeroMean(Mat&);
    void createPCA(Mat&);
    cv::Mat labeledVector(cv::Mat&);
    cv::Mat PCAlabeledVector(cv::Mat&,bool);
    //cv::Mat PCAlabeledVector(cv::Mat&);
    
};

#endif /* defined(__FaceExpressionRecognition__NeuralNetwork__) */
