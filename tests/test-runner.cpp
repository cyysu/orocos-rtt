/***************************************************************************
  tag: Peter Soetens  Mon Jan 10 15:59:50 CET 2005  test-runner.cpp

                        test-runner.cpp -  description
                           -------------------
    begin                : Mon January 10 2005
    copyright            : (C) 2005 Peter Soetens
    email                : peter.soetens@mech.kuleuven.ac.be

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <os/main.h>
#include <Logger.hpp>
#include <iostream>

#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>

using boost::unit_test::test_suite;

using namespace RTT;
using namespace std;

struct InitOrocos {
public:
	InitOrocos(){  }
	~InitOrocos(){ 
#ifndef OROCOS_TARGET_XENOMAI
        __os_exit(); 
#endif
}

};

BOOST_GLOBAL_FIXTURE( InitOrocos )

boost::unit_test::test_suite* init_unit_test_suite(int argc, char** const argv)
{
    if ( argc > 1 && strncmp(argv[1],"--help",6) == 0 ) {
        cout << "This unit test executable takes the following options:" <<endl<<endl;
        cout << "The starred option is the default, available options depend on Boost UTF library version." <<endl<<endl;
        cout << "  --build_info[=<yes|no*>]          "<<endl;
        cout << "  --catch_system_errors[=<yes*|no>  "<<endl;
        cout << "  --detect_memory_leaks[=<yes*|no>  "<<endl;
        cout << "  --log_format[=<HRF*|XML>          "<<endl;
        cout << "  --log_level[=<all|success|test_suite|message|warning|error*|cpp_exception|system_error|fatal_error|nothing>"<<endl;
        cout << "  --result_code[=<yes*|no>          "<<endl;
        cout << "  --output_format[=<HRF*|XML>       "<<endl;
        cout << "  --random[=<0*|1|>1>               "<<endl;
        cout << "  --report_format[=<HRF*|XML>       "<<endl;
        cout << "  --report level[=<no|confirm*|short|detailed>"<<endl;
        cout << "  --show_progress[=<yes|no*>        "<<endl<<endl;
        cout << "  --use_alt_stack[=<yes*|no>        "<<endl<<endl;
        
        cout << "Select tests by using the form:"<<endl;
        cout << "  " << argv[0] << " --run_test=suite/testX"<<endl;
        cout << "Wildcards are accepted:"<<endl;
        cout << "  " << argv[0] << " --run_test=*/testX"<<endl;
        exit(0);
    }

	__os_init(argc, argv);

    // disable logging of errors or warnings if no ORO_LOGLEVEL was set.
    if ( log().getLogLevel() == Logger::Warning ) {
        log(Info) << "Lowering LogLevel to Critical." << endlog();
        log().setLogLevel(Logger::Critical);
    } else {
        log(Info) << "LogLevel unaltered by test-runner." << endlog();
    }

    return 0;
}

