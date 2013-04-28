// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
 
#include<ramen/nodes/input/color_bars_node.hpp>

#include<ramen/image/color_bars.hpp>

namespace ramen
{

color_bars_node_t::color_bars_node_t() : generator_node_t()
{
    set_name( "color bars");
}

void color_bars_node_t::do_create_params()
{
    generator_node_t::do_create_params();
}

void color_bars_node_t::do_process( const render::render_context_t& context)
{
    image::make_color_bars( image_view(), domain(), defined());
}

void color_bars_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

void color_bars_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

// factory
node_t *create_color_bars_node() { return new color_bars_node_t();}

node_info_t color_bars_node_info()
{
    node_info_t info;
    info.id = "builtin.color_bars";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Input";
    info.menu_item = "Color Bars";
    info.create = &create_color_bars_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( color_bars_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::color_bars_node_t)
