// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<boost/python.hpp>
using namespace boost::python;

#include<ramen/ui/user_interface.hpp>
using namespace ramen::ui;

user_interface_impl& interface() { return user_interface_t::Instance();}

void export_ui()
{
    class_<user_interface_impl, boost::noncopyable>( "user_interface_impl", no_init)
	.add_property( "start_frame", &user_interface_impl::start_frame	, &user_interface_impl::set_start_frame)
	.add_property( "end_frame"  , &user_interface_impl::end_frame	, &user_interface_impl::set_end_frame)
	.add_property( "frame"	    , &user_interface_impl::frame	, &user_interface_impl::set_frame)

	.def( "update", &user_interface_impl::update)
	;

    def( "user_interface", &interface, return_value_policy<reference_existing_object>());
}
