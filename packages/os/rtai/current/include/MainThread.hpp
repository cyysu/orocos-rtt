/***************************************************************************
  tag: Peter Soetens  Thu Oct 10 16:16:56 CEST 2002  MainThread.hpp 

                        MainThread.hpp -  description
                           -------------------
    begin                : Thu October 10 2002
    copyright            : (C) 2002 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef MAINTHREAD_HPP
#define MAINTHREAD_HPP

#include "fosi.h"
#include "rtstl/rtstl.hpp"
#include "ComponentThreaded.hpp"
#include "Time.hpp"

namespace ORO_OS
{

    /**
     * This class initializes the scheduler on construction
     */

    class SchedPolicy
    {

        public:
            SchedPolicy()
            {
                if ( rt_is_hard_timer_running() )
                    return;
                // BE SURE TO SET rt_preempt_always(1) when using one shot mode
                // we want low priority threads to be preempted by high ones.
                //set_periodic_mode();
                set_oneshot_mode();
                //            cout <<"Starting timer with "<<int(Count::from_time( 1000*100 )) <<" ticks"<<endl;
                start_timer( Count::from_time( usecs_to_nsecs(100) ) ); // 0.1ms = 100us
            }

            virtual ~SchedPolicy()
            {
                //we don't stop the timer.
            }
    };


    /**
     * This class simulates a main() thread so that ipc is 
     * possible in init/cleanup module
     *
     * This is RTAI specific
     */
    class MainThread
                : protected SchedPolicy, public ComponentThreaded
    {
            static MainThread* mt;

        public:
            static MainThread* Instance()
            {
                if ( mt == 0 )
                    mt = new MainThread();

                return mt;
            }

            static void Release()
            {
                if ( mt != 0 )
                    delete mt;

                mt = 0;
            }

            /**
             * The function to be queued
             * Use this as a base class if you want a function
             * to be execute()'d
             */

            struct Functor
            {
                virtual void operator() () const = 0;
            };

            /**
             * Starts a task and returns when the
             * task effectively runs
             */
            void startTask( ComponentThreaded* t )
            {}

            /**
             * Stops a task and returns when the
             * task has effectively stopped
             */
            void stopTask ( ComponentThreaded* t )
            {}

            /**
             * Executes a function object in the main thread
             */
            void execute( Functor* funcObj )
            {
                q.push( funcObj );
            }

            /**
             * Complete all pending requests
             */
            void complete()
            {
                /**
                if ( isRunning() )
                    while ( !q.empty() )
                        rt_busy_sleep( nano2count(1000) );
                */
            }

        protected:

            virtual void step()
            {
                while ( !q.empty() )
                {
                    //      cout << "MainThread : processing sthing"<<endl;
                    Functor * f = q.front();
                    f->operator() ();
                    q.pop();
                }
            }

            MainThread()
                :ComponentThreaded(10, "MainThread") //priority=10
            {
                periodSet( 0, usecs_to_nsecs(1000) ); // 1ms = 1000us
            }

            virtual ~MainThread()
            {
            }

        private:
            /**
             * A queue of to be executed function objects.
             */
            std::queue<Functor*> q;
    };

}

#endif
