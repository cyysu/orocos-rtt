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
 *		$Id: rchain.cpp,v 1.1.1.1 2002/08/26 14:14:21 rmoreas Exp $
 *		$Name:  $ 
 ****************************************************************************/


#include <chain/rchain.h>

#ifdef USE_NAMESPACE
namespace ORO_Geometry {	
#endif

RFrame RChain::RMountingPlate() const
// returns the actual mounting plate wrt base frame.
{
	return mp_base;
}


RTwist RChain::RTwist_base_mp(int joint)  const {
	return (joints[joint].M * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p-joints[joint].p);
}

 RTwist RChain::RTwist_base_base(int joint)  const {
	return joints[joint] * list[joint]->UnitTwist();
}


RTwist RChain::RTwist_mp_mp(int joint)  const {
	return  mp_base.M.Inverse(
				(joints[joint].M *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p-joints[joint].p)
			);
// you could also write :
//   return  mp_base.Inverse( (joints[joint]*list[joint]->UnitTwist()) );
// but it is not as efficient (position is transformed to base and to mp,
// this is avoided above.
}


RTwist RChain::RTwist_mp_base(int joint)  const{
	return mp_base.M.Inverse(
		    joints[joint] * list[joint]->UnitTwist()
		);
}


int RChain::NrOfJoints() const {
		return ChainList::NrOfJoints();
}

void RChain::Add(ChainElement* el) {
		ChainList::Add(el);
		joints.insert(joints.end(),RFrame());
}

RChain::~RChain() {
	joints.erase(joints.begin(),joints.end());
}
	
Frame RChain::FwdKin(const NEWMAT::ColumnVector& v) {
	NEWMAT::ColumnVector zero(v.Nrows());
	for (int i=1;i<=v.Nrows();i++) zero(i)=0.0;
	return RFwdKin(v,zero).GetFrame();
}


Twist RChain::Twist_base_base(int joint)  const{
	return joints[joint].GetFrame() * list[joint]->UnitTwist();
}

Twist RChain::Twist_base_mp(int joint)  const{
	return (joints[joint].M.R * 
		    list[joint]->UnitTwist()).RefPoint(mp_base.p.p-joints[joint].p.p);
}

Twist RChain::Twist_mp_mp(int joint)  const{
	return  mp_base.M.R.Inverse(
				(joints[joint].M.R *	list[joint]->UnitTwist()).
					RefPoint(mp_base.p.p-joints[joint].p.p)
			);
}

Twist RChain::Twist_mp_base(int joint)  const{
	return mp_base.M.R.Inverse(
		    joints[joint].GetFrame() * list[joint]->UnitTwist()
		);
}


Frame RChain::MountingPlate() const{
	return mp_base.GetFrame();
}

RFrame RChain::RFwdKin(const NEWMAT::ColumnVector& jval,const NEWMAT::ColumnVector& vel) {
	int i;
	int s=NrOfJoints();
	RFrame tmp(RFrame::Identity() );
	for (i=0;i<s;i++) {
		// tmp = Frame w.r.t base, from axis it
		tmp = tmp * RTf(*list[i],RDouble(jval(i+1),vel(i+1)));
		joints[i] = tmp;
	}
	mp_base = tmp* mp;
	return mp_base;
}



#ifdef USE_NAMESPACE
}
#endif
