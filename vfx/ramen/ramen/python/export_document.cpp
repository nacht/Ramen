// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<boost/python.hpp>
using namespace boost::python;

#include<ramen/app/document.hpp>
using namespace ramen;

document_impl& current_document() { return document_t::Instance();}

void export_document()
{
    class_<document_impl, boost::noncopyable>( "document_impl", no_init)
	.def( "composition", (composition_t& (document_impl::*)()) &document_impl::composition, return_internal_reference<>());

    def( "document", &current_document, return_value_policy<reference_existing_object>());
}
