/***************************************************************************
  tag: Peter Soetens  Thu Apr 22 20:40:59 CEST 2004  SignalGenerator.hpp 

                        SignalGenerator.hpp -  description
                           -------------------
    begin                : Thu April 22 2004
    copyright            : (C) 2004 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be
 
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 
 
#ifndef ORO_SIGNALGENERATOR_HPP
#define ORO_SIGNALGENERATOR_HPP

#include <control_kernel/DataServer.hpp>
#include <control_kernel/KernelInterfaces.hpp>
#include <signal_processing/Sine.hpp>
#include <pkgconf/system.h>
#ifdef OROPKG_EXECUTION_PROGRAM_PARSER
#include "execution/TemplateDataSourceFactory.hpp"
#include "execution/TemplateCommandFactory.hpp"
#endif


namespace ORO_ControlKernel
{
    using namespace ORO_SignalProcessing;

    /**
     * The Setpoints generated by the SignalGenerator.
     */
    struct SignalSetPoint :
        public ServedTypes< vector<double> >
    {
        SignalSetPoint()
        {
            this->insert(make_pair(0, "ChannelValues") );
        }
    };

    /**
     * @brief A Generator Component for generating all kinds of signals
     * for N channels.
     *
     * It only accepts commands through its
     * methods and not through the Command DataObject.
     * For now, it also does not read the Input or Model
     * DataObject. It only writes new SignalSetPoints
     * for each channel.
     * @ingroup kcomps kcomp_generator
     */
    template< class Base >
    class SignalGenerator
        : public Base
    {
    public:
        /**
         * @brief Create a signalgenerator with a fixed number of channels.
         */
        SignalGenerator(int num_channels =  1) 
            : Base("SignalGenerator"),
              max_chans("Channels", "The number of channels", num_channels)
        {
            channel.resize(max_chans);
            set_point.resize(max_chans, 0.0);

        }

        virtual bool componentLoaded() 
        {
            Base::SetPoint::dObj()->Get("ChannelValues", setp_DObj);
            setp_DObj->Set(set_point);
            return true;
        }            
                
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void calculate() 
        {
            int i=0;
            for( chaniter it= channel.begin(); it != channel.end(); ++it, ++i)
                if (it->enable && it->gen)
                    if ( !it->freeze )
                        set_point[i]  = it->factor * it->gen->result( HeartBeatGenerator::Instance()->secondsSince(it->timestamp) );
                else
                    set_point[i] = 0;
        }
            
        /**
         * @see KernelInterfaces.hpp class ModuleControlInterface
         */
        virtual void push()      
        {
            setp_DObj->Set( set_point );
        }

        /**
         * @name The SignalGenerator Commands.
         * @{
         */

        /**
         * @brief Generate a sine on a channel.
         * @param chan_num The channel to set a sine.
         * @param amplitude The amplitude of the sine.
         * @param frequence The frequence of the sine in Hz.
         * @param phase The phase shift of the sine (in degrees).
         */
        bool sine( unsigned int chan_num, double frequence, double phase )
        {
            if (chan_num >= max_chans )
                return false;
            delete channel[chan_num].gen;
            channel[chan_num].gen = new Sine( 1.0, frequence, phase * 360/(2* M_PI) ); 
            return true;
        }

        /**
         * @brief Multiply the output of a channel with a special value.
         */
        bool scaleValue( unsigned int chan_num, double factor )
        {
            if (chan_num >= max_chans )
                return false;
            channel[chan_num].factor = factor;
            return true;
        }

        /**
         * @brief Unmute a channel.
         *
         * @return true if chan_num was valid.
         */
        bool enableChannel( unsigned int chan_num )
        {
            if (chan_num >= max_chans )
                return false;
            channel[chan_num].freezetime = 0;
            channel[chan_num].timestamp = HeartBeatGenerator::Instance()->ticksGet();
            channel[chan_num].enable = true;
            return true;
        }

        /**
         *  @brief Mute a channel.
         *
         * @return true if chan_num was valid.
         */
        bool disableChannel( unsigned int chan_num )
        {
            if (chan_num >= max_chans )
                return false;
            channel[chan_num].enable = false;
            return true;
        }

        bool isEnabled( unsigned int chan_num )
        {
            if (chan_num >= max_chans )
                return false;
            return channel[chan_num].enable;
        }

        bool isFrozen( unsigned int chan_num )
        {
            if (chan_num >= max_chans )
                return false;
            return channel[chan_num].freeze;
        }

        /**
         * @brief Freeze or pause the output of the channel.
         *
         * It will resume
         * when unfreezed. During freeze, it keeps generating its
         * last setpoint.
         */
        bool freezeChannel( unsigned int chan_num)
        {
            // we can only freeze if the channel is enabled.
            if (chan_num >= max_chans || channel[chan_num].enable==false || channel[chan_num].freeze == true)
                return false;
            channel[chan_num].freeze = true;
            channel[chan_num].freezetime = HeartBeatGenerator::Instance()->ticksGet();
            return true;
        }

        /**
         * @brief Unfreeze the channel, it will continue to generate
         * new setpoints.
         */
        bool unfreezeChannel( unsigned int chan_num)
        {
            // we can only freeze if the channel is enabled.
            if (chan_num >= max_chans || channel[chan_num].enable==false || channel[chan_num].freeze == false)
                return false;
            // add the frozen time to the timestamp
            channel[chan_num].timestamp += HeartBeatGenerator::Instance()->ticksSince( channel[chan_num].freezetime );
            channel[chan_num].freeze = false;
            return true;
        }

        /**
         * @brief Return the current setpoint of a channel.
         */
        double channelValue( unsigned int chan_num )
        {
            if (chan_num >= max_chans )
                return 0;
            return channel[chan_num].set_point;
        }

        bool isValidChannel( unsigned int chan_num )
        {
            if (chan_num >= max_chans )
                return false;
            return true;
        }

        /**
         * @}
         */
        virtual bool updateProperties( const PropertyBag& bag )
        {
            /*
             * <simple name="Channels" type="integer"><value>8</value</simple>
             * <sequence>
             * <struct name="Channel" type="type_less">
             *   <simple name="Channel Nr"        type="integer"> <value>0</value></simple>
             *   <simple name="Channel Generator" type="string">  <value>sine</value></simple>
             * </struct>
             * </sequence>
             */
            return true; //decomposeProperty( max_chans, bag);
        }

#ifdef OROPKG_EXECUTION_PROGRAM_PARSER

        DataSourceFactoryInterface* createDataSourceFactory()
        {
            TemplateDataSourceFactory< SignalGenerator<Base> >* ret =
                newDataSourceFactory( this );
            ret->add( "channelValue", 
                      data( &SignalGenerator<Base>::channelValue, "The current value "
                            "of the channel.",
                            "Channel", "The number of the channel") );
            return ret;
        }

        template< class T >
        bool true_gen( T t ) const { return true; }

        CommandFactoryInterface* createCommandFactory()
        {
            TemplateCommandFactory< SignalGenerator<Base> >* ret =
                newCommandFactory( this );
            ret->add( "enableChannel", 
                      command( &SignalGenerator<Base>::enableChannel,
                               &SignalGenerator<Base>::isValidChannel,
                               "Turn on a channel.",
                               "Channel", "The channel to turn on."
                               ) );
            ret->add( "disableChannel", 
                      command( &SignalGenerator<Base>::disableChannel,
                               &SignalGenerator<Base>::isValidChannel,
                               "Turn off a channel.",
                               "Channel", "The channel to turn off."
                               ) );
            ret->add( "freezeChannel", 
                      command( &SignalGenerator<Base>::freezeChannel,
                               &SignalGenerator<Base>::isFrozen,
                               "Freeze a channel.",
                               "Channel", "The channel to freeze."
                               ) );
            ret->add( "resumeChannel", 
                      command( &SignalGenerator<Base>::unfreezeChannel,
                               &SignalGenerator<Base>::isFrozen,
                               "Resume a channel.",
                               "Channel", "The channel to resume, after a freeze.", true
                               ) );
            ret->add( "channelValue", 
                      command( &SignalGenerator<Base>::channelValue,
                               &SignalGenerator<Base>::isValidChannel,
                               "Get the value of a channel.",
                               "Channel", "The channel to inspect."
                               ) );
            ret->add( "scaleValue", 
                      command( &SignalGenerator<Base>::scaleValue,
                               &SignalGenerator<Base>::isValidChannel,
                               "Scale the output of a channel.",
                               "Channel", "The channel number.",
                               "Factor", "The scale factor."
                               ) );
            ret->add( "sine", 
                      command( &SignalGenerator<Base>::scaleValue,
                               &SignalGenerator<Base>::isValidChannel,
                               "Generate a sine on a channel.",
                               "Channel", "The channel number.",
                               "Frequence", "The frequency in Hz.",
                               "Phase", "The phase in degrees"
                               ) );
            return ret;
        }
#endif

    protected:
        Property<int> max_chans;

        /**
         * Holds all info about the state of a channel.
         */
        struct Channel
        {
            Channel() : factor(1.0), enable(false), gen(0) {}
            Channel(double f, bool e, SignalGeneratorInterface* g)
                : factor(f), enable(e), gen(g) {}
            double factor;
            bool enable;
            bool freeze;
            SignalGeneratorInterface* gen;
            // the time when the signal was started
            HeartBeatGenerator::ticks timestamp;
            // freezetime is the accumulated frozen time
            HeartBeatGenerator::ticks freezetime;
        };

        typedef typename std::vector<Channel>::iterator chaniter;
        std::vector<Channel> channel;
        std::vector<double> set_point;
        DataObjectInterface< std::vector<double> >* setp_DObj;
    };

}

#endif
