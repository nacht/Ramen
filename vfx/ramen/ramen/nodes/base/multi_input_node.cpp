// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/multi_input_node.hpp>

#include<boost/foreach.hpp>

#include<ramen/app/composition.hpp>

namespace ramen
{

multi_input_node_t::multi_input_node_t() : node_t() {}

void multi_input_node_t::reconnect_node()
{
    composition_t *comp = composition();

    if( comp)
    {
	BOOST_FOREACH( composition_t::edge_type& e, comp->edges())
	{
	    if( e.dst == this)
		input_plugs()[e.port].set_input( e.src);
	}
    }
}

void multi_input_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( node_t);
}

void multi_input_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::multi_input_node_t)
