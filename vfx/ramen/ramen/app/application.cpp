// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/application.hpp>

#include<stdlib.h>

#include<exception>
#include<algorithm>
#include<iostream>
#include<fstream>

#include<boost/thread.hpp>
#include<boost/filesystem/path.hpp>
#include<boost/filesystem/operations.hpp>
namespace po = boost::program_options;

#include<OpenEXR/ImfThreading.h>

#include<ramen/system/system.hpp>

#include<ramen/app/preferences.hpp>
#include<ramen/app/document.hpp>

#include<ramen/ui/user_interface.hpp>
#include<ramen/ui/main_window.hpp>
#include<ramen/ui/splash_screen.hpp>

#include<ramen/render/render_sequence.hpp>
#include<ramen/render/renderer.hpp>

#include<ramen/memory/image_memory_manager.hpp>

#include<ramen/nodes/node_factory.hpp>

#include<ramen/image/init_image_processing.hpp>

#include<ramen/ofx/plugin_manager.hpp>

#include<ramen/python/interpreter.hpp>

namespace ramen
{

application_impl::application_impl() : desc_( "Allowed Options")
{
    max_threads_ = 0;
    run_command_line_ = false;
    img_cache_size_ = 0;
}

void application_impl::init( int argc, char **argv)
{
    parse_command_line( argc, argv);

    system::system_t::Instance();

    if( max_threads_ == 0)
	max_threads_ = boost::thread::hardware_concurrency();

    task_scheduler_.initialize( max_threads_);
    Imf::setGlobalThreadCount( max_threads_);

    if( !run_command_line_)
	init_gui();
    else
	init_command_line();
}

void application_impl::init_gui()
{
    ui::splash_screen_t::Instance().show();
    ui::splash_screen_t::Instance().show_message( RAMEN_NAME_VERSION_STR);

    ui::splash_screen_t::Instance().show_message( "Loading preferences");
    preferences_t::Instance();

    ui::splash_screen_t::Instance().show_message( "Initializing image cache");

    if( img_cache_size_ == 0)
    {
	boost::uint64_t percent  = preferences_t::Instance().max_image_memory();
	boost::uint64_t ram_size = system::system_t::Instance().ram_size();
	img_cache_size_ = ram_size / (boost::uint64_t) 100 * percent;
    }

    image_memory_manager_t::Instance().init( img_cache_size_);

    print_app_info();

    ui::splash_screen_t::Instance().show_message( "Initializing image processing");
    image::init_image_processing();

    ui::splash_screen_t::Instance().show_message( "Initializing builtin nodes");
    node_factory_t::Instance();

    ui::splash_screen_t::Instance().show_message( "Loading OFX plugins");
    ofx_plugin_manager_ = new ofx::plugin_manager_t();

    ui::splash_screen_t::Instance().show_message( "Initializing Python");
    python::interpreter_t::Instance();

    ui::splash_screen_t::Instance().show_message( "Initializing user interface");
    ui::user_interface_t::Instance().init();

    // open document if needed
    if( vm_.count( "input"))
    {
        boost::filesystem::path p( vm_["input"].as<std::string>());
        p = boost::filesystem::system_complete( p);
	ui::user_interface_t::Instance().open_document( p);
    }
}

void application_impl::init_command_line()
{
    preferences_t::Instance();

    // TODO: fix this...
    if( img_cache_size_ == 0)
	img_cache_size_ = 256 * 1024 * 1024; // 256 Mbytes?

    image_memory_manager_t::Instance().init( img_cache_size_);

    print_app_info();

    image::init_image_processing();

    node_factory_t::Instance();

    ofx_plugin_manager_ = new ofx::plugin_manager_t();

    python::interpreter_t::Instance();
}

int application_impl::run()
{
    if( !run_command_line_)
    {
	ui::user_interface_t::Instance().show();
	ui::splash_screen_t::Instance().finish( ui::user_interface_t::Instance().main_window());
	return ui::user_interface_t::Instance().run();
    }
    else
    {
	if( vm_.count( "render"))
	    return render_composition();
    }

    return 0;
}

void application_impl::print_app_info() const
{
    std::cout << RAMEN_NAME_VERSION_STR << ", " << __DATE__ << std::endl;
    std::cout << "System = " << system::system_t::Instance().system_name() << std::endl;

    switch( system::system_t::Instance().cpu_type())
    {
    case system::cpu_i386:
    {
	std::cout << "CPU = Intel compatible\n";

	int sset = system::system_t::Instance().simd_type();

	if( sset == system::simd_none)
	    std::cout << "SIMD = None\n";
	else
	{
	    std::cout << "SIMD = SSE ";
	    
	    if( sset & system::simd_sse2) std::cout << "SSE2 ";
	    if( sset & system::simd_sse3) std::cout << "SSE3";
	    std::cout << std::endl;
	}
    }
    break;

    case system::cpu_powerpc:
    {
	std::cout << "CPU = PowerPC\n";

	if( system::system_t::Instance().simd_type() & system::simd_altivec)
	    std::cout << "SIMD = Altivec\n";
	else
	    std::cout << "SIMD = None\n";
    }
    break;

    default:
	std::cout << "CPU = Unknown\n";
    }

    std::cout << "Using " << max_threads_ << " threads\n";
    std::cout << "Ram Size = " << system::system_t::Instance().ram_size() / 1024 / 1024 << " Mb\n";
    std::cout << "Image Memory = " << image_memory_manager_t::Instance().max_size() / 1024 / 1024 << " Mb\n";

    #ifdef RAMEN_SSE_SUPPORT
	std::cout << "SSE support\n";
    #endif

    std::cout << std::endl;
}

void application_impl::parse_command_line( int argc, char **argv)
{
    p_.add( "input" , 1);
    desc_.add_options()	    ("help,h", "produce help message")
			    // files
			    ("input", po::value<std::string>(), "input file")

			    ("render,r" , "render")
			    ("start,s" , po::value<int>(), "start frame")
			    ("end,e"   , po::value<int>(), "end frame")
			    ; // end

    po::store( po::command_line_parser( argc, argv).options( desc_).positional( p_).run(), vm_);
    po::notify( vm_);

    if( vm_.count( "help"))
	usage();

    if( vm_.count( "render"))
    {
	std::cout << "render flag\n";

	if( !vm_.count( "input"))
	    usage();

	run_command_line_ = true;
    }
}

void application_impl::usage()
{
    std::cout << "Usage:\n";
    std::cout << "------\n";
    std::cout << "open: ramen input_file\n";
    std::cout << "render: ramen --render --s 1 --e 100 input_file\n";
    std::cout << std::endl;
    exit( 0);
}

bool render_progress( int x)
{
    std::cout << "*";
    std::flush( std::cout);
    return false;
}

int application_impl::render_composition()
{
    boost::filesystem::path p( vm_["input"].as<std::string>());
    document_t::Instance().load( p);

    int start = document_t::Instance().composition().start_frame();
    int end = document_t::Instance().composition().end_frame();
    int subsample = 1;

    if( vm_.count( "start"))
	start = vm_["start"].as<int>();

    if( vm_.count( "end"))
	end = vm_["end"].as<int>();

    // TODO: add motion blur options here, too
    render::render_sequence( document_t::Instance().composition(), start, end, subsample, 0, 1, false, render_progress);
    return 0;
}

void application_impl::fatal_error( const std::string& message)
{
    if( !run_command_line_)
	ui::user_interface_t::Instance().fatal_error( message);
    else
	std::cout << "Ramen, fatal error: " << message << "\n";

    abort();
}

void application_impl::error( const std::string& message)
{
    if( !run_command_line_)
	ui::user_interface_t::Instance().error( message);
    else
	std::cout << "Ramen, error: " << message << "\n";
}

void application_impl::inform( const std::string& message)
{
    if( !run_command_line_)
	ui::user_interface_t::Instance().inform( message);
    else
	std::cout << "Ramen, inform: " << message << "\n";
}

bool application_impl::question( const std::string& what, bool default_answer)
{
    if( !run_command_line_)
	return ui::user_interface_t::Instance().question( what, default_answer);
    else
    {
	if( default_answer)
	    std::cout << "Ramen, question: " << what << ", replying yes by default\n";
	else
	    std::cout << "Ramen, question: " << what << ", replying no by default\n";

	return default_answer;
    }
}

} // namespace
