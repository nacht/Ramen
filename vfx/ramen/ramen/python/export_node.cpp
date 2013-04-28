// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#include<boost/python.hpp>
#include<boost/shared_ptr.hpp>
using namespace boost::python;

#include<ramen/nodes/node.hpp>
using namespace ramen;

boost::shared_ptr<param_t> get_param( node_t *n, const std::string& id)
{
    try
    {
	param_t& p( n->param( id));
	return boost::shared_ptr<param_t>( &p);
    }
    catch( std::runtime_error& e)
    {
	const char message[] = "can't find param in node";
	PyErr_SetString( PyExc_ValueError, message);
	throw boost::python::error_already_set();
	return boost::shared_ptr<param_t>();
    }
}

void export_node()
{
    class_<node_t, boost::noncopyable, boost::shared_ptr<node_t> >( "node_t", no_init)
	    .def( "name", &node_t::name, return_value_policy<copy_const_reference>())

//	    .def( "param", get_param)
	    ;
}
