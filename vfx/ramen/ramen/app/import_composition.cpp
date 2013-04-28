// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/app/import_composition.hpp>

#include<boost/foreach.hpp>

#include<ramen/app/document.hpp>
#include<ramen/serialization/archive.hpp>

namespace ramen
{

void import_composition( const boost::filesystem::path& p)
{
    composition_t comp;

    ifstream_t ifs( p);
    input_archive_t ia( ifs);
    ia & boost::serialization::make_nvp( "composition", comp);

    // copy edges
    std::vector<composition_t::edge_type> edges( comp.edges());

    while( !comp.edges().empty())
	comp.remove_edge( comp.edges()[0]);

    // merge
    while( !comp.nodes().empty())
    {
	std::auto_ptr<node_t> n( comp.release_node( &( comp.nodes()[0])));
	document_t::Instance().add_node( n);
    }

    BOOST_FOREACH( composition_t::edge_type& e, edges)
    {
	document_t::Instance().add_edge( e);
    }
}

} // namespace
