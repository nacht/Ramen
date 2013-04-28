// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/document.hpp>

#include<boost/bind.hpp>

#include<adobe/algorithm/for_each.hpp>
#include<adobe/algorithm/find.hpp>
#include<adobe/algorithm/count.hpp>

#include<ramen/serialization/archive.hpp>

#include<ramen/app/application.hpp>

namespace ramen
{

document_impl::document_impl() : dirty_( false) {}

document_impl::~document_impl()
{
    for( composition_t::node_iterator it( composition().nodes().begin()); it != composition().nodes().end(); ++it)
	released_( &(*it));
}

void document_impl::add_node( std::auto_ptr<composition_t::node_type> n) { composition().add_node( n);}

std::auto_ptr<composition_t::node_type> document_impl::release_node( composition_t::node_type *n)
{
    released_( n);
    return composition().release_node( n);
}

void document_impl::set_file( const boost::filesystem::path& p)
{
    file_ = p;
    composition().set_composition_dir( p.parent_path());
}

void document_impl::load( const boost::filesystem::path& p)
{
    set_file( p);
    ifstream_t ifs( file_);
    input_archive_t ia( ifs);
    ia & boost::serialization::make_nvp( "composition", comp_);
    dirty_ = false;
}

void document_impl::save()
{
    ofstream_t ofs( file_);

    if( !ofs.is_open())
    {
	application_t::Instance().error( std::string( "Couldn't open output file ") + file_.external_file_string());
	return;
    }

    if( !ofs.good())
    {
	application_t::Instance().error( "Couldn't open output file, stream not good.");
	return;
    }

    output_archive_t oa( ofs);

    try
    {
	oa & boost::serialization::make_nvp( "composition", comp_);
    }
    catch( std::exception& e)
    {
	ofs.flush();
	application_t::Instance().error( std::string( "Couldn't save file, reason = ") + e.what());
        return;
    }

    dirty_ = false;
}

void document_impl::deselect_all() { adobe::for_each( composition().nodes(), boost::bind( &node_t::select, _1, false));}

bool document_impl::any_selected() const
{
    return adobe::find_if( composition().nodes(), boost::bind( &node_t::selected, _1)) != composition().nodes().end();
}

node_t *document_impl::selected_node()
{
    if( adobe::count_if( composition().nodes(), boost::bind( &node_t::selected, _1)) == 1)
    {
	composition_t::node_iterator it( composition().nodes().begin());

	for( ; it != composition().nodes().end(); ++it)
	{
            if( it->selected())
                return &(*it);
	}
    }

    return 0;
}

} // namespace
