/***************************************************************************
  tag: Peter Soetens  Mon May 10 19:10:37 CEST 2004  CommandParser.cxx

                        CommandParser.cxx -  description
                           -------------------
    begin                : Mon May 10 2004
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

#include "execution/parser-debug.hpp"
#include "execution/parse_exception.hpp"

#ifdef ORO_PRAGMA_INTERFACE
#pragma implementation
#endif
#include "execution/CommandParser.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <functional>
#include <algorithm>

#include "corelib/CommandNOP.hpp"
#include "corelib/CommandDataSource.hpp"
#include "corelib/ConditionTrue.hpp"
#include "corelib/ConditionOnce.hpp"

#include "execution/AsynchCommandDecorator.hpp"

#include "execution/TaskContext.hpp"
#include "execution/ArgumentsParser.hpp"
#include "execution/ConditionComposite.hpp"
#include "execution/CommandDispatch.hpp"

namespace ORO_Execution
{
    using boost::bind;
    using ORO_CoreLib::CommandNOP;
    using ORO_CoreLib::ConditionTrue;
    using namespace detail;

    namespace {
        assertion<std::string> expect_methodname("Expected a method call on object.");
        assertion<std::string> expect_args( "Expected method call arguments between ()." );
    }
  CommandParser::CommandParser( TaskContext* c, bool dispatch)
      : mdispatch( dispatch ),
        dcom(0), retcommand( 0 ),
        implicittermcondition( 0 ), peer(0), context( c ),
        argsparser( 0 ), expressionparser( c ), peerparser( c )
  {
    BOOST_SPIRIT_DEBUG_RULE( objectmethod );
    BOOST_SPIRIT_DEBUG_RULE( callcommand );
    BOOST_SPIRIT_DEBUG_RULE( nopcommand );
    BOOST_SPIRIT_DEBUG_RULE( command );
    BOOST_SPIRIT_DEBUG_RULE( arguments );

    command = nopcommand | callcommand;

    nopcommand = str_p( "nothing" )[
      bind( &CommandParser::seennopcommand, this ) ];

    // a function call: "x.y( arg1, arg2, ..., argN )"
    // a command can optionally be declared synchronous by
    // putting the word "sync" in front of it..
    callcommand =
        objectmethod [
           bind( &CommandParser::seenstartofcall, this ) ]
      >> expect_args( arguments[
           bind( &CommandParser::seencallcommand, this ) ]);

    // the "x.y" part of a function call..
    // when 'x.' is ommitted, 'this.' is assumed.
    objectmethod = !peerparser.parser()
        >> lexeme_d[ expect_methodname( commonparser.lexeme_identifier[
           bind( &CommandParser::seenmethodname, this, _1, _2 ) ])
        ];
  }

    TaskContext* CommandParser::setStack( TaskContext* tc )
    {
        //std::cerr<< "Commands: stack: "<< tc->getName()<<std::endl;
        return expressionparser.setStack( tc );
    }

    TaskContext* CommandParser::setContext( TaskContext* tc )
    {
        //std::cerr<< "Commands: context: "<< tc->getName()<<std::endl;
        context = tc;
        peerparser.setContext(tc);
        peerparser.reset();
        return expressionparser.setContext( tc );
    }

  void CommandParser::seennopcommand()
  {
    retcommand = new CommandNOP;
    implicittermcondition = new ConditionOnce( false );
  }

  void CommandParser::seenstartofcall()
  {
      mcurobject =  peerparser.object();
      peer = peerparser.peer();
      peerparser.reset();

      OperationInterface* obj = 0;
      if (mcurobject != "this") 
          if ( (obj = peer->getObject(mcurobject)) == 0 )
              throw parse_exception_no_such_component( peer->getName(), mcurobject+"."+mcurmethod );
          else {
              if ( obj->methods()->hasMember(mcurmethod) == false && obj->commands()->hasMember(mcurmethod) == false)
                  throw parse_exception_no_such_method_on_component( mcurobject, mcurmethod );
          }
      else {
          if ( peer->methods()->hasMember(mcurmethod) == false && peer->commands()->hasMember(mcurmethod) == false)
              throw parse_exception_no_such_method_on_component( peer->getName(), mcurmethod );
      }

      // ok object and method are found.
      argsparser = new ArgumentsParser( expressionparser, peer,
                                      mcurobject, mcurmethod );
      arguments = argsparser->parser();
  }

  void CommandParser::seencallcommand()
  {
      assert( peer );
    // not really necessary, since mcurobject and mcurmethod should
    // still be valid, but anyway :)
    mcurobject = argsparser->objectname();
    mcurmethod = argsparser->methodname();

    OperationInterface* obj = 0;
    CommandRepository::Factory* cfi = 0;
    MethodRepository::Factory* mfi = 0;
    if ( mcurobject != "this" ) {
        obj = peer->getObject(mcurobject);
        assert(obj);
        mfi = obj->methods();
        cfi = obj->commands();
    } else {
        mfi = peer->methods();
        cfi = peer->commands();
    }
    assert(mfi);
    assert(cfi);

    ComCon comcon;
    if ( cfi->hasMember( mcurmethod ) )
        try
            {
                // check if dispatching is required:
                // This check is done, here but we always dispatch anyway,
                // to allow CommandProcessor policies to be in effect.
                if ( peer->engine()->commands() != context->engine()->commands() || mdispatch ) {
                    // different, dispatch:
                    dcom = cfi->produce( mcurmethod, argsparser->result() );// ,false );
                } else {
                    // within same execution engine, no dispatch:
                    dcom = cfi->produce( mcurmethod, argsparser->result() );//, true );
                }
                comcon.first = dcom;
                comcon.second = dcom->createCondition();
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (mcurobject, mcurmethod, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( mcurobject, mcurmethod, e.whicharg, e.expected_, e.received_ );
            }
        catch( ... )
            {
                assert( false );
            }
    else if ( mfi->hasMember( mcurmethod ) )
        try
            {
                // if the method returns a boolean, construct it as a command
                // which accepts/rejects the result.
                DataSourceBase* dsb =  mfi->produce( mcurmethod, argsparser->result() );
                DataSource<bool>* dsb_res = DataSource<bool>::narrow( dsb );
                if ( dsb_res == 0 )
                    comcon.first =  new CommandDataSource( dsb );
                else
                    comcon.first =  new CommandDataSourceBool( dsb_res );
                comcon.second = new ConditionTrue();
            }
        catch( const wrong_number_of_args_exception& e )
            {
                throw parse_exception_wrong_number_of_arguments
                    (mcurobject, mcurmethod, e.wanted, e.received );
            }
        catch( const wrong_types_of_args_exception& e )
            {
                throw parse_exception_wrong_type_of_argument
                    ( mcurobject, mcurmethod, e.whicharg, e.expected_, e.received_ );
            }
        catch( ... )
            {
                assert( false );
            }

    mcurobject.clear();
    mcurmethod.clear();
    delete argsparser;
    argsparser = 0;

    // argumentsparser.result() should have been properly filled up,
    // and mcurobject.mcurmethod should exist, we checked that in
    // seenstartofcall() already, so com should really be valid..
    if ( ! comcon.first )
      throw parse_exception_semantic_error(
        "Something weird went wrong in calling method \"" + mcurmethod +
        "\" on object \"" + mcurobject + "\"." );
    if ( ! comcon.second )
        comcon.second = new ConditionTrue;

    retcommand = comcon.first;
    implicittermcondition = comcon.second;
  }

    CommandParser::~CommandParser()
    {
        // if argsparser is non-zero, then something went wrong ( someone
        // threw an exception, and we didn't reach seencallcommand() ), so
        // we need to delete the argsparser..
        delete argsparser;
        delete retcommand;
        delete implicittermcondition;
    }

    void CommandParser::reset()
      {
        peer = 0;
        dcom = 0;
        retcommand = 0;
        implicittermcondition = 0;
        mcurobject.clear();
        mcurmethod.clear();
      }

}
