/***************************************************************************
  tag: Peter Soetens  Tue Jul 20 17:30:55 CEST 2004  StateDescription.cxx

                        StateDescription.cxx -  description
                           -------------------
    begin                : Tue July 20 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place,                                    *
 *   Suite 330, Boston, MA  02111-1307  USA                                *
 *                                                                         *
 ***************************************************************************/


#include "execution/StateDescription.hpp"

namespace ORO_Execution {
  StateDescription* StateDescription::postponeState()
  {
    StateDescription* ret = new StateDescription( "__pp__" + getName(), entrypoint );
    ret->setEntryProgram( mentry );
    ret->setRunProgram( mrun );
    ret->setHandleProgram( mhandle );
    ret->setExitProgram( mexit );
    ret->setDefined( isDefined() );
    // we don't use setEntryProgram cause that deletes the old
    // program...
    mentry = mrun = mhandle = mexit = 0;
    setDefined( false );
    return ret;
  }

  StateDescription::~StateDescription() {
            delete mentry;
            delete mexit;
            delete mhandle;
            delete mrun;
  }

  StateDescription* StateDescription::copy( std::map<const DataSourceBase*, DataSourceBase*>& replacementdss ) const
  {
    StateDescription* ret = new StateDescription( name, entrypoint );
    ret->inited = isDefined();
    if ( mentry )
    {
      ret->mentry = mentry->copy( replacementdss );
    }
    if ( mexit )
    {
      ret->mexit = mexit->copy( replacementdss );
    }
    if ( mhandle )
    {
      ret->mhandle = mhandle->copy( replacementdss );
    }
    if ( mrun )
    {
      ret->mrun = mrun->copy( replacementdss );
    }
    return ret;
  }

}
