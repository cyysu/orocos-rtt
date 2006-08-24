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

#include "rtt/corba/ScriptingAccessI.h"
#include "rtt/ScriptingAccess.hpp"
#include "rtt/ProgramLoader.hpp"
#include "rtt/parse_exception.hpp"
#include <sstream>

using namespace std;
using namespace RTT;
using namespace RTT::Corba;


// Implementation skeleton constructor
Orocos_ScriptingAccess_i::Orocos_ScriptingAccess_i (RTT::ScriptingAccess* ee)
    :mee(ee)
{
}

// Implementation skeleton destructor
Orocos_ScriptingAccess_i::~Orocos_ScriptingAccess_i (void)
{
}

void Orocos_ScriptingAccess_i::loadPrograms (
    const char * code,
    const char * filename
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::RTT::Corba::ScriptingAccess::LoadException
  ))
{
    try {
        mee->loadPrograms( code, filename, true );
    }
    catch (program_load_exception &ple) {
        throw ::RTT::Corba::ScriptingAccess::LoadException( ple.what() );
    } catch(file_parse_exception &fpe) {
        throw ::RTT::Corba::ScriptingAccess::LoadException( fpe.what().c_str() );
    }
}

void Orocos_ScriptingAccess_i::unloadProgram (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::RTT::Corba::ScriptingAccess::LoadException
  ))
{
    try {
        mee->unloadProgram( name, true );
    }
    catch (program_unload_exception &ple) {
        throw ::RTT::Corba::ScriptingAccess::LoadException( ple.what() );
    }
}

::RTT::Corba::ScriptingAccess::ProgramNames * Orocos_ScriptingAccess_i::getPrograms (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    std::vector<std::string> vlist = mee->getPrograms();
    ::RTT::Corba::ScriptingAccess::ProgramNames_var result = new ::RTT::Corba::ScriptingAccess::ProgramNames();
    result->length( vlist.size() );
    for (unsigned int i=0; i != vlist.size(); ++i )
        result[i] = CORBA::string_dup( vlist[i].c_str() );
    return result._retn();
}

CORBA::Long Orocos_ScriptingAccess_i::getProgramLine (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return mee->getProgramLine( name );
}

CORBA::Boolean Orocos_ScriptingAccess_i::hasProgram (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return mee->hasProgram( name );
}

char * Orocos_ScriptingAccess_i::getProgramText (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return CORBA::string_dup( mee->getProgramText(name).c_str() );
}

char * Orocos_ScriptingAccess_i::getProgramStatus (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return CORBA::string_dup( mee->getProgramStatus(name).c_str() );
}

void Orocos_ScriptingAccess_i::loadStateMachines (
    const char * code,
    const char * filename
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::RTT::Corba::ScriptingAccess::LoadException
  ))
{
    try {
        mee->loadStateMachines( code, filename, true);
    }
    catch (program_load_exception &ple) {
        throw ::RTT::Corba::ScriptingAccess::LoadException( ple.what() );
    } catch(file_parse_exception &fpe) {
        throw ::RTT::Corba::ScriptingAccess::LoadException( fpe.what().c_str() );
    }
}

void Orocos_ScriptingAccess_i::unloadStateMachine (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException,
    ::RTT::Corba::ScriptingAccess::LoadException
  ))
{
    try {
        mee->unloadStateMachine( name, true );
    }
    catch (program_unload_exception &ple) {
        throw ::RTT::Corba::ScriptingAccess::LoadException( ple.what() );
    }
}

::RTT::Corba::ScriptingAccess::StateMachineNames * Orocos_ScriptingAccess_i::getStateMachines (
    
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
  // Add your implementation here
    std::vector<std::string> vlist = mee->getStateMachines();
    ::RTT::Corba::ScriptingAccess::StateMachineNames_var result = new ::RTT::Corba::ScriptingAccess::StateMachineNames();
    result->length( vlist.size() );
    for (unsigned int i=0; i != vlist.size(); ++i )
        result[i] = CORBA::string_dup( vlist[i].c_str() );
    return result._retn();
}

CORBA::Boolean Orocos_ScriptingAccess_i::hasStateMachine (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return mee->hasStateMachine( name );
}

char * Orocos_ScriptingAccess_i::getStateMachineText (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return CORBA::string_dup( mee->getStateMachineText(name).c_str() );
}

char * Orocos_ScriptingAccess_i::getStateMachineStatus (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return CORBA::string_dup( mee->getStateMachineStatus(name).c_str() );
}

CORBA::Long Orocos_ScriptingAccess_i::getStateMachineLine (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return mee->getStateMachineLine( name );
}

char * Orocos_ScriptingAccess_i::getCurrentState (
    const char * name
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return CORBA::string_dup( mee->getCurrentState(name).c_str() );
}

CORBA::Boolean Orocos_ScriptingAccess_i::execute (
    const char * code
  )
  ACE_THROW_SPEC ((
    CORBA::SystemException
  ))
{
    return mee->execute( code );
}
