// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<stdlib.h>

#include<iostream>
#include<exception>

#include<boost/version.hpp>
#include<boost/static_assert.hpp>

#include<ramen/app/QRamenApp.hpp>
#include<ramen/app/application.hpp>

// abort compilation if boost version < 1.42
BOOST_STATIC_ASSERT( BOOST_VERSION >= 104200);

void ramen_terminate( void)
{
    ramen::application_t::Instance().fatal_error( "Ramen has encountered a fatal error");
}

void ramen_unexpected( void)
{
    ramen::application_t::Instance().error( "Ramen has encountered an unexpected exception");
    throw std::runtime_error( "Ramen has encountered an unexpected exception.\nPlease report this bug." );
}

int main( int argc, char **argv)
{
    std::set_terminate( &ramen_terminate);
    std::set_unexpected( &ramen_unexpected);

    try
    {
	QApplication *app = new QRamenApp( argc, argv);
	ramen::application_t::Instance().init(argc, argv);
	int result = ramen::application_t::Instance().run();
	return result;
    }
    catch( std::exception& e)
    {
	std::cout << "Exception: " << e.what() << "\n";
    }
    catch( ...)
    {
	std::cout << "Unknown exception" << std::endl;
    }
}
