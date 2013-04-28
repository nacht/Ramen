// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/areaop_node.hpp>

#include<cmath>

namespace ramen
{

areaop_node_t::areaop_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

areaop_node_t::areaop_node_t( const areaop_node_t& other) : image_node_t( other) {}

bool areaop_node_t::expand_defined() const { return false;}

void areaop_node_t::do_calc_bounds( const render::render_context_t& context)
{
    get_expand_radius( hradius_, vradius_);
    Imath::Box2i bounds( input()->bounds());

    if( expand_defined())
    {
	bounds.min.x -= hradius_; bounds.max.x += hradius_;
	bounds.min.y -= vradius_; bounds.max.y += vradius_;
    }

    set_bounds( bounds);
}

void areaop_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    Imath::Box2i roi = interest();
    roi.min.x -= hradius_; roi.min.y -= vradius_;
    roi.max.x += hradius_; roi.max.y += vradius_;
    input()->add_interest( roi);
}

void areaop_node_t::do_calc_defined( const render::render_context_t& context)
{
    if( expand_defined())
    {
	Imath::Box2i def( intersect( input()->defined(), interest()));

	def.min.x -= hradius_; def.max.x += hradius_;
	def.min.y -= vradius_; def.max.y += vradius_;

	set_defined( def);
    }
    else
    {
	Imath::Box2i def( input()->defined());
	set_defined( intersect( def, interest()));
    }
}

void areaop_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void areaop_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::areaop_node_t)
