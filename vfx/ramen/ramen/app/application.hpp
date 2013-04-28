// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_APP_HPP
#define RAMEN_APP_HPP

#include<ramen/app/application_fwd.hpp>

#include<string>

#include<boost/noncopyable.hpp>
#include<boost/cstdint.hpp>
#include<boost/program_options.hpp>

#include<loki/Singleton.h>

#include<tbb/task_scheduler_init.h>

#include<ramen/config.hpp>
#include<ramen/ofx/plugin_manager_fwd.hpp>

namespace ramen
{

class application_impl : boost::noncopyable
{
public:

    void init( int argc, char **argv);
    int run();

    bool run_command_line() const   { return run_command_line_;}
    int max_threads() const	    { return max_threads_;}
    
    ofx::plugin_manager_t& ofx_plugin_manager() { return *ofx_plugin_manager_;}

    // messages
    void fatal_error( const std::string& message);
    void error( const std::string& message);
    void inform( const std::string& message);
    bool question( const std::string& what, bool default_answer = true);

private:

    application_impl();

    void init_gui();
    void init_command_line();

    void print_app_info() const;

    void parse_command_line( int argc, char **argv);

    int render_composition();

    void usage();

    friend struct Loki::CreateUsingNew<application_impl>;
	
    tbb::task_scheduler_init task_scheduler_;

    boost::program_options::options_description desc_;
    boost::program_options::variables_map vm_;
    boost::program_options::positional_options_description p_;

    bool run_command_line_;
    int max_threads_;
    boost::uint64_t img_cache_size_;

    ofx::plugin_manager_t *ofx_plugin_manager_;
};

typedef Loki::SingletonHolder<application_impl> application_t;

} // namespace

#endif
