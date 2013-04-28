// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/base_keyer_node.hpp>

namespace ramen
{

base_keyer_node_t::base_keyer_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

base_keyer_node_t::base_keyer_node_t( const base_keyer_node_t& other) : image_node_t( other) {}


void base_keyer_node_t::do_process( const render::render_context_t& context)
{
}

void base_keyer_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void base_keyer_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::base_keyer_node_t)
