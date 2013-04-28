// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<boost/python.hpp>
using namespace boost::python;

#include<ramen/params/param.hpp>
using namespace ramen;

void export_param()
{
    class_<param_t, boost::noncopyable, boost::shared_ptr<param_t> >( "param_t", no_init)
	;
}
