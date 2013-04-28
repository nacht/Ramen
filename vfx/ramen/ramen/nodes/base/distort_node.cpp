// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#include<ramen/nodes/base/distort_node.hpp>

namespace ramen
{

distort_node_t::distort_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

distort_node_t::distort_node_t( const distort_node_t& other) : image_node_t( other) {}

void distort_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    hradius = vradius = 0;
}

void distort_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    if( needs_full_input_frame())
	input()->add_interest( input()->bounds());
    else
    {
	get_expand_radius( hradius_, vradius_);
	Imath::Box2i roi = interest();
	roi.min.x -= hradius_; roi.min.y -= vradius_;
	roi.max.x += hradius_; roi.max.y += vradius_;
	input()->add_interest( roi);
    }
}

void distort_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void distort_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::distort_node_t)
