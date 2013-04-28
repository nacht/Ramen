// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<boost/bind.hpp>
#include<boost/python.hpp>
using namespace boost::python;

#include<adobe/algorithm/find.hpp>

#include<ramen/app/composition.hpp>
using namespace ramen;

boost::shared_ptr<node_t> get_node( composition_t& c, const std::string& name)
{
    node_t *n = c.find_node( name);

    if( n != 0)
	return boost::shared_ptr<node_t>( n, node_deleter());

    const char message[] = "node not in composition";
    PyErr_SetString( PyExc_ValueError, message);
    throw boost::python::error_already_set();
    return boost::shared_ptr<node_t>();
}

void connect( composition_t& c, boost::shared_ptr<node_t> src, boost::shared_ptr<node_t> dst, int port)
{
    if( port >= dst->num_inputs())
    {
	const char message[] = "input number of out range";
	PyErr_SetString( PyExc_ValueError, message);
	throw boost::python::error_already_set();
    }

    c.connect( src.get(), dst.get(), port);
}

void disconnect( composition_t& c, boost::shared_ptr<node_t> src, boost::shared_ptr<node_t> dst, int port)
{
    if( port >= dst->num_inputs())
    {
	const char message[] = "input number of out range";
	PyErr_SetString( PyExc_ValueError, message);
	throw boost::python::error_already_set();
    }

    c.disconnect( src.get(), dst.get(), port);
}

void export_composition()
{
    class_<composition_t>( "composition_t")
	    .def( "get_node", get_node)
	    .def( "connect", connect)
	    .def( "disconnect", disconnect);
}
