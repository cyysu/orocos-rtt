/*****************************************************************************
 *  \author 
 *  	Erwin Aertbelien, Div. PMA, Dep. of Mech. Eng., K.U.Leuven
 *
 *  \version 
 *		ORO_Geometry V0.2
 *
 *	\par History
 *		- $log$
 *
 *	\par Release
 *		$Id: orientation.cpp,v 1.1.1.1.2.3 2003/02/24 13:13:06 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/

#include "geometry/error.h"
#include "geometry/error_stack.h"
#include "geometry/orientation.h"
#include "geometry/orientation_singleaxis.h"

#include <memory>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

using namespace std;

#ifdef OROINT_OS_STDIOSTREAM
Orientation* Orientation::Read(istream& is) {
	// auto_ptr because exception can be thrown !
	IOTrace("Orientation::Read");
	char storage[64];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"SINGLEAXIS")==0) {
		IOTrace("SINGLEAXIS");
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new Orientation_SingleAxis();
	} else if (strcmp(storage,"THREEAXIS")==0) {
		IOTrace("THREEAXIS");
#ifdef HAVE_EXCEPTIONS
		throw Error_Not_Implemented();
#endif
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return NULL;
	} else if (strcmp(storage,"TWOAXIS")==0) {
		IOTrace("TWOAXIS");
#ifdef HAVE_EXCEPTIONS
		throw Error_Not_Implemented();
#endif
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return NULL;
	} else {
#ifdef HAVE_EXCEPTIONS
		throw Error_MotionIO_Unexpected_Traj();
#endif
	}
	return NULL; // just to avoid the warning;
}

#endif


#ifdef USE_NAMESPACE
}
#endif

