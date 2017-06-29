//
//  _FaceTrack.hpp
//  FaceX
//
//  Created by SIFEN ZHONG on 10/8/15.
//
//

#ifndef FaceX__FaceTrack_hpp
#define FaceX__FaceTrack_hpp

#include "FaceTrack.hpp"

class FaceTrackParam {
public:
	int type;               /**< Type of object                           */
	int timeDet;            /**< Time between detections (seconds)        */
	int itol;               /**< Maximum number of iterations             */
	double ftol;            /**< Convergence tolerance                    */
	double clamp;           /**< Shape model clamping factor              */
	double init_lambda;     /**< Weight of pra model [0,1]                */
	double track_lambda;    /**< Weight of online model [0,1]             */
	double gamma;           /**< Weight of global vs local model [0,1]    */
	int init_type;          /**< 0=CLM only, 1=CLM+pra                    */
	int track_type;         /**< 0=CLM only, 1=CLM+atm, 2=CLM+atm+ksmooth */
	bool shape_predict;     /**< Use shape predictor for refinement?      */
	bool check_health;      /**< Check health of tracker                 */
	std::vector<int> init_wSize; /**< CLM search window sizes             */
	std::vector<int> track_wSize; /**< CLM search window sizes            */
	std::vector<cv::Mat> center; /**< Center view poses                   */
	std::vector<cv::Mat> visi;   /**< Patch visibility while tracking */
	
	cv::Mat atm_tri;        /**< Triangulation for ATM      */
	double atm_scale;       /**< Scale of ATM               */
	double atm_thresh;      /**< Threhold for ATM update    */
	int atm_ntemp;          /**< Number of templates in ATM */
	
	cv::Size ksmooth_size; /**< Size of ksmooth window         */
	double ksmooth_sigma;  /**< Variance of ksmooth kernel     */
	double ksmooth_noise;  /**< Noise variance of ksmooth      */
	double ksmooth_thresh; /**< Update threshold for ksmooth   */
	int ksmooth_ntemp;     /**< Number of templates in ksmooth */
	
	FaceTrackParam() {
		type = IO::MYFACETRACKERPARAMS;
		init_wSize.resize(3); init_wSize[0] = 11; init_wSize[1] = 9; init_wSize[2]=7;
		track_wSize.resize(1); track_wSize[0] = 7;
		timeDet = -1;
		itol = 10;
		ftol = 0.1;
		clamp = 3;
		init_lambda = 0.5;
		track_lambda = 0.01;
		gamma = 0.9;
		init_type = 0; //change to 0. PRA was not used in any case
		track_type = 0;
		shape_predict = false;
		check_health = true;
		
		atm_tri = cv::Mat();
		atm_scale = 0.25;
		atm_thresh = 100;
		atm_ntemp = 2;
		
		ksmooth_size = cv::Size(32,32);
		ksmooth_sigma = 0.01;
		ksmooth_noise = 0.1;
		ksmooth_thresh = 0.01;
		ksmooth_ntemp = 10;
		
		center.resize(9);
		center[0] = (cv::Mat_<double>(3,1) << 0, 0, 0);
		center[1] = (cv::Mat_<double>(3,1) << 0, 3.14/20, 0);
		center[2] = (cv::Mat_<double>(3,1) << 3.14/20, 3.14/20, 0);
		center[3] = (cv::Mat_<double>(3,1) << 3.14/20, 0, 0);
		center[4] = (cv::Mat_<double>(3,1) << 3.14/20, -3.14/20, 0);
		center[5] = (cv::Mat_<double>(3,1) << 0, -3.14/20, 0);
		center[6] = (cv::Mat_<double>(3,1) << -3.14/20, -3.14/20, 0);
		center[7] = (cv::Mat_<double>(3,1) << -3.14/20, 0, 0);
		center[8] = (cv::Mat_<double>(3,1) << -3.14/20, 3.14/20, 0);
		
		visi.resize(0);
	}
	void Save(const char* fname, bool binary = false);
	void Load(const char* fname, bool binary = false);
};

static FaceTrackParam FACETRACKPARAM;

#endif
