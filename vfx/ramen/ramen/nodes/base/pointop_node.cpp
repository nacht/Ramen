// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/pointop_node.hpp>

namespace ramen
{

pointop_node_t::pointop_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

pointop_node_t::pointop_node_t( const pointop_node_t& other) : image_node_t( other) {}

void pointop_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i def( input()->defined());
    set_defined( intersect( def, interest()));
}

void pointop_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    do_process( input()->const_subimage_view( area), subimage_view( area), context);
}

void pointop_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void pointop_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::pointop_node_t)
