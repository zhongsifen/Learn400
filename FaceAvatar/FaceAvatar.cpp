//
//  FaceAvatar.cpp
//  FaceX
//
//  Created by SIFEN ZHONG on 12/8/15.
//
//

#include "_FaceAvatar.hpp"

#include <iostream>
#include <fstream>
#define it at<int>
#define db at<double>

FaceAvatar::FaceAvatar()
{
	this->status = 0;
}

FaceAvatar::FaceAvatar(bool binary)
{
	this->status = 0;

	if (binary) {
		bool ret = this->load_binary(RES_FACEX + "CI2CV.avatar.binary");		if (!ret) return;
	}
	else {
		bool ret = this->load(RES_FACEX + "kabuki.avatar");		if (!ret) return;
	}
	
	this->status = 1;
}

FaceAvatar::FaceAvatar(std::string filename)
{
	this->status = 0;
	
	bool ret = this->load(filename);		if (!ret) return;
	
	this->status = 1;
}

bool
FaceAvatar::get_index(int& index)
{
	this->index = this->_idx;
	index = this->index;

	return true;
}

bool
FaceAvatar::set_index(int& index)
{
	this->index = index;
	this->setAvatar(index);
	
	return true;
}

bool
FaceAvatar::get_a(cv::Mat& a)
{
	a = this->_images[index];		if (!a.data)	return false;
	
	return true;
}

bool
FaceAvatar::load(std::ifstream& s)
{
	int i=0;
	int type; s >> type; if (type != IO::MYAVATAR) return false;
	int size; s >> size;
	_images.resize(size);
	_shapes.resize(size);
	_textr.resize(size);
	_scale.resize(size);
	_lpupil.resize(size);
	_rpupil.resize(size);
	{
		std::vector<cv::Mat> rgb(3);
		for(int j = 0; j < 3; j++) {
			cv::Mat img; FACETRACKER::IO::ReadMat(s,img);
			img.convertTo(rgb[j],CV_8U);
		}
		_images[i].create(rgb[0].rows,rgb[0].cols,CV_8UC3);
		cv::merge(rgb,_images[i]);
		FACETRACKER::IO::ReadMat(s,_shapes[i]);
		FACETRACKER::IO::ReadMat(s,_textr[i]);
		s >> _scale[i];
		s >> _lpupil[i].rad  >> _rpupil[i].rad
		>> _lpupil[i].px   >> _rpupil[i].px
		>> _lpupil[i].py   >> _rpupil[i].py
		>> _lpupil[i].scelera.val[0] >> _rpupil[i].scelera.val[0]
		>> _lpupil[i].scelera.val[1] >> _rpupil[i].scelera.val[1]
		>> _lpupil[i].scelera.val[2] >> _rpupil[i].scelera.val[2];
		FACETRACKER::IO::ReadMat(s,_lpupil[i].idx);
		FACETRACKER::IO::ReadMat(s,_rpupil[i].idx);
		FACETRACKER::IO::ReadMat(s,_lpupil[i].tri);
		FACETRACKER::IO::ReadMat(s,_rpupil[i].tri);
		_lpupil[i].image.resize(3); _rpupil[i].image.resize(3);
		for(int j = 0; j < 3; j++){
			cv::Mat limg; FACETRACKER::IO::ReadMat(s,limg);
			cv::Mat rimg; FACETRACKER::IO::ReadMat(s,rimg);
			limg.convertTo(_lpupil[i].image[j],CV_8U);
			rimg.convertTo(_rpupil[i].image[j],CV_8U);
		}
		FACETRACKER::IO::ReadMat(s,_ocav_idx);
		FACETRACKER::IO::ReadMat(s,_ocav_tri);
	}
	FACETRACKER::IO::ReadMat(s,_basis);
	_warp.Read(s); _pdm.Read(s); _gpdm.Read(s); _gen.Read(s);  _expr.resize(size);
	for(int i = 0; i < size; i++) FACETRACKER::IO::ReadMat(s, _expr[i]);
	_user.create(3*_pdm.nPoints(),1,CV_64F);
	plocal_.create(_pdm.nModes(),1,CV_64F);
	pglobl_.create(6,1,CV_64F);
	gplocal_.create(_gpdm.nModes(),1,CV_64F);
	gpglobl_.create(6,1,CV_64F);
	_shape.create(2*_pdm.nPoints(),1,CV_64F);
	textr_.create(3*_warp._nPix,1,CV_64F);
	gray_.create(_warp._nPix,1,CV_64F);
	img_.create(_warp.Height(),_warp.Width(),CV_8U);
	if(_lpupil[0].idx.rows > _rpupil[0].idx.rows)
		epts_.create(2*_lpupil[0].idx.rows,1,CV_64F);
	else epts_.create(2*_rpupil[0].idx.rows,1,CV_64F);
	_idx = 0;
	
	_base_lpupil = _lpupil[0];
	_base_rpupil = _rpupil[0];
	
	return true;
}

bool
FaceAvatar::load_binary(std::string filename)
{
	std::ifstream s(filename, std::ios::binary);		if (!s.is_open()) return false;
	bool ret = this->load_binary(s);		if (!ret) return false;
	
	return true;
}

bool
FaceAvatar::load_binary(std::ifstream& s)
{
	int type;
	s.read((char*)(&type), sizeof(type));		if (type != IOBinary::MYAVATAR) return false;
	
	int k;
	s.read((char*)(&k), sizeof(k));
	
	_images.resize(k); _shapes.resize(k); _textr.resize(k); _scale.resize(k);
	_lpupil.resize(k); _rpupil.resize(k);
	for(unsigned i = 0; i < _images.size(); i++){
		std::vector<cv::Mat> rgb(3);
		for(int j = 0; j < 3; j++){
			cv::Mat img; FACETRACKER::IOBinary::ReadMat(s,img);
			img.convertTo(rgb[j],CV_8U);
		}
		_images[i].create(rgb[0].rows,rgb[0].cols,CV_8UC3);
		cv::merge(rgb,_images[i]);
		FACETRACKER::IOBinary::ReadMat(s,_shapes[i]);
		FACETRACKER::IOBinary::ReadMat(s,_textr[i]);
		
		s.read(reinterpret_cast<char*>(&_scale[i]), sizeof(_scale[i]));
		s.read(reinterpret_cast<char*>(&_lpupil[i].rad), sizeof(_lpupil[i].rad));
		s.read(reinterpret_cast<char*>(&_rpupil[i].rad), sizeof(_rpupil[i].rad));
		s.read(reinterpret_cast<char*>(&_lpupil[i].px), sizeof(_lpupil[i].px));
		s.read(reinterpret_cast<char*>(&_rpupil[i].px), sizeof(_rpupil[i].px));
		s.read(reinterpret_cast<char*>(&_lpupil[i].py), sizeof(_lpupil[i].py));
		s.read(reinterpret_cast<char*>(&_rpupil[i].py), sizeof(_rpupil[i].py));
		s.read(reinterpret_cast<char*>(&_lpupil[i].scelera), sizeof(_lpupil[i].scelera));
		s.read(reinterpret_cast<char*>(&_rpupil[i].scelera), sizeof(_rpupil[i].scelera));
		
		
		FACETRACKER::IOBinary::ReadMat(s,_lpupil[i].idx);
		FACETRACKER::IOBinary::ReadMat(s,_rpupil[i].idx);
		FACETRACKER::IOBinary::ReadMat(s,_lpupil[i].tri);
		FACETRACKER::IOBinary::ReadMat(s,_rpupil[i].tri);
		_lpupil[i].image.resize(3); _rpupil[i].image.resize(3);
		for(int j = 0; j < 3; j++){
			cv::Mat limg; FACETRACKER::IOBinary::ReadMat(s,limg);
			cv::Mat rimg; FACETRACKER::IOBinary::ReadMat(s,rimg);
			limg.convertTo(_lpupil[i].image[j],CV_8U);
			rimg.convertTo(_rpupil[i].image[j],CV_8U);
		}
		// cv::namedWindow("testpupil");
		// cv::imshow("testpupil", _lpupil[i].image[0]);
		// cv::waitKey(0);
		
		FACETRACKER::IOBinary::ReadMat(s,_ocav_idx);
		FACETRACKER::IOBinary::ReadMat(s,_ocav_tri);
	}
	FACETRACKER::IOBinary::ReadMat(s,_basis);
	_warp.ReadBinary(s); _pdm.ReadBinary(s); _gpdm.ReadBinary(s); _gen.ReadBinary(s);  _expr.resize(k);
	for(int i = 0; i < k; i++)
		FACETRACKER::IOBinary::ReadMat(s,_expr[i]);
	
	_user.create(3*_pdm.nPoints(),1,CV_64F);
	plocal_.create(_pdm.nModes(),1,CV_64F);
	pglobl_.create(6,1,CV_64F);
	gplocal_.create(_gpdm.nModes(),1,CV_64F);
	gpglobl_.create(6,1,CV_64F);
	_shape.create(2*_pdm.nPoints(),1,CV_64F);
	textr_.create(3*_warp._nPix,1,CV_64F);
	gray_.create(_warp._nPix,1,CV_64F);
	img_.create(_warp.Height(),_warp.Width(),CV_8U);
	if(_lpupil[0].idx.rows > _rpupil[0].idx.rows)
		epts_.create(2*_lpupil[0].idx.rows,1,CV_64F);
	else epts_.create(2*_rpupil[0].idx.rows,1,CV_64F);
	_idx = 0;
	
	_base_lpupil = _lpupil[0];
	_base_rpupil = _rpupil[0];
	
	return true;
}

bool
FaceAvatar::save(std::ofstream& s)
{
	int i=0;
	std::vector<cv::Mat> rgb(3); cv::split(_images[i],rgb);
	{
		for(int j = 0; j < 3; j++) {
			cv::Mat img; rgb[j].convertTo(img,CV_32S);
			FaceX::write_mat(s, img);
		}
		FaceX::write_mat(s,_shapes[i]);
		FaceX::write_mat(s,_textr[i]);
		s << _scale[i] << " "
		<< _lpupil[i].rad  << " "
		<< _rpupil[i].rad  << " "
		<< _lpupil[i].px   << " "
		<< _rpupil[i].px   << " "
		<< _lpupil[i].py   << " "
		<< _rpupil[i].py   << " "
		<< _lpupil[i].scelera.val[0] << " "
		<< _rpupil[i].scelera.val[0] << " "
		<< _lpupil[i].scelera.val[1] << " "
		<< _rpupil[i].scelera.val[1] << " "
		<< _lpupil[i].scelera.val[2] << " "
		<< _rpupil[i].scelera.val[2] << " ";
		FaceX::write_mat(s,_lpupil[i].idx);
		FaceX::write_mat(s,_rpupil[i].idx);
		FaceX::write_mat(s,_lpupil[i].tri);
		FaceX::write_mat(s,_rpupil[i].tri);
		for(int j = 0; j < 3; j++) {
			cv::Mat limg; _lpupil[i].image[j].convertTo(limg,CV_32S);
			cv::Mat rimg; _rpupil[i].image[j].convertTo(rimg,CV_32S);
			FaceX::write_mat(s,limg);
			FaceX::write_mat(s,rimg);
		}
		FaceX::write_mat(s,_ocav_idx);
		FaceX::write_mat(s,_ocav_tri);
	}
	FaceX::write_mat(s,_basis);
	_warp.Write(s); _pdm.Write(s); _gpdm.Write(s); _gen.Write(s);
	FaceX::write_mat(s,_expr[i]);
	
	return true;
}

bool
FaceAvatar::load(std::string filename)
{
	std::ifstream s(filename);		if (!s.is_open()) return false;
	
	int type; s >> type; if (type != IO::MYAVATAR) return false;
	int size; s >> size;
	_images.resize(size);
	_shapes.resize(size);
	_textr.resize(size);
	_scale.resize(size);
	_lpupil.resize(size);
	_rpupil.resize(size);
	for(unsigned i = 0; i < _images.size(); i++) {
		std::vector<cv::Mat> rgb(3);
		for(int j = 0; j < 3; j++) {
			cv::Mat img; FACETRACKER::IO::ReadMat(s,img);
			img.convertTo(rgb[j],CV_8U);
		}
		_images[i].create(rgb[0].rows,rgb[0].cols,CV_8UC3);
		cv::merge(rgb,_images[i]);
		FACETRACKER::IO::ReadMat(s,_shapes[i]);
		FACETRACKER::IO::ReadMat(s,_textr[i]);
		s >> _scale[i];
		s >> _lpupil[i].rad  >> _rpupil[i].rad
		>> _lpupil[i].px   >> _rpupil[i].px
		>> _lpupil[i].py   >> _rpupil[i].py
		>> _lpupil[i].scelera.val[0] >> _rpupil[i].scelera.val[0]
		>> _lpupil[i].scelera.val[1] >> _rpupil[i].scelera.val[1]
		>> _lpupil[i].scelera.val[2] >> _rpupil[i].scelera.val[2];
		FACETRACKER::IO::ReadMat(s,_lpupil[i].idx);
		FACETRACKER::IO::ReadMat(s,_rpupil[i].idx);
		FACETRACKER::IO::ReadMat(s,_lpupil[i].tri);
		FACETRACKER::IO::ReadMat(s,_rpupil[i].tri);
		_lpupil[i].image.resize(3); _rpupil[i].image.resize(3);
		for(int j = 0; j < 3; j++){
			cv::Mat limg; FACETRACKER::IO::ReadMat(s,limg);
			cv::Mat rimg; FACETRACKER::IO::ReadMat(s,rimg);
			limg.convertTo(_lpupil[i].image[j],CV_8U);
			rimg.convertTo(_rpupil[i].image[j],CV_8U);
		}
		FACETRACKER::IO::ReadMat(s,_ocav_idx);
		FACETRACKER::IO::ReadMat(s,_ocav_tri);
	}
	FACETRACKER::IO::ReadMat(s,_basis);
	_warp.Read(s); _pdm.Read(s); _gpdm.Read(s); _gen.Read(s);  _expr.resize(size);
	for(int i = 0; i < size; i++) FACETRACKER::IO::ReadMat(s, _expr[i]);
	_user.create(3*_pdm.nPoints(),1,CV_64F);
	plocal_.create(_pdm.nModes(),1,CV_64F);
	pglobl_.create(6,1,CV_64F);
	gplocal_.create(_gpdm.nModes(),1,CV_64F);
	gpglobl_.create(6,1,CV_64F);
	_shape.create(2*_pdm.nPoints(),1,CV_64F);
	textr_.create(3*_warp._nPix,1,CV_64F);
	gray_.create(_warp._nPix,1,CV_64F);
	img_.create(_warp.Height(),_warp.Width(),CV_8U);
	if(_lpupil[0].idx.rows > _rpupil[0].idx.rows)
		epts_.create(2*_lpupil[0].idx.rows,1,CV_64F);
	else epts_.create(2*_rpupil[0].idx.rows,1,CV_64F);
	_idx = 0;
	
	_base_lpupil = _lpupil[0];
	_base_rpupil = _rpupil[0];
	
	if (index >= this->numberOfAvatars() || index < 0) {
		index = this->numberOfAvatars() - 1;
	}
	
	return true;
}

bool
FaceAvatar::save(std::string filename)
{
	std::ofstream s(filename);		if (!s.is_open()) return false;
	
	s << IO::MYAVATAR << " ";
	s << _images.size() << " ";
	for(unsigned i = 0; i < _images.size(); i++){
		std::vector<cv::Mat> rgb(3); cv::split(_images[i],rgb);
		for(int j = 0; j < 3; j++){
			cv::Mat img; rgb[j].convertTo(img,CV_32S);
			FACETRACKER::IO::WriteMat(s,img);
		}
		FACETRACKER::IO::WriteMat(s,_shapes[i]);
		FACETRACKER::IO::WriteMat(s,_textr[i]);
		s << _scale[i] << " ";
		s << _lpupil[i].rad  << " " << _rpupil[i].rad  << " "
		<< _lpupil[i].px   << " " << _rpupil[i].px   << " "
		<< _lpupil[i].py   << " " << _rpupil[i].py   << " "
		<< _lpupil[i].scelera.val[0] << " " << _rpupil[i].scelera.val[0] << " "
		<< _lpupil[i].scelera.val[1] << " " << _rpupil[i].scelera.val[1] << " "
		<< _lpupil[i].scelera.val[2] << " " << _rpupil[i].scelera.val[2] << " ";
		FACETRACKER::IO::WriteMat(s,_lpupil[i].idx);
		FACETRACKER::IO::WriteMat(s,_rpupil[i].idx);
		FACETRACKER::IO::WriteMat(s,_lpupil[i].tri);
		FACETRACKER::IO::WriteMat(s,_rpupil[i].tri);
		for(int j = 0; j < 3; j++){
			cv::Mat limg; _lpupil[i].image[j].convertTo(limg,CV_32S);
			cv::Mat rimg; _rpupil[i].image[j].convertTo(rimg,CV_32S);
			FACETRACKER::IO::WriteMat(s,limg);
			FACETRACKER::IO::WriteMat(s,rimg);
		}
		FACETRACKER::IO::WriteMat(s,_ocav_idx);
		FACETRACKER::IO::WriteMat(s,_ocav_tri);
	}
	FACETRACKER::IO::WriteMat(s,_basis);
	_warp.Write(s); _pdm.Write(s); _gpdm.Write(s); _gen.Write(s);
	for(unsigned i = 0; i < _images.size(); i++)
		FACETRACKER::IO::WriteMat(s,_expr[i]);
	
	return true;
}

bool
FaceAvatar::init(cv::Mat& f, cv::Mat& shape)
{
	//set parameters
	AvatarParam* p = &AVATARPARAM;
	
	dx0_ = dy0_ = 0.0;
	
	//set gscale
	cv::Mat g;
	cv::cvtColor(f, g ,CV_BGR2GRAY);
	
  //get 3D shape of user
  if((_user.rows != 3*_gpdm.nPoints()) || (_user.cols != 1))
	  _user.create(3*_gpdm.nPoints(),1,CV_64F);
	  _gpdm.CalcParams(shape,gplocal_,gpglobl_);
	  _gpdm.CalcShape3D(_user,gplocal_);
	  
	  //get pupil locations
	  this->GetPupils(g,shape,_lp0,_rp0);
	  this->WarpBackPupils(_lp0,_rp0,shape,_user);
	  
	  //compute parameter vectors in generic basis
	  if(p->alpha == 0){ //user identity regressor
    _reg.resize(_shapes.size());
    for(unsigned i = 0; i < _shapes.size(); i++)
		_reg[i] = cv::Mat::eye(_pdm.nModes(),_pdm.nModes(),CV_64F);
  }else{
	  //extract generic shape parameters for user
	  int n = _gpdm.nPoints();
	  std::vector<cv::Mat> X = _gen.Generate(shape);
	  cv::Mat Y(_pdm.nModes(),X.size(),CV_64F);
	  _user.copyTo(_pdm._M);
	  for(unsigned i = 0; i < X.size(); i++){
		  cv::Mat x = X[i](cv::Rect(0,0,1,2*n)),y = Y.col(i);
		  _pdm.CalcParams(x,plocal_,pglobl_); plocal_.copyTo(y);
	  }
	  if(p->alpha == 1){ //use semantic correspondences only
		  //learn user's basis
		  cv::Mat user_basis;
		  {
			  cv::SVD svd(Y); int i;
			  for(i = 0; i < Y.cols; i++){if(svd.w.db(i,0) < 1.0e-3)break;}
			  user_basis = svd.u(cv::Rect(0,0,i,Y.rows)).clone();
		  }
		  //learn mapping for each avatar
		  _reg.resize(_shapes.size());
		  for(unsigned k = 0; k < _shapes.size(); k++){
			  //learn avatar's basis
			  cv::Mat avatar_basis;
			  {
				  cv::SVD svd(_expr[k]); int i;
				  for(i = 0; i < _expr[k].cols; i++){if(svd.w.db(i,0) < 1.0e-3)break;}
				  avatar_basis = svd.u(cv::Rect(0,0,i,_expr[k].rows)).clone();
			  }
			  //learn within subspace mapping
			  cv::Mat P = user_basis.t()*Y;
			  cv::Mat Q = avatar_basis.t()*_expr[k];
			  cv::Mat H = P*P.t(),Hi;
			  for(int i = 0; i < H.rows; i++)H.db(i,i) += 1.0e+3;
				  cv::invert(H,Hi,cv::DECOMP_CHOLESKY);
			  cv::Mat R = Q*P.t()*Hi;
			  
			  //build regressor
			  _reg[k] = avatar_basis*R*user_basis.t();
		  }
	  }else{
		  //compute regularized regression
		  double val = (1.0-p->alpha)/p->alpha;
		  cv::Mat Hi,H=Y*Y.t()+val*cv::Mat::eye(_pdm.nModes(),_pdm.nModes(),CV_64F);
		  for(int i = 0; i < H.rows; i++)H.db(i,i) += 1.0e+3;
			  cv::invert(H,Hi,cv::DECOMP_CHOLESKY); _reg.resize(_shapes.size());
		  for(unsigned i = 0; i < _shapes.size(); i++){
			  _reg[i].create(_pdm.nModes(),_pdm.nModes(),CV_64F);
			  for(int j = 0; j < _pdm.nModes(); j++){
				  cv::Mat g = Y*_expr[i].row(j).t(); g.db(j,0) += val;
				  cv::Mat h = Hi*g; cv::Mat ht = h.t();
				  cv::Mat r = _reg[i].row(j); ht.copyTo(r);
			  }
		  }
	  }
  }
	
	return true;
}

bool
FaceAvatar::animate(cv::Mat& draw, cv::Mat& shape)
{
	shape.copyTo(_shape);
	p_ = cv::Scalar(0);
	gray_ = cv::Scalar(0);
//	if(rgb_.size() != 3){rgb_.resize(3);}
	cv::split(draw,rgb_);
	for(int j = 0; j < 3; j++){
		cv::Mat t = _textr[_idx](cv::Rect(0,j*_warp._nPix,1,_warp._nPix)) + gray_;
		_warp.UnVectorize(t,img_);
		_warp.Draw(img_,rgb_[j],_shape);
	}
	cv::merge(rgb_,draw);
	
	return true;
}

bool
FaceAvatar::animate_p(cv::Mat& draw, cv::Mat& shape)
{
	//set parameters
	AvatarParam* p = &AVATARPARAM;
	
	//draw basic texture
	if(p->avatar_shape){
		if(p->animate_rigid && p->animate_exprs)
			_shape=this->AnimateShape(shape,1);
		else if(p->animate_rigid && !p->animate_exprs){
			_user.copyTo(_pdm._M); _pdm.CalcParams(shape,plocal_,pglobl_);
			_shapes[_idx].copyTo(_pdm._M); p_ = cv::Scalar(0);
			_pdm.CalcShape2D(_shape,p_,pglobl_);
		}
		else if(!p->animate_rigid && p->animate_exprs){
			_shape = this->AnimateShape(shape,1);
			_shapes[_idx].copyTo(_pdm._M);
			pglobl_ = cv::Scalar(0);
			pglobl_.db(0,0) = 1;
			pglobl_.db(4,0) = draw.cols/2;
			pglobl_.db(5,0) = draw.rows/2;
			_pdm.CalcShape2D(_shape,p_,pglobl_);
		}else{
			_shapes[_idx].copyTo(_pdm._M);
			p_ = cv::Scalar(0);
			pglobl_ = cv::Scalar(0);
			pglobl_.db(0,0) = 1;
			pglobl_.db(4,0) = draw.cols/2;
			pglobl_.db(5,0) = draw.rows/2;
			_pdm.CalcShape2D(_shape,p_,pglobl_);
		}
	}
	else{
		p_ = cv::Scalar(0);
		shape.copyTo(_shape);
	}
	
	if(p->animate_textr)
		gray_ = _scale[_idx]*_basis*p_; //p_ set in AnimateShape(shape)
	else
		gray_ = cv::Scalar(0);
	if(rgb_.size() != 3){rgb_.resize(3);}
	cv::split(draw,rgb_);
	for(int j = 0; j < 3; j++){
		cv::Mat t = textr_(cv::Rect(0,j*_warp._nPix,1,_warp._nPix));
		t = _textr[_idx](cv::Rect(0,j*_warp._nPix,1,_warp._nPix)) + gray_;
		_warp.UnVectorize(t,img_);
		_warp.Draw(img_,rgb_[j],_shape);
	}
	//draw eyes
	if(p->animate_eyes){
		cv::Point lp,rp; double wl,wr,hl,hr,ldx,ldy,rdx,rdy,lrad,rrad,dx,dy;
		this->GetPupils(grayImg_,shape,lp,rp);
		this->WarpBackPupils(lp,rp,shape,_user);
		this->GetWidthHeight(_user,_lpupil[_idx].idx,wl,hl);
		this->GetWidthHeight(_user,_rpupil[_idx].idx,wr,hr);
		ldx = double(lp.x - _lp0.x)/wl; ldy = double(lp.y - _lp0.y)/hl;
		rdx = double(rp.x - _rp0.x)/wr; rdy = double(rp.y - _rp0.y)/hr;
		double ealpha = 0.3;
		dx = ealpha*dx0_ + (1.0-ealpha)*0.5*(ldx+rdx);
		dy = ealpha*dy0_ + (1.0-ealpha)*0.5*(ldy+rdy);
		dx0_ = dx; dy0_ = dy;
		this->GetWidthHeight(_shapes[_idx],_lpupil[_idx].idx,wl,hl);
		this->GetWidthHeight(_shapes[_idx],_rpupil[_idx].idx,wr,hr);
		lp.x = _lpupil[_idx].px + dx*wl; lp.y = _lpupil[_idx].py + dy*hl;
		rp.x = _rpupil[_idx].px + dx*wr; rp.y = _rpupil[_idx].py + dy*hr;
		this->WarpBackPupils(lp,rp,_shapes[_idx],_shape);
		_gpdm.CalcParams(shape,gplocal_,gpglobl_);
		lrad = _lpupil[_idx].rad*gpglobl_.db(0,0);
		rrad = _rpupil[_idx].rad*gpglobl_.db(0,0);
		
		//apply shading due to eye closing
		int n = _pdm.nPoints();
		double v =
		((fabs(_shape.db(41+n,0)-_shape.db(37+n,0))+
		  fabs(_shape.db(40+n,0)-_shape.db(38+n,0))+
		  fabs(_shape.db(47+n,0)-_shape.db(43+n,0))+
		  fabs(_shape.db(46+n,0)-_shape.db(44+n,0)))/
		 (fabs(_shape.db(39,0)-_shape.db(36,0))+
		  fabs(_shape.db(45,0)-_shape.db(42,0))))/
		((fabs(_user.db(41+n,0)-_user.db(37+n,0))+
		  fabs(_user.db(40+n,0)-_user.db(38+n,0))+
		  fabs(_user.db(47+n,0)-_user.db(43+n,0))+
		  fabs(_user.db(46+n,0)-_user.db(44+n,0)))/
		 (fabs(_user.db(39,0)-_user.db(36,0))+
		  fabs(_user.db(45,0)-_user.db(42,0))));
		std::vector<cv::Mat> limg(3),rimg(3);
		for(int l = 0; l < 3; l++){
			limg[l] = v*_lpupil[_idx].image[l];
			rimg[l] = v*_rpupil[_idx].image[l];
		}
		cv::Scalar lscelera = v*_lpupil[_idx].scelera;
		cv::Scalar rscelera = v*_rpupil[_idx].scelera;
		if(lscelera.val[0] > 255 || lscelera.val[1] > 255 || lscelera.val[2] > 255)
			lscelera = cv::Scalar(255,255,255);
		if(rscelera.val[0] > 255 || rscelera.val[1] > 255 || rscelera.val[2] > 255)
			rscelera = cv::Scalar(255,255,255);
		this->DrawPupil(lp.x,lp.y,lrad,_shape,limg,rgb_,lscelera,_lpupil[_idx].idx,
		    _lpupil[_idx].tri);
		this->DrawPupil(rp.x,rp.y,rrad,_shape,rimg,rgb_,rscelera,_rpupil[_idx].idx,
		    _rpupil[_idx].tri);
	}
	//draw oral cavity
	if(p->oral_cavity){ //copy oral cavity
//		this->GetIdxPts(shape ,_ocav_idx,opts1_,true);
//		this->GetIdxPts(_shape,_ocav_idx,opts2_,true);
//		this->WarpTexture(opts1_,opts2_,rgb,rgb_,_ocav_tri);
	}
	cv::merge(rgb_,draw);
	
	return true;
}

bool
FaceAvatar::add(cv::Mat &f, cv::Mat &shape, cv::Mat &eyes)
{
	bool norm_mouth = true;
	
	//get 3D expressions
	std::vector<cv::Mat> X; int n = _gpdm.nPoints();
	if(norm_mouth){
		//normalise mouth
		cv::Mat ss = shape.clone();
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
	}else X = _gen.Generate(shape);
	
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
	cv::Mat shape3D(3*_gpdm.nPoints(),1,CV_64F);
	if(norm_mouth){
		cv::Mat ss = expr.col(0).clone();
		_gpdm.CalcParams(ss,plocal,pglobl);
		_gpdm.CalcShape3D(shape3D,plocal);
		cv::Mat R(3,3,CV_64F),Ri(3,3,CV_64F);
		FACETRACKER::Euler2Rot(R,pglobl.db(1,0),pglobl.db(2,0),pglobl.db(3,0));
		R *= pglobl.db(0,0); cv::invert(R,Ri,cv::DECOMP_SVD);
		cv::Mat x(3,1,CV_64F),y(3,1,CV_64F);
		for(int j = 0; j < n; j++){
			x.db(0,0) = ss.db(j  ,0) - pglobl.db(4,0);
			x.db(1,0) = ss.db(j+n,0) - pglobl.db(5,0);
			x.db(2,0) =
			R.db(2,0)*shape3D.db(j    ,0)+
			R.db(2,1)*shape3D.db(j+n  ,0)+
			R.db(2,2)*shape3D.db(j+n*2,0);
			y = Ri*x;
			shape3D.db(j    ,0) = y.db(0,0);
			shape3D.db(j+n  ,0) = y.db(1,0);
			shape3D.db(j+n*2,0) = y.db(2,0);
		}
	}else{
		_gpdm.CalcParams(shape,plocal,pglobl);
		_gpdm.CalcShape3D(shape3D,plocal);
		cv::Mat R(3,3,CV_64F),Ri(3,3,CV_64F);
		FACETRACKER::Euler2Rot(R,pglobl.db(1,0),pglobl.db(2,0),pglobl.db(3,0));
		R *= pglobl.db(0,0); cv::invert(R,Ri,cv::DECOMP_SVD);
		cv::Mat x(3,1,CV_64F),y(3,1,CV_64F);
		for(int j = 0; j < n; j++){
			x.db(0,0) = shape.db(j  ,0) - pglobl.db(4,0);
			x.db(1,0) = shape.db(j+n,0) - pglobl.db(5,0);
			x.db(2,0) =
			R.db(2,0)*shape3D.db(j    ,0)+
			R.db(2,1)*shape3D.db(j+n  ,0)+
			R.db(2,2)*shape3D.db(j+n*2,0);
			y = Ri*x;
			shape3D.db(j    ,0) = y.db(0,0);
			shape3D.db(j+n  ,0) = y.db(1,0);
			shape3D.db(j+n*2,0) = y.db(2,0);
		}
	}
	_shapes.push_back(shape3D);
	
	//crop appearance
	cv::Mat gray,grayTex,crop; cv::cvtColor(f,gray,CV_BGR2GRAY);
	_warp.Crop(gray,crop,shape); _warp.Vectorize(crop,grayTex);
	_scale.push_back(cv::norm(grayTex));
	std::vector<cv::Mat> rgb(3); cv::split(f,rgb);
	std::vector<cv::Mat> crop_rgb(3);
	cv::Mat textr(3*_warp._nPix,1,CV_64F);
	for(int j = 0; j < 3; j++){
		cv::Mat t = textr(cv::Rect(0,j*_warp._nPix,1,_warp._nPix));
		_warp.Crop(rgb[j],crop_rgb[j],shape); _warp.Vectorize(crop_rgb[j],t);
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
	cv::Mat S = shape3D(cv::Rect(0,0,1,2*n)).clone();
	double a,b,tx,ty; FACETRACKER::CalcSimT(S,shape,a,b,tx,ty);
	cv::Mat sim = (cv::Mat_<float>(2,3)<<a,-b,tx,b,a,ty);
	cv::Mat wimg_o = crop,sim_o = sim;
	cv::Mat w_o;

	std::cout << sim.at<float>(0,0) << '\t' << sim.at<float>(0,1) << '\t' << sim.at<float>(0,2) << std::endl;
	std::cout << sim.at<float>(1,0) << '\t' << sim.at<float>(1,1) << '\t' << sim.at<float>(1,2) << std::endl;
	
	for(int j = 0; j < 3; j++){
		cv::Mat im_o = rgb[j]; cv::warpAffine(im_o,w_o,sim_o,wimg_o.size());
		w_o.copyTo(crop_rgb[j]);
		// ZHONG
		cv::imshow("zhong", w_o);
		cv::waitKey();
	}
	cv::Mat img_rgb; cv::merge(crop_rgb,img_rgb); _images.push_back(img_rgb);
	
	//******************
	//cvNamedWindow("test"); cv::imshow("test",img_rgb); cvWaitKey(0); exit(0);
	//******************
	
	//compute expression parameters under generic PDM
	std::vector<cv::Mat> p(expr.cols); shape3D.copyTo(_pdm._M);
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
		_gpdm.CalcParams(shape,plocal,pglobl);
		this->GetEyes(shape,f,
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
		shape3D = ds.clone(); std::vector<cv::Mat> rgb(3);
		for(int j = 0; j < 3; j++){
			rgb[j] = cv::Mat::zeros(rect.height+3,rect.width+3,CV_8U);
			cv::Mat t = textr(cv::Rect(0,j*_warp._nPix,1,_warp._nPix));
			cv::Mat crop; _warp.UnVectorize(t,crop);
			_warp.Draw(crop,rgb[j],ds);
		}
		cv::Mat lpt; this->GetIdxPts(shape3D,_lpupil[_idx].idx,lpt,true);
		cv::Mat rpt; this->GetIdxPts(shape3D,_rpupil[_idx].idx,rpt,true);
		cv::Rect lrect=this->GetBoundingBox(lpt),rrect=this->GetBoundingBox(rpt);
		_lpupil[_idx].px = lrect.x + lrect.width/2 ;
		_lpupil[_idx].py = lrect.y + lrect.height/2;
		_rpupil[_idx].px = rrect.x + rrect.width/2 ;
		_rpupil[_idx].py = rrect.y + rrect.height/2;
		std::vector<cv::Mat> rgb_clone(3);
		while(1){
			for(int j = 0; j < 3; j++)rgb_clone[j] = rgb[j].clone();
			this->DrawPupil(_lpupil[_idx].px,_lpupil[_idx].py,
							_lpupil[_idx].rad,shape3D,_lpupil[_idx].image,
							rgb_clone,_lpupil[_idx].scelera,_lpupil[_idx].idx,
							_lpupil[_idx].tri);
			this->DrawPupil(_rpupil[_idx].px,_rpupil[_idx].py,
							_rpupil[_idx].rad,shape3D,_rpupil[_idx].image,
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
		_idx = _lpupil.size()-1; shape3D = S; int n = _pdm.nPoints();
		cv::Rect rect = this->GetBoundingBox(shape3D); shape3D = shape.clone();
		for(int j = 0; j < n; j++){
			shape3D.db(j,0) -= rect.x-1; shape3D.db(j+n,0) -= rect.y-1;
		}
		cv::Mat lpt; this->GetIdxPts(shape3D,_lpupil[_idx].idx,lpt,true);
		cv::Mat rpt; this->GetIdxPts(shape3D,_rpupil[_idx].idx,rpt,true);
		cv::Rect lrect = this->GetBoundingBox(lpt);
		cv::Rect rrect = this->GetBoundingBox(rpt);
		_lpupil[_idx].px = lrect.x + lrect.width/2 ;
		_lpupil[_idx].py = lrect.y + lrect.height/2;
		_rpupil[_idx].px = rrect.x + rrect.width/2 ;
		_rpupil[_idx].py = rrect.y + rrect.height/2;
		//*************************************************************************
	}
	_idx = _shapes.size()-1;
	
	return true;
}
