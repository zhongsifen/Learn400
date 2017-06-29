//
//  _FaceAvatar.hpp
//  FaceX
//
//  Created by SIFEN ZHONG on 5/10/15.
//
//

#ifndef _FaceAvatar_h
#define _FaceAvatar_h

#include "FaceAvatar.hpp"

class IO {
public:
	enum { MYAVATAR = 0, MYAVATARPARAMS, DUMMY_LAST_DONT_USE };
};

class IOBinary : public IO {
public:
	enum{ MYAVATAR = DUMMY_LAST_DONT_USE + 1, MYAVATARPARAMS };
};

class AvatarParam {
public:
	int type;           /**< Type of parameters          */
	bool animate_rigid; /**< Animate head pose motion?   */
	bool animate_exprs; /**< Animate facial expressions? */
	bool avatar_shape;  /**< User avatar's shape?        */
	bool animate_textr; /**< Animate texture change?     */
	bool animate_eyes;  /**< Animate eyes?               */
	bool oral_cavity;   /**< Do oral cavity replacement? */
	double alpha;       /**< Generic vs specific weight  */
	
	AvatarParam() {
		type = IO::MYAVATARPARAMS;
		animate_rigid = true;
		animate_exprs =	false;
		avatar_shape =	false;
		animate_textr =	false;
		animate_eyes =	false;
		oral_cavity =	false;
		alpha = 0.001;
	}
};

static AvatarParam AVATARPARAM;

#endif /* _FaceAvatar_h */
