// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/python/interpreter.hpp>

using namespace boost::python;

// prototypes
void export_composition();
void export_document();
void export_node();
void export_param();
void export_ui();

BOOST_PYTHON_MODULE( ramen)
{
    export_composition();
    export_document();
    export_node();
    export_param();
    export_ui();
}

namespace ramen
{
namespace python
{

interpreter_impl::interpreter_impl()
{
    try
    {
	PyImport_AppendInittab( (char *) "ramen", &initramen);
	Py_Initialize();

	object main_module( handle<>( borrowed( PyImport_AddModule("__main__"))));
	main_namespace_ = main_module.attr( "__dict__");

	object ramen_module( handle<>( PyImport_ImportModule( "ramen")));
	main_namespace_["ramen"] = ramen_module;
    }
    catch( boost::python::error_already_set)
    {
	PyErr_Print();
	PyErr_Clear();
    }
}

} // namespace
} // namespace
