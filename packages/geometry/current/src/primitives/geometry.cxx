
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
 *		$Id: geometry.cpp,v 1.1.1.1.2.4 2003/07/18 14:49:50 psoetens Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include "geometry/error.h"
#include "geometry/error_stack.h"
#include "geometry/geometry.h"
#include "geometry/geometry_line.h"
#include "geometry/geometry_point.h"
#include "geometry/geometry_circle.h"
#include "geometry/geometry_composite.h"
#include "geometry/geometry_roundedcomposite.h"
#include "geometry/geometry_cyclic_closed.h"

#include <memory>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {
#endif

using namespace std;

#if HAVE_IOSTREAM

Geometry* Geometry::Read(istream& is) {
	// auto_ptr because exception can be thrown !
	IOTrace("Geometry::Read");
	char storage[64];
	EatWord(is,"[",storage,sizeof(storage));
	Eat(is,'[');
	if (strcmp(storage,"POINT")==0) {
		IOTrace("POINT");
		Frame startpos;
		is >> startpos;
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new Geometry_Point(startpos);
	} else 	if (strcmp(storage,"LINE")==0) {
		IOTrace("LINE");
		Frame startpos;
		Frame endpos;
		is >> startpos;
		is >> endpos;
		auto_ptr<Orientation> orient( Orientation::Read(is) );
		double eqradius;
		is >> eqradius;
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new Geometry_Line(startpos,endpos,orient.release(),eqradius);
	} else if (strcmp(storage,"CIRCLE")==0) {
		IOTrace("CIRCLE");
		Frame F_base_start;
		Vector V_base_center;
		Vector V_base_p;
		Rotation R_base_end;
		double alpha;
		double eqradius;
		is >> F_base_start;
		is >> V_base_center;
		is >> V_base_p;
		is >> R_base_end;
		is >> alpha;
		alpha *= deg2rad;
		auto_ptr<Orientation> orient( Orientation::Read(is) );
		is >> eqradius;
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new Geometry_Circle(
						F_base_start,
						V_base_center,
						V_base_p,
						R_base_end,
						alpha,
						orient.release() ,
						eqradius
					);
	} else if (strcmp(storage,"ROUNDEDCOMPOSITE")==0) {
		IOTrace("ROUNDEDCOMPOSITE");
		double radius;
		is >> radius;
		double eqradius;
		is >> eqradius;
		auto_ptr<Orientation> orient( Orientation::Read(is) );
		auto_ptr<Geometry_RoundedComposite> tr( 
			new Geometry_RoundedComposite(radius,eqradius,orient.release()) 
		);
		int size;
		is >> size;		
		int i;
		for (i=0;i<size;i++) {			
			Frame f;
			is >> f;
			tr->Add(f);
		}
		tr->Finish();
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return tr.release();
	} else if (strcmp(storage,"COMPOSITE")==0) {
		IOTrace("COMPOSITE");
		int size;
		auto_ptr<Geometry_Composite> tr( new Geometry_Composite() );
		is >> size;
		int i;
		for (i=0;i<size;i++) {			
			tr->Add(Geometry::Read(is));
		}
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return tr.release();
	} else if (strcmp(storage,"CYCLIC_CLOSED")==0) {
		IOTrace("CYCLIC_CLOSED");
		int times;
		auto_ptr<Geometry> tr( Geometry::Read(is) );
		is >> times;
		EatEnd(is,']');
		IOTracePop();
		IOTracePop();
		return new Geometry_Cyclic_Closed(tr.release(),times);
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

