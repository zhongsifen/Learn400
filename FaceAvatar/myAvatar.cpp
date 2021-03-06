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

#include "myAvatar.hpp"
#define it at<int>
#define db at<double>
using namespace std;

cv::Mat_<double>
vectorise_points(const std::vector<cv::Point_<double> > &points)
{
	const int n = (int)points.size();
	
	cv::Mat_<double> rv(n * 2, 1);
	
	for (int i = 0; i < n; i++) {
		rv(i + 0) = points[i].x;
		rv(i + n) = points[i].y;
	}
	
	return rv;
}

//=============================================================================
int
myAvatar::numberOfAvatars() const
{
  return int(_images.size());
}
//=============================================================================
void
myAvatar::setAvatar(int idx) {
  if ((idx < 0) || (idx >= int(_images.size())))
	  _idx = 0;
  else 
    _idx = idx;
} 
//=============================================================================
cv::Mat
myAvatar::Thumbnail(int idx)
{
  if((idx < 0) || (idx >= int(_images.size())))return cv::Mat();
  cv::Mat thumb = _images[idx].clone(); return thumb;
}
//=============================================================================
//=============================================================================
cv::Mat 
myAvatar::AnimateShape(cv::Mat &shape,
		       int scale_eyes)
{
  //sanity check
  if((plocal_.rows != _pdm.nModes()) || (plocal_.cols != 1))
    plocal_.create(_pdm.nModes(),1,CV_64F);
  if((pglobl_.rows != 6) || (pglobl_.cols != 1))pglobl_.create(6,1,CV_64F);
  if((_shape.rows != 2*_pdm.nPoints()) || (_shape.cols != 1))
    _shape.create(2*_pdm.nPoints(),1,CV_64F);
  if((s3D_.rows != 3*_pdm.nPoints()) || (s3D_.cols != 1))
    s3D_.create(3*_pdm.nPoints(),1,CV_64F);
  if((p_.rows != _pdm.nModes()) || (p_.cols != 1))
    p_.create(_pdm.nModes(),1,CV_64F);
  
  //calculate user parameters
  _user.copyTo(_pdm._M);
  _pdm.CalcParams(shape,plocal_,pglobl_);

  //map parameters to avatar
  p_ = _reg[_idx]*plocal_;
  
  //scale eyelids (blinking for different eye sizes)
  if(scale_eyes == 1){
    int n = _pdm.nPoints();
    double v = 
      ((_user.db(41+n,0)-_user.db(37+n,0))+
       (_user.db(40+n,0)-_user.db(38+n,0))+
       (_user.db(47+n,0)-_user.db(43+n,0))+
       (_user.db(46+n,0)-_user.db(44+n,0)))/
      ((_shapes[_idx].db(41+n,0)-_shapes[_idx].db(37+n,0))+
       (_shapes[_idx].db(40+n,0)-_shapes[_idx].db(38+n,0))+
       (_shapes[_idx].db(47+n,0)-_shapes[_idx].db(43+n,0))+
       (_shapes[_idx].db(46+n,0)-_shapes[_idx].db(44+n,0)));    
    if(p_.db(0,0) > 0)p_.db(0,0) /= 0.5*v;
    if(p_.db(1,0) < 0)p_.db(1,0) /= 0.5*v;
  }else if(scale_eyes < 0){for(int i = 0; i < 4; i++)p_.db(i,0) = 0.0;}

  //get avatar shape
  _shapes[_idx].copyTo(_pdm._M); _pdm.CalcShape3D(s3D_,p_);
  FACETRACKER::Align3Dto2DShapes(pglobl_.db(0,0),pglobl_.db(1,0),
				 pglobl_.db(2,0),pglobl_.db(3,0),
				 pglobl_.db(4,0),pglobl_.db(5,0),
				 shape,s3D_);
  _pdm.CalcShape2D(_shape,p_,pglobl_); return _shape;
}
//=============================================================================
void myAvatar::WarpTexture(cv::Mat &src_pts,cv::Mat &dst_pts,
			 vector<cv::Mat> &src_img,
			 vector<cv::Mat> &dst_img,
			 cv::Mat &tri)
{
  int l,n = src_pts.rows/2,c = src_img.size();

  //check points are within image
  {
    int rows = src_img[0].rows,cols = src_img[0].cols;
    for(int i = 0; i < n; i++){
      if((src_pts.db(i  ,0) < 0) || (src_pts.db(i  ,0) >= cols) ||
	 (src_pts.db(i+n,0) < 0) || (src_pts.db(i+n,0) >= rows) ||
	 (dst_pts.db(i  ,0) < 0) || (dst_pts.db(i  ,0) >= cols) ||
	 (dst_pts.db(i+n,0) < 0) || (dst_pts.db(i+n,0) >= rows))return;
    }
  }
  //normalize oral cavity colors
  {
    //get bounding box
    double xmin=src_pts.db(0,0),xmax = src_pts.db(0,0);
    double ymin=src_pts.db(n,0),ymax = src_pts.db(n,0);
    for(int i = 1; i < n; i++){
      if(xmin > src_pts.db(i,0))xmin = src_pts.db(i,0);
      if(xmax < src_pts.db(i,0))xmax = src_pts.db(i,0);
      if(ymin > src_pts.db(n+i,0))ymin = src_pts.db(n+i,0);
      if(ymax < src_pts.db(n+i,0))ymax = src_pts.db(n+i,0);
    }
    cv::Rect R(floor(xmin),floor(ymin),
	       ceil(xmax-xmin),ceil(ymax-ymin));
    if(R.width*R.height > 1){
      cv::Mat M(R.height,3*R.width,CV_8U);
      for(int i = 0; i < 3; i++){
	cv::Mat m = M(cv::Rect(i*R.width,0,R.width,R.height));
	cv::Mat I = src_img[i](R);
	I.copyTo(m);
      }
      cv::normalize(M,M,0,255,cv::NORM_MINMAX);
      for(int i = 0; i < 3; i++){
	cv::Mat m = M(cv::Rect(i*R.width,0,R.width,R.height));
	cv::Mat I = src_img[i](R);
	m.copyTo(I);
      }
    }
  }
#ifdef _OPENMP
#pragma omp parallel for
#endif
  for(l = 0; l < tri.rows; l++){
    int i,j,k,xmin,ymin,xmax,ymax;
    double dx[3],dy[3],sx[3],sy[3],x,y;
    cv::Mat A(2,3,CV_64F),Xi(3,3,CV_64F),X(3,3,CV_64F),Y(3,2,CV_64F);
    for(i = 0; i < 3; i++){
      j = tri.it(l,i);
      sx[i] = src_pts.db(j,0); sy[i] = src_pts.db(j+n,0);
      dx[i] = dst_pts.db(j,0); dy[i] = dst_pts.db(j+n,0);
    }
    xmax = (int)ceil( max(max(dx[0],dx[1]),dx[2]));
    ymax = (int)ceil( max(max(dy[0],dy[1]),dy[2]));
    xmin = (int)floor(min(min(dx[0],dx[1]),dx[2]));
    ymin = (int)floor(min(min(dy[0],dy[1]),dy[2]));
    if( (xmin < 0) || (xmax >= dst_img[0].cols) || 
	(ymin < 0) || (ymax >= dst_img[0].rows) ||
       (((dx[1] - dx[0])*(dy[0] - dy[2])+(dy[1] - dy[0])*(dx[2] - dx[0]))/
	 ((sx[1] - sx[0])*(sy[0] - sy[2])+(sy[1] - sy[0])*(sx[2] - sx[0]))<=0)){
      continue;
    }
    for(i = 0; i < 3; i++){
      X.db(i,0) = dx[i]; X.db(i,1) = dy[i]; X.db(i,2) = 1.0;
      Y.db(i,0) = sx[i]; Y.db(i,1) = sy[i];
    }
    cv::invert(X,Xi,cv::DECOMP_SVD); A = (Xi*Y).t();
    for(i = ymin; i <= ymax; i++){
      for(j = xmin; j <= xmax; j++){
	if(FACETRACKER::sameSide(j,i,dx[0],dy[0],dx[1],dy[1],dx[2],dy[2]) &&
	   FACETRACKER::sameSide(j,i,dx[1],dy[1],dx[0],dy[0],dx[2],dy[2]) &&
	   FACETRACKER::sameSide(j,i,dx[2],dy[2],dx[0],dy[0],dx[1],dy[1])){
	  x = A.db(0,0)*j + A.db(0,1)*i + A.db(0,2);
	  y = A.db(1,0)*j + A.db(1,1)*i + A.db(1,2);
	  for(k = 0; k < c; k++)
	    dst_img[k].at<uchar>(i,j) = 
	      (uchar)(FACETRACKER::bilinInterp(src_img[k],x,y)+0.5);
	}
      }
    }
  }return;
}
//=============================================================================
void myAvatar::GetIdxPts(cv::Mat &shape,cv::Mat &idx,cv::Mat &ishape,bool twoD)
{
  if((ishape.rows != 2*idx.rows) || (ishape.cols != 1))
    ishape.create(2*idx.rows,1,CV_64F);
  int d; if(twoD)d = 2; else d = 3;
  for(int i = 0; i < idx.rows; i++){
    ishape.db(i         ,0) = shape.db(idx.it(i,0)             ,0); 
    ishape.db(i+idx.rows,0) = shape.db(idx.it(i,0)+shape.rows/d,0);
  }return;
}
//=============================================================================
void myAvatar::GetWidthHeight(cv::Mat &shape,cv::Mat &idx,double &w,double &h)
{
  cv::Mat p; this->GetIdxPts(shape,idx,p,false);
  cv::Rect rect = this->GetBoundingBox(p);
  w = rect.width; h = rect.height; return;
}
//=============================================================================
void 
myAvatar::GetPupils(cv::Mat &im,
		    cv::Mat &shape,
		    cv::Point &pl,
		    cv::Point &pr)
{
  pl = this->GetPupil(im,shape,_lpupil[_idx].idx,_lpupil[_idx].tri);
  pr = this->GetPupil(im,shape,_rpupil[_idx].idx,_rpupil[_idx].tri);
  return;
}
//=============================================================================
cv::Point 
myAvatar::GetPupil(cv::Mat &im,
		   cv::Mat &shape,
		   cv::Mat &idx,
		   cv::Mat &tri)
{
  cv::Mat pt = epts_(cv::Rect(0,0,1,2*idx.rows));
  for(int i = 0; i < idx.rows; i++){
    pt.db(i         ,0) = shape.db(idx.it(i,0)             ,0); 
    pt.db(i+idx.rows,0) = shape.db(idx.it(i,0)+shape.rows/2,0);
  }
  cv::Rect rect = this->GetBoundingBox(pt); double vx=0,vy=0,sum=0;
  if((rect.width <= 0) || (rect.height <= 0)){
    cv::Point p(0,0);
    for(int i = 0; i < idx.rows; i++){
      p.x += pt.db(i,0); p.y += pt.db(i+idx.rows,0);
    }
    p.x /= idx.rows; p.y /= idx.rows; return p;
  }
  if((rect.x < 0) || (rect.x + rect.width >= im.cols) ||
     (rect.y < 0) || (rect.y + rect.height >= im.rows)){
    cv::Point p; p.x = 0; p.y = 0;
    for(int i = 0; i < idx.rows; i++){
      p.x += pt.db(i,0); p.y += pt.db(i+idx.rows,0);
    }
    p.x /= idx.rows; p.y /= idx.rows; return p;
  }
  cv::Mat imn = im(rect).clone(); equalizeHist(imn,imn);
  for(int y = rect.y; y < rect.y+rect.height; y++){
    for(int x = rect.x; x < rect.x+rect.width; x++){
      if(FACETRACKER::isWithinTri(x,y,tri,pt)>=0){
	double v = exp(-0.01*double(imn.at<uchar>(y-rect.y,x-rect.x)));
	vx += v*x; vy += v*y; sum += v;
      }
    }
  }
  cv::Point p;
  if(sum == 0){
    p.x = 0; p.y = 0;
    for(int i = 0; i < idx.rows; i++){
      p.x += pt.db(i,0); p.y += pt.db(i+idx.rows,0);
    }
    p.x /= idx.rows; p.y /= idx.rows;
  }else{p.x = vx/sum; p.y = vy/sum;}
  return p;
}
//==============================================================================
cv::Rect 
myAvatar::GetBoundingBox(cv::Mat &pt)
{
  assert(pt.cols == 1);
  assert(pt.rows > 0);
  int n = pt.rows/2;
  double xmin=pt.db(0,0),xmax=pt.db(0,0),ymin=pt.db(n,0),ymax=pt.db(n,0);
  for(int i = 1; i < n; i++){
    if(xmin > pt.db(i  ,0))xmin = pt.db(i  ,0);
    if(xmax < pt.db(i  ,0))xmax = pt.db(i  ,0);
    if(ymin > pt.db(i+n,0))ymin = pt.db(i+n,0);
    if(ymax < pt.db(i+n,0))ymax = pt.db(i+n,0);
  }
  cv::Rect r(xmin,ymin,xmax-xmin,ymax-ymin); return r;
}
//==============================================================================
void 
myAvatar::DrawPupil(double px,
		    double py,
		    double rad,
		    cv::Mat &pt,
		    vector<cv::Mat> &pupil,
		    vector<cv::Mat> &img,
		    cv::Scalar &b,
		    cv::Mat &idx,
		    cv::Mat &tri)
{
  for(int i = 0; i < idx.rows; i++){
    epts_.db(i         ,0) = pt.db(idx.it(i,0)          ,0); 
    epts_.db(i+idx.rows,0) = pt.db(idx.it(i,0)+pt.rows/2,0);
  }
  cv::Rect rect = this->GetBoundingBox(epts_); 
  int r = img[0].rows,c=img[0].cols;
  for(int y = rect.y; y < rect.y+rect.height; y++){
    for(int x = rect.x; x < rect.x+rect.width; x++){ 
      if(FACETRACKER::isWithinTri(x,y,tri,epts_)>=0){
	if(!((x < 0) || (x >= c) || (y < 0) || (y >= r))){
	  double v = sqrt((x-px)*(x-px)+(y-py)*(y-py));
	  if(v < rad){
	    double vx = (pupil[0].cols-1)/2 + (x-px)*((pupil[0].cols-1)/2)/rad;
	    double vy = (pupil[0].rows-1)/2 + (y-py)*((pupil[0].rows-1)/2)/rad;
	    for(int i = 0; i < 3; i++){
	      img[i].at<uchar>(y,x) = FACETRACKER::bilinInterp(pupil[i],vx,vy);
	    }
	  }else{for(int i = 0; i < 3; i++)img[i].at<uchar>(y,x) = b.val[i];}
	}
      }
    }
  }return;
}
//=============================================================================
cv::Point 
myAvatar::WarpBackLeftPupil(cv::Point &p,
			    cv::Mat &shape,
			    cv::Mat &ref)
{
  int n = _gpdm.nPoints();
  cv::Mat X(4,4,CV_64F),Y(4,1,CV_64F),Z(4,1,CV_64F);
  X.db(0,0)=shape.db(36,0);X.db(0,1)=shape.db(36+n,0); X.db(0,2)=1;X.db(0,3)=0;
  X.db(1,0)=shape.db(36+n,0);X.db(1,1)=-shape.db(36,0);X.db(1,2)=0;X.db(1,3)=1;
  X.db(2,0)=shape.db(39,0);X.db(2,1)=shape.db(39+n,0); X.db(2,2)=1;X.db(2,3)=0;
  X.db(3,0)=shape.db(39+n,0);X.db(3,1)=-shape.db(39,0);X.db(3,2)=0;X.db(3,3)=1;
  Y.db(0,0)=ref.db(36,0); Y.db(1,0)=ref.db(36+n,0);
  Y.db(2,0)=ref.db(39,0); Y.db(3,0)=ref.db(39+n,0);
  cv::solve(X,Y,Z,cv::DECOMP_SVD);
  double a = Z.db(0,0),b = Z.db(1,0),x = Z.db(2,0),y = Z.db(3,0);
  double vx = a*p.x + b*p.y + x,vy = a*p.y - b*p.x + y;
  cv::Point P(vx,vy); return P;
}
//=============================================================================
cv::Point 
myAvatar::WarpBackRightPupil(cv::Point &p,
			     cv::Mat &shape,
			     cv::Mat &ref)
{
  int n = _gpdm.nPoints();
  cv::Mat X(4,4,CV_64F),Y(4,1,CV_64F),Z(4,1,CV_64F);
  X.db(0,0)=shape.db(42,0);X.db(0,1)=shape.db(42+n,0); X.db(0,2)=1;X.db(0,3)=0;
  X.db(1,0)=shape.db(42+n,0);X.db(1,1)=-shape.db(42,0);X.db(1,2)=0;X.db(1,3)=1;
  X.db(2,0)=shape.db(45,0);X.db(2,1)=shape.db(45+n,0); X.db(2,2)=1;X.db(2,3)=0;
  X.db(3,0)=shape.db(45+n,0);X.db(3,1)=-shape.db(45,0);X.db(3,2)=0;X.db(3,3)=1;
  Y.db(0,0)=ref.db(42,0); Y.db(1,0)=ref.db(42+n,0);
  Y.db(2,0)=ref.db(45,0); Y.db(3,0)=ref.db(45+n,0);
  cv::solve(X,Y,Z,cv::DECOMP_SVD);
  double a = Z.db(0,0),b = Z.db(1,0),x = Z.db(2,0),y = Z.db(3,0);
  double vx = a*p.x + b*p.y + x,vy = a*p.y - b*p.x + y;
  cv::Point P(vx,vy); return P;
}
//=============================================================================
void 
myAvatar::WarpBackPupils(cv::Point &pl,
			 cv::Point &pr,
			 cv::Mat &shape,
			 cv::Mat &ref)
{
  pl = this->WarpBackLeftPupil(pl,shape,ref);
  pr = this->WarpBackRightPupil(pr,shape,ref); return;
}
//==============================================================================


//==============================================================================
void myAvatar::AddAvatar(cv::Mat &image, cv::Mat &points, cv::Mat &eyes)
{
  bool norm_mouth = true;

  //get 3D expressions
  vector<cv::Mat> X; int n = _gpdm.nPoints();  
  if(norm_mouth){
    //normalise mouth
    cv::Mat ss = points.clone();
    double y_top = 0; for (int i = 0; i < 3; i++) y_top += ss.db(60+i+n);
    double y_bot = 0; for (int i = 0; i < 3; i++) y_bot += ss.db(63+i+n);
    y_top /= 3.0; y_bot /= 3.0;
    double med = 0.5*(y_top+y_bot);
	  double d_top = med-y_top;
	  double d_bot = med-y_bot;
    for(int i = 0; i < 5; i++)ss.db(49+i+n) += d_top;
    for(int i = 0; i < 3; i++)ss.db(60+i+n) += d_top;
    for(int i = 0; i < 5; i++)ss.db(55+i+n) += d_bot;
    for(int i = 0; i < 3; i++)ss.db(63+i+n) += d_bot;
    X = _gen.Generate(ss);
  }else X = _gen.Generate(points);

  //***********************************
  /*
  for(int i = 0; i < (int)X.size(); i++){
    cv::Mat shape = X[i];
    for(int k = 0;;k++){
      int n = shape.rows/3;
      cv::Mat im = cv::Mat::zeros(300,300,CV_8U);
      cv::Mat R; FACETRACKER::Euler2Rot(R,0,k*3.14/200.0,0,true);
      for(int j = 0; j < n; j++){
	double x = R.db(0,0)*shape.db(j,0) + R.db(0,1)*shape.db(j+n,0) + 
	  R.db(0,2)*shape.db(j+n*2,0) + 150;
	double y = R.db(1,0)*shape.db(j,0) + R.db(1,1)*shape.db(j+n,0) + 
	  R.db(1,2)*shape.db(j+n*2,0) + 150;	  
	cv::circle(im,cv::Point(x,y),1,CV_RGB(255,255,255),2);
      }
      cvNamedWindow("test"); cv::imshow("test",im); 
      if(cvWaitKey(10) == 27)break;
    }
  }exit(0);
  */
  //************************************

  //***************************
  /*  
  for(int i = 0; i < (int)X.size(); i++){
    int n = X[i].rows/3; cv::Mat im = cv::Mat::zeros(300,300,CV_8UC3);
    X[i] *= 4;
    for(int j = 0; j < n; j++)
      cv::circle(im,cv::Point(X[i].db(j,0)+150,
			      X[i].db(j+n,0)+150),3,CV_RGB(255,0,0));
    cvNamedWindow("test"); cv::imshow("test",im); 
    if(cvWaitKey(0) == 27)break;
  }exit(0);
  */
  //***************************

  cv::Mat expr(2*n,X.size(),CV_64F);
  for(int j = 0; j < (int)X.size(); j++){
    cv::Mat x = expr.col(j),y = X[j](cv::Rect(0,0,1,2*n)); y.copyTo(x);
  } 
  //get shape and pose
  cv::Mat plocal(_gpdm.nModes(),1,CV_64F),pglobl(6,1,CV_64F);
  cv::Mat shape(3*_gpdm.nPoints(),1,CV_64F);
  if(norm_mouth){
    cv::Mat ss = expr.col(0).clone();
    _gpdm.CalcParams(ss,plocal,pglobl);
    _gpdm.CalcShape3D(shape,plocal);
    cv::Mat R(3,3,CV_64F),Ri(3,3,CV_64F);
    FACETRACKER::Euler2Rot(R,pglobl.db(1,0),pglobl.db(2,0),pglobl.db(3,0)); 
    R *= pglobl.db(0,0); cv::invert(R,Ri,cv::DECOMP_SVD);
    cv::Mat x(3,1,CV_64F),y(3,1,CV_64F);
    for(int j = 0; j < n; j++){
      x.db(0,0) = ss.db(j  ,0) - pglobl.db(4,0); 
      x.db(1,0) = ss.db(j+n,0) - pglobl.db(5,0);
      x.db(2,0) = R.db(2,0)*shape.db(j    ,0)+ 
	R.db(2,1)*shape.db(j+n  ,0)+ 
	R.db(2,2)*shape.db(j+n*2,0);
      y = Ri*x;
      shape.db(j    ,0) = y.db(0,0);
      shape.db(j+n  ,0) = y.db(1,0);
      shape.db(j+n*2,0) = y.db(2,0);
    }
  }else{
    _gpdm.CalcParams(points,plocal,pglobl);
    _gpdm.CalcShape3D(shape,plocal);
    cv::Mat R(3,3,CV_64F),Ri(3,3,CV_64F);
    FACETRACKER::Euler2Rot(R,pglobl.db(1,0),pglobl.db(2,0),pglobl.db(3,0)); 
    R *= pglobl.db(0,0); cv::invert(R,Ri,cv::DECOMP_SVD);
    cv::Mat x(3,1,CV_64F),y(3,1,CV_64F);
    for(int j = 0; j < n; j++){
      x.db(0,0) = points.db(j  ,0) - pglobl.db(4,0); 
      x.db(1,0) = points.db(j+n,0) - pglobl.db(5,0);
      x.db(2,0) = R.db(2,0)*shape.db(j    ,0)+ 
	R.db(2,1)*shape.db(j+n  ,0)+ 
	R.db(2,2)*shape.db(j+n*2,0);
      y = Ri*x;
      shape.db(j    ,0) = y.db(0,0);
      shape.db(j+n  ,0) = y.db(1,0);
      shape.db(j+n*2,0) = y.db(2,0);
    }
  }
  _shapes.push_back(shape);

  //crop appearance
  cv::Mat gray,grayTex,crop; cv::cvtColor(image,gray,CV_BGR2GRAY);
  _warp.Crop(gray,crop,points); _warp.Vectorize(crop,grayTex);
  _scale.push_back(cv::norm(grayTex));
  vector<cv::Mat> rgb(3); cv::split(image,rgb);
  vector<cv::Mat> crop_rgb(3);
  cv::Mat textr(3*_warp._nPix,1,CV_64F);
  for(int j = 0; j < 3; j++){
    cv::Mat t = textr(cv::Rect(0,j*_warp._nPix,1,_warp._nPix));
    _warp.Crop(rgb[j],crop_rgb[j],points); _warp.Vectorize(crop_rgb[j],t);
  }
  _textr.push_back(textr);


  //************************************
  /*
  cv::Mat imc; cv::merge(crop_rgb,imc); 
  cvNamedWindow("test"); cv::imshow("test",imc); 
  cvNamedWindow("test2"); cv::imshow("test2",image);  cvWaitKey(0); exit(0);
  */
  //************************************

  //crop template
  cv::Mat S = shape(cv::Rect(0,0,1,2*n)).clone();
  double a,b,tx,ty; FACETRACKER::CalcSimT(S,points,a,b,tx,ty);
  cv::Mat sim = (cv::Mat_<float>(2,3)<<a,-b,tx,b,a,ty);
  cv::Mat wimg_o = crop,sim_o = sim;
  for(int j = 0; j < 3; j++){
    cv::Mat im_o = rgb[j]; cv::warpAffine(im_o,wimg_o,sim_o,wimg_o.size());
    crop.copyTo(crop_rgb[j]);
  }
  cv::Mat img_rgb; cv::merge(crop_rgb,img_rgb); _images.push_back(img_rgb);
  
  //******************
  //cvNamedWindow("test"); cv::imshow("test",img_rgb); cvWaitKey(0); exit(0);
  //******************
  
  //compute expression parameters under generic PDM    
  vector<cv::Mat> p(expr.cols); shape.copyTo(_pdm._M);
  for(int j = 0; j < expr.cols; j++){
    cv::Mat x = expr.col(j); 
    _pdm.CalcParams(x,plocal_,pglobl_); p[j] = plocal_.clone();
  }
  cv::Mat Expr(_pdm.nModes(),p.size(),CV_64F);
  for(int j = 0; j < (int)p.size(); j++){cv::Mat x=Expr.col(j);p[j].copyTo(x);}
  _expr.push_back(Expr);

  //set pupils
  if(eyes.rows == 8){ //defined pupils
    cv::Mat lpupil,rpupil; cv::Scalar lscelera,rscelera; double lrad=0,rrad=0;
    _gpdm.CalcParams(points,plocal,pglobl);
    this->GetEyes(points,image,
		  eyes.db(0,0),eyes.db(4,0),
		  eyes.db(2,0),eyes.db(6,0),
		  eyes.db(1,0),eyes.db(5,0),
		  eyes.db(3,0),eyes.db(7,0),
		  lpupil,rpupil,lscelera,rscelera,lrad,rrad,100,101);
    lrad /= pglobl.db(0,0); rrad /= pglobl.db(0,0); double vv=0.5*(lrad+rrad);
    
    pupil llpupil,rrpupil;
    llpupil.rad = vv;                rrpupil.rad = vv;
    llpupil.idx = _base_lpupil.idx.clone();
    rrpupil.idx = _base_rpupil.idx.clone();
    llpupil.tri = _base_lpupil.tri.clone();
    rrpupil.tri = _base_rpupil.tri.clone();
    llpupil.scelera = lscelera;      rrpupil.scelera = rscelera;
    llpupil.image.resize(3); cv::split(lpupil,llpupil.image);
    rrpupil.image.resize(3); cv::split(rpupil,rrpupil.image);
    _lpupil.push_back(llpupil); _rpupil.push_back(rrpupil);

    //***********************************************************************
    //make spearate function!
    _idx = _lpupil.size()-1; cv::Mat ds = S.clone();
    cv::Rect rect = this->GetBoundingBox(ds); 
    for(int j = 0; j < n; j++){ds.db(j,0)-=rect.x-1; ds.db(j+n,0)-=rect.y-1;}
    shape = ds.clone(); vector<cv::Mat> rgb(3);
    for(int j = 0; j < 3; j++){
      rgb[j] = cv::Mat::zeros(rect.height+3,rect.width+3,CV_8U);
      cv::Mat t = textr(cv::Rect(0,j*_warp._nPix,1,_warp._nPix));
      cv::Mat crop; _warp.UnVectorize(t,crop); 
      _warp.Draw(crop,rgb[j],ds);
    }
    cv::Mat lpt; this->GetIdxPts(shape,_lpupil[_idx].idx,lpt,true);
    cv::Mat rpt; this->GetIdxPts(shape,_rpupil[_idx].idx,rpt,true);    
    cv::Rect lrect=this->GetBoundingBox(lpt),rrect=this->GetBoundingBox(rpt);
    _lpupil[_idx].px = lrect.x + lrect.width/2 ;
    _lpupil[_idx].py = lrect.y + lrect.height/2;
    _rpupil[_idx].px = rrect.x + rrect.width/2 ;
    _rpupil[_idx].py = rrect.y + rrect.height/2;
    vector<cv::Mat> rgb_clone(3);
    while(0){
      for(int j = 0; j < 3; j++)rgb_clone[j] = rgb[j].clone();
      this->DrawPupil(_lpupil[_idx].px,_lpupil[_idx].py,
		      _lpupil[_idx].rad,shape,_lpupil[_idx].image,
		      rgb_clone,_lpupil[_idx].scelera,_lpupil[_idx].idx,
		      _lpupil[_idx].tri);
      this->DrawPupil(_rpupil[_idx].px,_rpupil[_idx].py,
		      _rpupil[_idx].rad,shape,_rpupil[_idx].image,
		      rgb_clone,_rpupil[_idx].scelera,_rpupil[_idx].idx,
		      _rpupil[_idx].tri);
      cv::Mat im; cv::merge(rgb_clone,im);
      cvNamedWindow("Eyes"); cv::imshow("Eyes",im); 
      int c = cvWaitKey(0);
      if(c == 27)break;
      else if(c == int('w'))_lpupil[_idx].py -= 1; //left up
      else if(c == int('d'))_lpupil[_idx].px += 1; //left right
      else if(c == int('x'))_lpupil[_idx].py += 1; //left down
      else if(c == int('a'))_lpupil[_idx].px -= 1; //left left
      else if(c == int('i'))_rpupil[_idx].py -= 1; //right up
      else if(c == int('l'))_rpupil[_idx].px += 1; //right right
      else if(c == int('m'))_rpupil[_idx].py += 1; //right down
      else if(c == int('j'))_rpupil[_idx].px -= 1; //right left
      else if(c == int('b')){
	_lpupil[_idx].scelera *= 1.1;
	_rpupil[_idx].scelera *= 1.1;
      }
      else if(c == int('v')){
	_lpupil[_idx].scelera *= 0.9;
	_rpupil[_idx].scelera *= 0.9;
      }
      else if(c == int('h')){
	_lpupil[_idx].rad *= 1.1;
	_rpupil[_idx].rad *= 1.1;
      }
      else if(c == int('g')){
	_lpupil[_idx].rad *= 0.9;
	_rpupil[_idx].rad *= 0.9;
      }
      else if(c == int('y')){        //change pupil appearance to standard
	cv::Mat limg;cv::merge(_lpupil[_idx].image,limg);limg=cv::Scalar(0,0,0);
	cv::Mat rimg;cv::merge(_rpupil[_idx].image,rimg);rimg=cv::Scalar(0,0,0);
	cv::split(limg,_lpupil[_idx].image);cv::split(rimg,_rpupil[_idx].image);
      }
    }
    _lpupil[_idx].px += rect.x-1; _lpupil[_idx].py += rect.y-1;
    _rpupil[_idx].px += rect.x-1; _rpupil[_idx].py += rect.y-1;
    //make spearate function!
    //***********************************************************************
  }else{ //undefined pupils
    pupil llpupil,rrpupil;
    llpupil.rad = 1;                      rrpupil.rad = 1;
    llpupil.idx = _base_lpupil.idx.clone();
    rrpupil.idx = _base_rpupil.idx.clone();
    llpupil.tri = _base_lpupil.tri.clone();
    rrpupil.tri = _base_rpupil.tri.clone();
    llpupil.scelera = cv::Scalar(150,150,150);     
    rrpupil.scelera = cv::Scalar(150,150,150);
    cv::Mat limg = cv::Mat::zeros(101,101,CV_8UC3);
    cv::Mat rimg = cv::Mat::zeros(101,101,CV_8UC3);
    cv::circle(limg,cv::Point((limg.rows-1)/2,(limg.rows-1)/2),
	       (limg.rows-1)*0.3,CV_RGB(50,50,50),(limg.rows-1)/4);
    cv::circle(rimg,cv::Point((rimg.rows-1)/2,(rimg.rows-1)/2),
	       (rimg.rows-1)/4,CV_RGB(50,50,50),(limg.rows-1)/4);
    llpupil.image.resize(3); cv::split(limg,llpupil.image);
    rrpupil.image.resize(3); cv::split(rimg,rrpupil.image);
    _lpupil.push_back(llpupil); _rpupil.push_back(rrpupil);

    //*************************************************************************
    //make separate function
    _idx = _lpupil.size()-1; shape = S; int n = _pdm.nPoints();
    cv::Rect rect = this->GetBoundingBox(shape); shape = points.clone();
    for(int j = 0; j < n; j++){
      shape.db(j,0) -= rect.x-1; shape.db(j+n,0) -= rect.y-1;
    }
    cv::Mat lpt; this->GetIdxPts(shape,_lpupil[_idx].idx,lpt,true);
    cv::Mat rpt; this->GetIdxPts(shape,_rpupil[_idx].idx,rpt,true);
    cv::Rect lrect = this->GetBoundingBox(lpt);
    cv::Rect rrect = this->GetBoundingBox(rpt);
    _lpupil[_idx].px = lrect.x + lrect.width/2 ;
    _lpupil[_idx].py = lrect.y + lrect.height/2;
    _rpupil[_idx].px = rrect.x + rrect.width/2 ;
    _rpupil[_idx].py = rrect.y + rrect.height/2;
    //*************************************************************************
  }
  _idx = _shapes.size()-1; return;
}
//=============================================================================
void myAvatar::GetEyes(cv::Mat &pt,cv::Mat &im,
		       double cxl,double cyl,double exl,double eyl,
		       double cxr,double cyr,double exr,double eyr,	     
		       cv::Mat &leye,cv::Mat &reye,
		       cv::Scalar &lbck,cv::Scalar &rbck,
		       double &lrad,double &rrad,
		       const int d,const int size)
{
  cv::Mat lpt(12,1,CV_64F),rpt(12,1,CV_64F);
  for(int i = 0; i < 6; i++){
    lpt.db(i,0) = pt.db(36+i,0); lpt.db(i+6,0) = pt.db(36+i+66,0);
    rpt.db(i,0) = pt.db(42+i,0); rpt.db(i+6,0) = pt.db(42+i+66,0);
  }
  cv::Mat tri = (cv::Mat_<int>(4,3) << 0,1,5,1,2,5,2,4,5,2,3,4);  
  std::vector<cv::Mat> rgb(3); cv::split(im,rgb);
  lrad = sqrt((exl-cxl)*(exl-cxl) + (eyl-cyl)*(eyl-cyl));
  rrad = sqrt((exr-cxr)*(exr-cxr) + (eyr-cyr)*(eyr-cyr));
  
  //find pupil appearance
  cv::Mat Tl = cv::Mat::zeros(3*d,1,CV_64F);
  cv::Mat Tr = cv::Mat::zeros(3*d,1,CV_64F);
  cv::Mat Sl = cv::Mat::zeros(3*d,1,CV_64F);
  cv::Mat Sr = cv::Mat::zeros(3*d,1,CV_64F);
  for(double theta = 0; theta < 360; theta += 1){
    double gxl = std::cos(3.14159265*theta/180)*lrad/(d-1);
    double gyl = std::sin(3.14159265*theta/180)*lrad/(d-1);
    double gxr = std::cos(3.14159265*theta/180)*rrad/(d-1);
    double gyr = std::sin(3.14159265*theta/180)*rrad/(d-1);
    for(int i = 0; i < d; i++){
      double xl = cxl + gxl*i,yl = cyl + gyl*i;
      double xr = cxr + gxr*i,yr = cyr + gyr*i;      
      if(FACETRACKER::isWithinTri(xl,yl,tri,lpt)>=0){	
	Sl.db(i,0) += 1; Sl.db(i+d,0) += 1; Sl.db(i+d*2,0) += 1;
	Tl.db(i    ,0) += FACETRACKER::bilinInterp(rgb[0],xl,yl);
	Tl.db(i+d  ,0) += FACETRACKER::bilinInterp(rgb[1],xl,yl);
	Tl.db(i+d*2,0) += FACETRACKER::bilinInterp(rgb[2],xl,yl);
      }
      if(FACETRACKER::isWithinTri(xr,yr,tri,rpt)>=0){	
	Sr.db(i,0) += 1; Sr.db(i+d,0) += 1; Sr.db(i+d*2,0) += 1;
	Tr.db(i    ,0) += FACETRACKER::bilinInterp(rgb[0],xr,yr);
	Tr.db(i+d  ,0) += FACETRACKER::bilinInterp(rgb[1],xr,yr);
	Tr.db(i+d*2,0) += FACETRACKER::bilinInterp(rgb[2],xr,yr);
      }
    }
  }
  for(int i = 0; i < 3*d; i++){
    Tl.db(i,0) /= Sl.db(i,0); Tr.db(i,0) /= Sr.db(i,0);
  }
  //get background color
  cv::Rect lrect = this->GetBoundingBox(lpt),rrect = this->GetBoundingBox(rpt);
  double r = 0,g = 0,b = 0, sum = 0;
  for(int y = lrect.y; y < lrect.y+lrect.height; y++){
    for(int x = lrect.x; x < lrect.x+lrect.width; x++){
      double v = sqrt((x-cxl)*(x-cxl) + (y-cyl)*(y-cyl));
      if(v < lrad)continue;
      if(FACETRACKER::isWithinTri(x,y,tri,lpt)>=0){
	r += rgb[0].at<uchar>(y,x);
	g += rgb[1].at<uchar>(y,x);
	b += rgb[2].at<uchar>(y,x);
	sum += 1;
      }
    }
  }
  lbck = cv::Scalar(150,150,150);//cv::Scalar(r/sum,g/sum,b/sum);
  //lbck = cv::Scalar(255,255,255);
  r = g = b = sum = 0;
  for(int y = rrect.y; y < rrect.y+rrect.height; y++){
    for(int x = rrect.x; x < rrect.x+rrect.width; x++){
      double v = sqrt((x-cxr)*(x-cxr) + (y-cyr)*(y-cyr));
      if(v < rrad)continue;
      if(FACETRACKER::isWithinTri(x,y,tri,rpt)>=0){
	r += rgb[0].at<uchar>(y,x);
	g += rgb[1].at<uchar>(y,x);
	b += rgb[2].at<uchar>(y,x);
	sum += 1;
      }
    }
  }
  rbck = cv::Scalar(150,150,150);//cv::Scalar(r/sum,g/sum,b/sum);
  //rbck = cv::Scalar(255,255,255);

  //draw pupils
  leye = cv::Mat::zeros(size,size,CV_8UC3); leye = lbck;
  reye = cv::Mat::zeros(size,size,CV_8UC3); reye = rbck;
  std::vector<cv::Mat> lrgb(3),rrgb(3);
  cv::split(leye,lrgb);cv::split(reye,rrgb);
  for(int y = 0; y < size; y++){
    for(int x = 0; x < size; x++){
      double v = sqrt((y-(size-1)/2)*(y-(size-1)/2) + 
		      (x-(size-1)/2)*(x-(size-1)/2));
      int vi = floor((double(d)-1.0)*v/((size-1)/2));
      if(v < (size-1)/2){
	double xl = cxl + (x - (size-1)/2)*lrad/((size-1)/2);
	double yl = cyl + (y - (size-1)/2)*lrad/((size-1)/2);
	double xr = cxr + (x - (size-1)/2)*rrad/((size-1)/2);
	double yr = cyr + (y - (size-1)/2)*rrad/((size-1)/2);
	if(FACETRACKER::isWithinTri(xl,yl,tri,lpt)>=0){
	  for(int i = 0; i < 3; i++)
	    lrgb[i].at<uchar>(y,x) = 
	      (uchar)FACETRACKER::bilinInterp(rgb[i],xl,yl);
	}else{	 
	  for(int i = 0; i < 3; i++)lrgb[i].at<uchar>(y,x) = Tl.db(vi+d*i,0);
	}
	if(FACETRACKER::isWithinTri(xr,yr,tri,rpt)>=0){
	  for(int i = 0; i < 3; i++)
	    rrgb[i].at<uchar>(y,x) = 
	      (uchar)FACETRACKER::bilinInterp(rgb[i],xr,yr);
	}else{	 
	  for(int i = 0; i < 3; i++)rrgb[i].at<uchar>(y,x) = Tr.db(vi+d*i,0);
	}
      }
    }
  }
  cv::merge(lrgb,leye); cv::merge(rrgb,reye); return;
}
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//==============================================================================
//=============================================================================
KSmooth& KSmooth::operator=(KSmooth const&rhs)
{
  _sigma = rhs._sigma; _X.resize(rhs._X.size()); _Y.resize(rhs._Y.size());
  for(unsigned i = 0; i < _X.size(); i++){
    _X[i] = rhs._X[i].clone(); _Y[i] = rhs._Y[i].clone();
  }return *this;
}
//=============================================================================
void KSmooth::Read(ifstream &s)
{
  int N; s >> N >> _sigma; _X.resize(N); _Y.resize(N);
  for(int i = 0; i < N; i++){
    FACETRACKER::IO::ReadMat(s,_X[i]);
    FACETRACKER::IO::ReadMat(s,_Y[i]);
  }
}
//=============================================================================
void KSmooth::ReadBinary(ifstream &s, bool readType)
{
  if(readType){int type;
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == FACETRACKER::IOBinary::KSMOOTH);
  }

  int N; 
  s.read(reinterpret_cast<char*>(&N), sizeof(N));
  s.read(reinterpret_cast<char*>(&_sigma), sizeof(_sigma));

  _X.resize(N); _Y.resize(N);
  for(int i = 0; i < N; i++){
    FACETRACKER::IOBinary::ReadMat(s,_X[i]);
    FACETRACKER::IOBinary::ReadMat(s,_Y[i]);
  }
}
//=============================================================================
void KSmooth::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << (int)_X.size() << " " << _sigma << " "; 
    for(unsigned i = 0; i < _X.size(); i++){
      FACETRACKER::IO::WriteMat(s,_X[i]);
      FACETRACKER::IO::WriteMat(s,_Y[i]);
    }
  }
  else{
    int t = FACETRACKER::IOBinary::KSMOOTH; 
    s.write(reinterpret_cast<char*>(&t), sizeof(t));

    t = _X.size();
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    s.write(reinterpret_cast<char*>(&_sigma), sizeof(_sigma));
    for(unsigned i = 0; i < _X.size(); i++){
      FACETRACKER::IOBinary::WriteMat(s,_X[i]);
      FACETRACKER::IOBinary::WriteMat(s,_Y[i]);
    }
  }
}
//=============================================================================
void KSmooth::Train(vector<cv::Mat> &Y,vector<cv::Mat> &X,
		    double sigma)
{
  _sigma = sigma;
  _X.resize(X.size()); _Y.resize(Y.size());
  for(unsigned i = 0; i < _X.size(); i++){
    _X[i] = X[i].clone(); _Y[i] = Y[i].clone();
  }return;
}
//=============================================================================
cv::Mat KSmooth::Predict(cv::Mat &x)
{
  vector<double> a(_X.size()); double sum = 0.0;
  for(unsigned i = 0; i < _X.size(); i++){
    sum += (a[i] = this->Kernel(x,_X[i],_sigma));
  }
  cv::Mat y = cv::Mat::zeros(_Y[0].rows,_Y[0].cols,CV_64F);  
  if(sum == 0){
    sum = 1.0/_Y.size();for(unsigned i = 0; i < _Y.size(); i++)y += sum*_Y[i];
  }else{for(unsigned i = 0; i < _Y.size(); i++)y += (a[i]/sum)*_Y[i];}
  return y;
}
//=============================================================================
double KSmooth::Kernel(cv::Mat &x1,cv::Mat &x2,double sigma)
{
  double v = cv::norm(x1,x2); return exp(-(v*v)/sigma);
}
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
ShapeExpMap& ShapeExpMap::operator=(ShapeExpMap const&rhs)
{
  _pdm = rhs._pdm; _reg = rhs._reg;
  plocal_.create(_pdm.nModes(),1,CV_64F); pglobl_.create(6,1,CV_64F);
  shape_.create(3*_pdm.nPoints(),1,CV_64F);
  R_.create(3,3,CV_64F); Ri_.create(3,3,CV_64F); return *this;
}
//=============================================================================
void ShapeExpMap::Read(ifstream &s)
{
  int N; s >> N; _reg.resize(N); _pdm.Read(s); 
  for(unsigned i = 0; i < _reg.size(); i++)_reg[i].Read(s);
  plocal_.create(_pdm.nModes(),1,CV_64F); pglobl_.create(6,1,CV_64F);
  shape_.create(3*_pdm.nPoints(),1,CV_64F);
  R_.create(3,3,CV_64F); Ri_.create(3,3,CV_64F); 
}
//=============================================================================
void ShapeExpMap::ReadBinary(ifstream &s, bool readType)
{
  if(readType){int type;
    s.read(reinterpret_cast<char*>(&type), sizeof(type));
    assert(type == FACETRACKER::IOBinary::SHAPEEXPMAP);
  }
  
  int N; s.read(reinterpret_cast<char*>(&N), sizeof(N));
  _reg.resize(N); _pdm.ReadBinary(s); 
  for(unsigned i = 0; i < _reg.size(); i++)_reg[i].ReadBinary(s);
  
  plocal_.create(_pdm.nModes(),1,CV_64F); pglobl_.create(6,1,CV_64F);
  shape_.create(3*_pdm.nPoints(),1,CV_64F);
  R_.create(3,3,CV_64F); Ri_.create(3,3,CV_64F); 
}
//=============================================================================
void ShapeExpMap::Write(ofstream &s, bool binary)
{
  if(!binary){
    s << (int)_reg.size() << " ";
    _pdm.Write(s); for(unsigned i = 0; i < _reg.size(); i++)_reg[i].Write(s);
  }
  else{
    int t = FACETRACKER::IOBinary::SHAPEEXPMAP;
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    t = _reg.size();
    s.write(reinterpret_cast<char*>(&t), sizeof(t));
    _pdm.Write(s,binary);
    for(unsigned i = 0; i < _reg.size(); i++)_reg[i].Write(s, binary);
  }
}
//=============================================================================
void ShapeExpMap::Train(FACETRACKER::PDM3D &pdm,
			vector<vector<cv::Mat> > express,
			vector<vector<cv::Mat> > neutral,
			double sigma)
{
  assert(express.size() == neutral.size());
  _pdm = pdm; _reg.resize(express.size());
  plocal_.create(_pdm.nModes(),1,CV_64F); pglobl_.create(6,1,CV_64F);
  shape_.create(3*_pdm.nPoints(),1,CV_64F);
  R_.create(3,3,CV_64F); Ri_.create(3,3,CV_64F);
  for(unsigned i = 0; i < _reg.size(); i++){
    assert(express[i].size() == neutral[i].size());
    int N = neutral[i].size();
    vector<cv::Mat> X(N),Y(N);
    for(int j = 0; j < N; j++){
      shape_ = this->CalcNormed3D(neutral[i][j]);
      X[j] = shape_.clone();
      shape_ = this->CalcNormed3D(express[i][j]);
      Y[j] = shape_ - X[j];
    }
    _reg[i].Train(Y,X,sigma);
  }return;
}
//=============================================================================
vector<cv::Mat> ShapeExpMap::Generate(cv::Mat &s2D)
{
  shape_ = this->CalcNormed3D(s2D);
  vector<cv::Mat> s3D(_reg.size()+1); s3D[0] = shape_.clone();
  for(unsigned i = 0; i < _reg.size(); i++)
    s3D[i+1] = shape_+_reg[i].Predict(shape_);
  return s3D;
}
//=============================================================================
cv::Mat ShapeExpMap::CalcNormed3D(cv::Mat &s2D)
{
  _pdm.CalcParams(s2D,plocal_,pglobl_); _pdm.CalcShape3D(shape_,plocal_);
  FACETRACKER::Euler2Rot(R_,pglobl_.db(1,0),pglobl_.db(2,0),pglobl_.db(3,0)); 
  R_ *= pglobl_.db(0,0); cv::invert(R_,Ri_,cv::DECOMP_SVD);
  cv::Mat x(3,1,CV_64F),y(3,1,CV_64F); int n = _pdm.nPoints();
  for(int j = 0; j < n; j++){
    x.db(0,0) = s2D.db(j  ,0) - pglobl_.db(4,0); 
    x.db(1,0) = s2D.db(j+n,0) - pglobl_.db(5,0);
    x.db(2,0) = R_.db(2,0)*shape_.db(j    ,0)+ 
      R_.db(2,1)*shape_.db(j+n  ,0)+ 
      R_.db(2,2)*shape_.db(j+n*2,0);
    y = Ri_*x;
    shape_.db(j    ,0) = y.db(0,0);
    shape_.db(j+n  ,0) = y.db(1,0);
    shape_.db(j+n*2,0) = y.db(2,0);
  }return shape_;
}
//==============================================================================
