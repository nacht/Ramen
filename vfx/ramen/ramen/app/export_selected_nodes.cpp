// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/export_selected_nodes.hpp>

#include<map>

#include<boost/foreach.hpp>

#include<ramen/app/document.hpp>
#include<ramen/serialization/archive.hpp>

namespace ramen
{

void export_selected_nodes( const boost::filesystem::path& p)
{
    composition_t comp;

    std::map<node_t*, node_t*> relation;

    BOOST_FOREACH( node_t& n, document_t::Instance().composition().nodes())
    {
	if( n.selected())
	{
	    node_t *nclone = new_clone( n);
	    relation[&n] = nclone;
	    comp.add_node( std::auto_ptr<node_t>( nclone));
	}
    }

    BOOST_FOREACH( composition_t::edge_type& e, document_t::Instance().composition().edges())
    {
	if( e.src->selected() && e.dst->selected())
	    comp.add_edge( composition_t::edge_type( relation[e.src], relation[e.dst], e.port));
    }

    ofstream_t ofs( p);
    output_archive_t oa( ofs);
    oa & boost::serialization::make_nvp( "composition", comp);

    // save any extra data needed to make it a document
}

} // namespace
