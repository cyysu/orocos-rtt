#include "control_kernel/ExecutionExtension.hpp"

#include <execution/CommandFactoryInterface.hpp>
#include <execution/GlobalCommandFactory.hpp>
#include <execution/DataSourceFactory.hpp>
#include <execution/ProgramGraph.hpp>
#include <execution/Parser.hpp>

#include <execution/TemplateCommandFactory.hpp>

#include <functional>
#include <boost/function.hpp>

namespace ORO_ControlKernel
{
    using namespace ORO_Execution;
    using namespace boost;

    ExecutionExtension::ExecutionExtension( KernelBaseFunction* _base )
        : detail::ExtensionInterface( "Execution" ), program(0), context(0),
          running_progr(false),count(0), base( _base ),
          interval("Interval", "The relative interval of executing a program node \
with respect to the Kernels period. Should be strictly positive ( > 0).", 1)
    {
    }

    ExecutionExtension::~ExecutionExtension()
    {
    }

    using std::cerr;
    using std::endl;

    // is this still needed ???
    TaskInterface* ExecutionExtension::getTask() const
    {
        return base->getTask();
    }
    
    void ExecutionExtension::setTask( TaskInterface* task )
    {
        base->setTask( task );
    }

    bool ExecutionExtension::initialize() 
    { 
        if ( !proc.startStateContext("Default") )
            cerr << "Processor could not start \"Default\" StateContext."<<endl;
        return true;
    }

    void ExecutionExtension::startProgram(const std::string& name)
    {
        proc.resetProgram(name);
        proc.startProgram(name);
    }

    bool ExecutionExtension::isProgramRunning(const std::string& name) const
    {
        return proc.isProgramRunning(name);
    }

    void ExecutionExtension::stopProgram(const std::string& name)
    {
        proc.stopProgram(name);
    }

    void ExecutionExtension::resetProgram(const std::string& name)
    {
        proc.resetProgram(name);
    }

    bool ExecutionExtension::loadProgram( std::istream& prog_stream, const std::string& name )
    {
        initKernelCommands();
        Parser    parser;
        program = parser.parseProgram( prog_stream, &proc, this );
        if (program == 0) 
            return false;
        proc.loadProgram(name, program);
        return true;
    }

    bool ExecutionExtension::loadStateContext( std::istream& state_stream, const std::string& name )
    {
        initKernelCommands();
        Parser    parser;
        context = parser.parseStateContext( state_stream, &proc, this );
        if (context == 0) 
            return false;
        proc.loadStateContext(name, context);
        return true;
    }

    bool ExecutionExtension::isStateContextRunning(const std::string& name) const
    {
        return proc.isStateContextRunning(name);
    }

    void ExecutionExtension::resetStateContext(const std::string& name)
    {
        proc.resetStateContext(name);
    }

    void ExecutionExtension::step() {
        if ( count % interval == 0 )
            {
                count = 0;
                proc.doStep();
            }
        ++count;
    }

    void ExecutionExtension::finalize() 
    {
        proc.stopStateContext("Default");
    }

    CommandFactoryInterface* ExecutionExtension::createCommandFactory()
    {
        // see initKernelCommands()
        return 0;
    }

    DataSourceFactory* ExecutionExtension::createDataSourceFactory()
    {
        // see initKernelCommands()
        return 0;
    }

    void ExecutionExtension::initKernelCommands()
    {
        // I wish I could do this cleaner, but I can not do it
        // in the constructor (to early) and not in initialize (to late).
        static bool is_called = false;
        if ( is_called )
            return;
        is_called = true;

        // Add the commands / data of the kernel :
        commandfactory = createCommandFactory();
        if ( commandfactory )
            commandFactory().registerObject( "kernel", commandfactory );

        dataSourceFactory = createDataSourceFactory();
        if ( dataSourceFactory )
            dataFactory().registerObject( "kernel", dataSourceFactory );

        // Add the commands/ data of the EE:
        TemplateCommandFactory< ExecutionExtension  >* ret =
            newCommandFactory( this );
        ret->add( "startProgram", 
                  command
                  ( &ExecutionExtension::startProgram ,
                    &ExecutionExtension::isProgramRunning ,
                    "Start a program", "Name", "The Name of the Loaded Program" ) );
        ret->add( "stopProgram", 
                  command
                  ( &ExecutionExtension::stopProgram ,
                    //bind(&ExecutionExtension::foo, _1, mem_fn(&ExecutionExtension::isProgramRunning), std::logical_not<bool>() ),
                    &ExecutionExtension::isProgramRunning ,
                    "Stop a program", "Name", "The Name of the Started Program", true ) ); // true ==  invert the result.
        ret->add( "resetProgram", 
                  command
                  ( &ExecutionExtension::resetProgram ,
                    &ExecutionExtension::isProgramRunning ,
                    "Reset a program", "Name", "The Name of the Stopped Program", true ) );
        ret->add( "startStateContext", 
                  command
                  ( &ExecutionExtension::startStateContext ,
                    &ExecutionExtension::isStateContextRunning ,
                    "Start a program", "Name", "The Name of the Loaded StateContext" ) );
        ret->add( "stopStateContext", 
                  command
                  ( &ExecutionExtension::stopStateContext ,
                    &ExecutionExtension::isStateContextRunning ,
                    "Stop a program", "Name", "The Name of the Started StateContext", true ) );
        ret->add( "resetStateContext", 
                  command
                  ( &ExecutionExtension::resetStateContext ,
                    &ExecutionExtension::isStateContextRunning ,
                    "Reset a stateContext", "Name", "The Name of the Stopped StateContext", true ) );
        commandFactory().registerObject( "engine", ret );
    }

    ExecutionComponentInterface::ExecutionComponentInterface( const std::string& _name )
        : detail::ComponentAspectInterface<ExecutionExtension>( _name + std::string( "::Execution" ) ),
          name( _name ), master( 0 ),
          commandfactory( 0 ), dataSourceFactory( 0 )
    {
    }

    bool ExecutionComponentInterface::enableAspect( ExecutionExtension* ext )
    {
        master = ext;
        commandfactory = createCommandFactory();
        if ( commandfactory )
            master->commandFactory().registerObject( name, commandfactory );
        dataSourceFactory = createDataSourceFactory();
        if ( dataSourceFactory )
            master->dataFactory().registerObject( name, dataSourceFactory );
        return true;
    }

    bool ExecutionExtension::updateProperties( const PropertyBag& bag )
    {
        // The interval for executing the program, relative to the
        // kernel period. This property is optional.
        composeProperty(bag, interval);
        // check for validity.
        return interval > 0;
    }

    void ExecutionComponentInterface::disableAspect()
    {
        master->commandFactory().unregisterObject( name );
        master->dataFactory().unregisterObject( name );
    }

    ExecutionComponentInterface::~ExecutionComponentInterface()
    {
    }

    CommandFactoryInterface* ExecutionComponentInterface::createCommandFactory()
    {
        return 0;
    }

    DataSourceFactory* ExecutionComponentInterface::createDataSourceFactory()
    {
        return 0;
    }
}

