// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/full_frame_node.hpp>

namespace ramen
{

full_frame_node_t::full_frame_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

full_frame_node_t::full_frame_node_t( const full_frame_node_t& other) : image_node_t( other) {}

void full_frame_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    for( int i = 0; i < num_inputs(); ++i)
    {
	if( input( i))
	    input( i)->add_interest( input( i)->domain()); // Maybe, it'd be better to use the bounds instead of the domain...
    }
}

void full_frame_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( input()->defined());
}

void full_frame_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void full_frame_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::full_frame_node_t)
