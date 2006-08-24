// -*- C++ -*-
//
// $Id$

// ****  Code generated by the The ACE ORB (TAO) IDL Compiler ****
// TAO and the TAO IDL Compiler have been developed by:
//       Center for Distributed Object Computing
//       Washington University
//       St. Louis, MO
//       USA
//       http://www.cs.wustl.edu/~schmidt/doc-center.html
// and
//       Distributed Object Computing Laboratory
//       University of California at Irvine
//       Irvine, CA
//       USA
//       http://doc.ece.uci.edu/
// and
//       Institute for Software Integrated Systems
//       Vanderbilt University
//       Nashville, TN
//       USA
//       http://www.isis.vanderbilt.edu/
//
// Information about TAO is available at:
//     http://www.cs.wustl.edu/~schmidt/TAO.html

// TAO_IDL - Generated from 
// be/be_codegen.cpp:910

#include "rtt/corba/AttributesI.h"
#include "rtt/corba/AttributesC.h"
#include <vector>
#include <rtt/PropertyBag.hpp>
#include <rtt/Property.hpp>




using namespace RTT;
using namespace RTT::Corba;
using namespace std;

// Implementation skeleton constructor
Orocos_AttributeInterface_i::Orocos_AttributeInterface_i (AttributeRepository* ar)
    :mar (ar), mbag(0)
{
}

Orocos_AttributeInterface_i::Orocos_AttributeInterface_i (PropertyBag* bag)
    :mar (0), mbag(bag)
{
}

// Implementation skeleton destructor
Orocos_AttributeInterface_i::~Orocos_AttributeInterface_i (void)
{
    // if no AttributeRepository, it must have been a PropertyBag.
    if ( mar == 0 )
        delete mbag;
}

::RTT::Corba::AttributeInterface::AttributeNames * Orocos_AttributeInterface_i::getAttributeList (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    ::RTT::Corba::AttributeInterface::AttributeNames_var ret = new ::RTT::Corba::AttributeInterface::AttributeNames();
    if ( !mar )
        return ret._retn();
    vector<string> names = mar->names();
    ret->length( names.size() );
    for(size_t i=0; i != names.size(); ++i)
        ret[i] = CORBA::string_dup( names[i].c_str() );
    return ret._retn();
}

::RTT::Corba::AttributeInterface::PropertyNames * Orocos_AttributeInterface_i::getPropertyList (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
    ::RTT::Corba::AttributeInterface::PropertyNames_var ret = new ::RTT::Corba::AttributeInterface::PropertyNames();
    if (mar)
        mbag = mar->properties(); // leave this here to get latest propertybag.
    if (mbag == 0)
        return ret._retn();
    ret->length( mbag->size() );
    PropertyBag::const_iterator it = mbag->getProperties().begin();
    size_t index = 0;
    for( ; it != mbag->getProperties().end(); ++it, ++index) {
        ::RTT::Corba::AttributeInterface::Property prop;
        prop.name = CORBA::string_dup( (*it)->getName().c_str() );
        prop.description = CORBA::string_dup( (*it)->getDescription().c_str() );
        ret[index] = prop;
    }
    return ret._retn();
}

::RTT::Corba::Expression_ptr Orocos_AttributeInterface_i::getAttribute (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if ( !mar || !mar->hasAttribute( string(name) ) )
        return ::RTT::Corba::Expression::_nil();
    return mar->getValue( string(name) )->getDataSource()->server();
}

::RTT::Corba::Expression_ptr Orocos_AttributeInterface_i::getProperty (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if (mar)
        mbag = mar->properties(); // leave this here to get latest propertybag.
    if ( mbag ==0 || !mbag->find( string(name) ) )
        return ::RTT::Corba::Expression::_nil();
    DataSourceBase::shared_ptr ds = mbag->find( string(name) )->getDataSource();
    return ds->server();
}


