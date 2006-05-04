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
// be/be_codegen.cpp:925

#include "corba/CosPropertyServiceI.h"
#include <corelib/Property.hpp>
#include <corelib/PropertyBag.hpp>
#include <corelib/Logger.hpp>
#include <execution/AttributeRepository.hpp>
#include <vector>
#include <algorithm>


using namespace CosPropertyService;
using namespace ORO_CoreLib;
using namespace ORO_Execution;
using namespace std;

#if 0 // NOT IMPLEMENTED

// Implementation skeleton constructor
CosPropertyService_PropertySetFactory_i::CosPropertyService_PropertySetFactory_i (void)
{
}

// Implementation skeleton destructor
CosPropertyService_PropertySetFactory_i::~CosPropertyService_PropertySetFactory_i (void)
{
}

::CosPropertyService::PropertySet_ptr CosPropertyService_PropertySetFactory_i::create_propertyset (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

::CosPropertyService::PropertySet_ptr CosPropertyService_PropertySetFactory_i::create_constrained_propertyset (
    const ::CosPropertyService::PropertyTypes & allowed_property_types,
    const ::CosPropertyService::Properties & allowed_properties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::ConstraintNotSupported
  ))
{
  // Add your implementation here
}

::CosPropertyService::PropertySet_ptr CosPropertyService_PropertySetFactory_i::create_initial_propertyset (
    const ::CosPropertyService::Properties & initial_properties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
}

// Implementation skeleton constructor
CosPropertyService_PropertySetDefFactory_i::CosPropertyService_PropertySetDefFactory_i ()
{
}

// Implementation skeleton destructor
CosPropertyService_PropertySetDefFactory_i::~CosPropertyService_PropertySetDefFactory_i (void)
{
}

::CosPropertyService::PropertySetDef_ptr CosPropertyService_PropertySetDefFactory_i::create_propertysetdef (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

::CosPropertyService::PropertySetDef_ptr CosPropertyService_PropertySetDefFactory_i::create_constrained_propertysetdef (
    const ::CosPropertyService::PropertyTypes & allowed_property_types,
    const ::CosPropertyService::PropertyDefs & allowed_property_defs
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::ConstraintNotSupported
  ))
{
  // Add your implementation here
}

::CosPropertyService::PropertySetDef_ptr CosPropertyService_PropertySetDefFactory_i::create_initial_propertysetdef (
    const ::CosPropertyService::PropertyDefs & initial_property_defs
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
}

#endif // NOT IMPLEMENTED

// Implementation skeleton constructor
CosPropertyService_PropertySet_i::CosPropertyService_PropertySet_i (  PropertyBag* s )
    :bag(s), is_attr(false)
{
}

CosPropertyService_PropertySet_i::CosPropertyService_PropertySet_i (  AttributeRepository* ar )
    : bag( new PropertyBag() ), is_attr(true)
{
    vector<string> names = ar->attributes();
    for ( vector<string>::iterator it = names.begin(); it != names.end() ; ++it) {
        CORBA::Any_ptr any_value = ar->getValue(*it)->toDataSource()->getAny();
        bag->add( new ORO_CoreLib::Property<CORBA::Any>( *it, "Task Attribute", *any_value ) );
    }
}

// Implementation skeleton destructor
CosPropertyService_PropertySet_i::~CosPropertyService_PropertySet_i (void)
{
    if (is_attr) {
        deleteProperties( *bag );
        delete bag;
    }
}

void CosPropertyService_PropertySet_i::define_property (
    const char * property_name,
    const ::CORBA::Any & property_value
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::InvalidPropertyName,
    ::CosPropertyService::ConflictingProperty,
    ::CosPropertyService::UnsupportedTypeCode,
    ::CosPropertyService::UnsupportedProperty,
    ::CosPropertyService::ReadOnlyProperty
  ))
{
    // By default, store as Any, but if already present, try conversion.
    string pname( property_name );
    ORO_CoreLib::Property<CORBA::Any>* p = bag->getProperty<CORBA::Any>( pname );
    if ( p ) {
        Logger::log() << Logger::Debug << "Updating Property<CORBA::Any> "<< pname <<Logger::nl;
        p->value() = property_value;
    } else {
        PropertyBase* pbase = bag->find( pname );
        if ( pbase ) {
            Logger::log() << Logger::Debug << "Updating PropertyBase::update() of "<< pname <<Logger::nl;
            if ( pbase->getDataSource()->update( property_value ) == false )
                throw ::CosPropertyService::ConflictingProperty();
            return;
        }
        if ( is_attr )
            throw ::CosPropertyService::InvalidPropertyName();
        // new property...
        this->modified();
        Logger::log() << Logger::Debug << "Creating Property "<< pname <<Logger::nl;
        bag->add( new ORO_CoreLib::Property<CORBA::Any>( pname, "Defined Property", property_value ) );
    }
}

void CosPropertyService_PropertySet_i::define_properties (
    const ::CosPropertyService::Properties & nproperties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
    size_t i = 0;
    PropertyExceptions me;
    while ( i != nproperties.length() ) {
        try {
            this->define_property( nproperties[i].property_name, nproperties[i].property_value );
        } catch ( ConflictingProperty cp ) {
            me.length( me.length() + 1 );
            PropertyException pe;
            pe.reason = conflicting_property;
            pe.failing_property_name = nproperties[i].property_name;
            me[ me.length() -1 ] = pe;
        }
        ++i;
    }
    if ( me.length() != 0 )
        throw MultipleExceptions( me );
}

CORBA::ULong CosPropertyService_PropertySet_i::get_number_of_properties (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return bag->size();
}

void CosPropertyService_PropertySet_i::get_all_property_names (
    ::CORBA::ULong how_many,
    ::CosPropertyService::PropertyNames_out property_names,
    ::CosPropertyService::PropertyNamesIterator_out rest
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
    size_t i=0;
    size_t toRetrieve = how_many < bag->size() ? how_many : bag->size();
    PropertyBag::iterator ci = bag->begin();
    property_names = new ::CosPropertyService::PropertyNames();
    property_names->length( toRetrieve );
    while ( i < toRetrieve ) {
        property_names[i] = CORBA::string_dup( (*ci)->getName().c_str() );
        ++i;
        ++ci;
    }

    rest = ::CosPropertyService::PropertyNamesIterator::_nil();

    if ( i < bag->size() ) {
        Logger::log() << Logger::Info<< "Returning PropertyNamesIterator."<<Logger::endl;
        CosPropertyService_PropertyNamesIterator_i* pit = new CosPropertyService_PropertyNamesIterator_i(this, ci, bag->end() );
        n_iters.push_back( pit );
        CosPropertyService::PropertyNamesIterator_ptr pit_obj = pit->_this();
        rest = pit_obj;
    }
        
}

CORBA::Any * CosPropertyService_PropertySet_i::get_property_value (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::PropertyNotFound,
    ::CosPropertyService::InvalidPropertyName
  ))
{
    string pname( property_name );
    ORO_CoreLib::Property<CORBA::Any>* p = bag->getProperty<CORBA::Any>( pname );
    if ( p ) {
        Logger::log() << Logger::Debug<< "Returning value of Property<CORBA::Any>."<<Logger::endl;
        return new CORBA::Any( (p->get()) );
    } else {
        PropertyBase* pbase = bag->find( pname );
        if ( pbase ) {
            Logger::log() << Logger::Debug<< "Returning value of PropertyBase::getAny()."<<Logger::endl;
            return pbase->getDataSource()->getAny();
        }
        throw PropertyNotFound();
    }
}

CORBA::Boolean CosPropertyService_PropertySet_i::get_properties (
    const ::CosPropertyService::PropertyNames & property_names,
    ::CosPropertyService::Properties_out nproperties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    bool retval = true;
    size_t i = 0;
    nproperties = new CosPropertyService::Properties();
    nproperties->length( property_names.length() );
    while (i != property_names.length() ) {
        PropertyBase* pbase = bag->find( std::string(property_names[i]) );
        if ( pbase ) {
            ::CosPropertyService::Property pcopy;
            pcopy.property_name = CORBA::string_dup(property_names[i]);
            CORBA::Any_var any = pbase->getDataSource()->getAny();
            pcopy.property_value = *any;
            nproperties[i] = pcopy;
        } else
            retval = false;
        ++i;
    }
    return retval;
}

void CosPropertyService_PropertySet_i::get_all_properties (
    ::CORBA::ULong how_many,
    ::CosPropertyService::Properties_out nproperties,
    ::CosPropertyService::PropertiesIterator_out rest
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    size_t i=0;
    size_t toRetrieve = how_many < bag->size() ? how_many : bag->size();
    PropertyBag::iterator ci = bag->begin();
    nproperties = new CosPropertyService::Properties();
    nproperties->length( toRetrieve );
    Logger::log() << Logger::Debug << "Retrieving " <<toRetrieve <<" Properties" <<Logger::nl;
    while ( i < toRetrieve ) {
        nproperties[i].property_name = CORBA::string_dup( (*ci)->getName().c_str() );
        CORBA::Any_var any = this->get_property_value( nproperties[i].property_name.in() );
        nproperties[i].property_value = *any;
        ++i;
        ++ci;
    }
    rest = ::CosPropertyService::PropertiesIterator::_nil();
    if ( how_many < bag->size() ) {
        Logger::log() << Logger::Info<< "Returning PropertiesIterator."<<Logger::endl;
        CosPropertyService_PropertiesIterator_i* pit = new CosPropertyService_PropertiesIterator_i(this, ci, bag->end() );
        p_iters.push_back( pit );
        CosPropertyService::PropertiesIterator_ptr pit_obj = pit->_this();
        rest = pit_obj;
    }
}

void CosPropertyService_PropertySet_i::delete_property (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::PropertyNotFound,
    ::CosPropertyService::InvalidPropertyName,
    ::CosPropertyService::FixedProperty
  ))
{
  // Add your implementation here
    string pname( property_name );
    PropertyBase* pbase = bag->find( pname );
    // if present but not an Any property, do not erase it.
    if ( pbase && bag->getProperty<CORBA::Any>( pname ) == 0 ) {
        throw FixedProperty();
    }
    if (pbase) {
        Logger::log() << Logger::Debug << "Deleting Property "<< pname <<Logger::nl;
        this->modified();
        bag->remove(pbase);
        delete pbase;
        return;
    }
    throw PropertyNotFound();
    
}

void CosPropertyService_PropertySet_i::delete_properties (
    const ::CosPropertyService::PropertyNames & property_names
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
    size_t i = 0;
    PropertyExceptions pes;
    while ( i < property_names.length() ) {
        try {
            this->delete_property( property_names[i] );
        } catch ( FixedProperty& fp ) {
            pes.length( pes.length() +1);
            PropertyException pe;
            pe.reason = conflicting_property;
            pe.failing_property_name = CORBA::string_dup(property_names[i].in());
            pes[ pes.length() -1 ] = pe;
        } catch ( PropertyNotFound& pnf ) {
            pes.length( pes.length() +1);
            PropertyException pe;
            pe.reason = property_not_found;
            pe.failing_property_name = CORBA::string_dup(property_names[i].in());
            pes[ pes.length() -1 ] = pe;
        }
    }
    if ( pes.length() )
        throw MultipleExceptions( pes );
}

void CosPropertyService_PropertySet_i::modified()
{
    PV p_cache( p_iters);
    NV n_cache( n_iters );

    for ( PV::iterator it = p_cache.begin(); it !=p_cache.end(); ++it)
        (*it)->destroy();
    for ( NV::iterator it = n_cache.begin(); it !=n_cache.end(); ++it)
        (*it)->destroy();
}

void CosPropertyService_PropertySet_i::erasePIter(CosPropertyService_PropertiesIterator_i* pi)
{
    PV::iterator it = find( p_iters.begin(), p_iters.end(), pi );
    if ( it != p_iters.end() )
        p_iters.erase(it);
}

void CosPropertyService_PropertySet_i::eraseNIter(CosPropertyService_PropertyNamesIterator_i* pi)
{
    NV::iterator it = find( n_iters.begin(), n_iters.end(), pi );
    if ( it != n_iters.end() )
        n_iters.erase(it);
}

CORBA::Boolean CosPropertyService_PropertySet_i::delete_all_properties (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
    this->modified();
    bag->clear();
    return 1;
}

CORBA::Boolean CosPropertyService_PropertySet_i::is_property_defined (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::InvalidPropertyName
  ))
{
  // Add your implementation here
    string pname(property_name);
    return bag->find( pname );
}

#if 0 // NOT IMPLEMENTED

// Implementation skeleton constructor
CosPropertyService_PropertySetDef_i::CosPropertyService_PropertySetDef_i (void)
{
}

// Implementation skeleton destructor
CosPropertyService_PropertySetDef_i::~CosPropertyService_PropertySetDef_i (void)
{
}

void CosPropertyService_PropertySetDef_i::get_allowed_property_types (
    ::CosPropertyService::PropertyTypes_out property_types
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::get_allowed_properties (
    ::CosPropertyService::PropertyDefs_out property_defs
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::define_property_with_mode (
    const char * property_name,
    const ::CORBA::Any & property_value,
    ::CosPropertyService::PropertyModeType property_mode
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::InvalidPropertyName,
    ::CosPropertyService::ConflictingProperty,
    ::CosPropertyService::UnsupportedTypeCode,
    ::CosPropertyService::UnsupportedProperty,
    ::CosPropertyService::UnsupportedMode,
    ::CosPropertyService::ReadOnlyProperty
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::define_properties_with_modes (
    const ::CosPropertyService::PropertyDefs & property_defs
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
}

::CosPropertyService::PropertyModeType CosPropertyService_PropertySetDef_i::get_property_mode (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::PropertyNotFound,
    ::CosPropertyService::InvalidPropertyName
  ))
{
  // Add your implementation here
}

CORBA::Boolean CosPropertyService_PropertySetDef_i::get_property_modes (
    const ::CosPropertyService::PropertyNames & property_names,
    ::CosPropertyService::PropertyModes_out property_modes
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::set_property_mode (
    const char * property_name,
    ::CosPropertyService::PropertyModeType property_mode
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::InvalidPropertyName,
    ::CosPropertyService::PropertyNotFound,
    ::CosPropertyService::UnsupportedMode
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::set_property_modes (
    const ::CosPropertyService::PropertyModes & property_modes
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::define_property (
    const char * property_name,
    const ::CORBA::Any & property_value
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::InvalidPropertyName,
    ::CosPropertyService::ConflictingProperty,
    ::CosPropertyService::UnsupportedTypeCode,
    ::CosPropertyService::UnsupportedProperty,
    ::CosPropertyService::ReadOnlyProperty
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::define_properties (
    const ::CosPropertyService::Properties & nproperties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
}

CORBA::ULong CosPropertyService_PropertySetDef_i::get_number_of_properties (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::get_all_property_names (
    ::CORBA::ULong how_many,
    ::CosPropertyService::PropertyNames_out property_names,
    ::CosPropertyService::PropertyNamesIterator_out rest
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

CORBA::Any * CosPropertyService_PropertySetDef_i::get_property_value (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::PropertyNotFound,
    ::CosPropertyService::InvalidPropertyName
  ))
{
  // Add your implementation here
}

CORBA::Boolean CosPropertyService_PropertySetDef_i::get_properties (
    const ::CosPropertyService::PropertyNames & property_names,
    ::CosPropertyService::Properties_out nproperties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::get_all_properties (
    ::CORBA::ULong how_many,
    ::CosPropertyService::Properties_out nproperties,
    ::CosPropertyService::PropertiesIterator_out rest
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::delete_property (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::PropertyNotFound,
    ::CosPropertyService::InvalidPropertyName,
    ::CosPropertyService::FixedProperty
  ))
{
  // Add your implementation here
}

void CosPropertyService_PropertySetDef_i::delete_properties (
    const ::CosPropertyService::PropertyNames & property_names
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::MultipleExceptions
  ))
{
  // Add your implementation here
}

CORBA::Boolean CosPropertyService_PropertySetDef_i::delete_all_properties (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
}

CORBA::Boolean CosPropertyService_PropertySetDef_i::is_property_defined (
    const char * property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::CosPropertyService::InvalidPropertyName
  ))
{
  // Add your implementation here
}

#endif // NOT IMPLEMENTED


// Implementation skeleton constructor
CosPropertyService_PropertyNamesIterator_i::CosPropertyService_PropertyNamesIterator_i ( CosPropertyService_PropertySet_i* owner, PropertyBag::iterator it, PropertyBag::iterator end)
    : mowner(owner), bag_it(it), origin(it), bag_end(it)
{
}

// Implementation skeleton destructor
CosPropertyService_PropertyNamesIterator_i::~CosPropertyService_PropertyNamesIterator_i (void)
{
    this->destroy();
}

void CosPropertyService_PropertyNamesIterator_i::reset (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
    bag_it = origin;
}

CORBA::Boolean CosPropertyService_PropertyNamesIterator_i::next_one (
    CORBA::String_out property_name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if (bag_it == bag_end) {
        property_name = "";
        return 0;
    }
    property_name = CORBA::string_dup( (*bag_it)->getName().c_str() );
    ++bag_it;
    return 1;
}

CORBA::Boolean CosPropertyService_PropertyNamesIterator_i::next_n (
    ::CORBA::ULong how_many,
    ::CosPropertyService::PropertyNames_out property_names
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    // detect end.
    if ( bag_end == bag_it ) {
        property_names = new PropertyNames();
        return 0;
    }
    // fill as much as possible.
    property_names = new PropertyNames();
    property_names->length( how_many );
    size_t i = 0;
    while ( bag_it != bag_end && i < how_many ) {
        property_names[ i ] = CORBA::string_dup( (*bag_it)->getName().c_str() );
        ++bag_it;
        ++i;
    }
    // resize to actual fetched elements.
    property_names->length( i );
    return 1;
}

void CosPropertyService_PropertyNamesIterator_i::destroy (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    // make sure every next invocation fails.
    origin = bag_end;
    bag_it = bag_end;
    if (mowner)
        mowner->eraseNIter( this );
    mowner = 0;
}

// Implementation skeleton constructor
CosPropertyService_PropertiesIterator_i::CosPropertyService_PropertiesIterator_i (	CosPropertyService_PropertySet_i* owner, PropertyBag::iterator it, PropertyBag::iterator end)
  : mowner(owner), bag_it(it), origin(it), bag_end(end)
{
}

// Implementation skeleton destructor
CosPropertyService_PropertiesIterator_i::~CosPropertyService_PropertiesIterator_i (void)
{
    this->destroy();
}

void CosPropertyService_PropertiesIterator_i::reset (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    bag_it = origin;
}

CORBA::Boolean CosPropertyService_PropertiesIterator_i::next_one (
    ::CosPropertyService::Property_out aproperty
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    if (bag_it == bag_end) {
        aproperty = new CosPropertyService::Property();
        return 0;
    }
    aproperty->property_name = CORBA::string_dup( (*bag_it)->getName().c_str() );
    CORBA::Any_var any = (*bag_it)->getDataSource()->getAny();
    aproperty->property_value = *any;
    ++bag_it;
    return 1;
}

CORBA::Boolean CosPropertyService_PropertiesIterator_i::next_n (
    ::CORBA::ULong how_many,
    ::CosPropertyService::Properties_out nproperties
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    // detect end.
    if ( bag_end == bag_it ) {
        nproperties = new Properties();
        return 0;
    }
    // fill as much as possible.
    nproperties = new Properties();
    nproperties->length( how_many );
    size_t i = 0;
    while ( bag_it != bag_end && i < how_many ) {
        nproperties[ i ].property_name = CORBA::string_dup( (*bag_it)->getName().c_str() );
        CORBA::Any_var any = (*bag_it)->getDataSource()->getAny();
        nproperties[ i ].property_value = *any;
        ++bag_it;
        ++i;
    }
    // resize to actual fetched elements.
    nproperties->length( i );
    return 1;
}

void CosPropertyService_PropertiesIterator_i::destroy (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    // make sure every next invocation fails.
    origin = bag_end;
    bag_it = bag_end;
    if (mowner)
        mowner->erasePIter( this );
    mowner = 0;
}


