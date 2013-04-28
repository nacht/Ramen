// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/input/color_node.hpp>

#include<ramen/params/color_param.hpp>

namespace ramen
{

color_node_t::color_node_t() : generator_node_t() { set_name( "color");}

void color_node_t::do_create_params()
{
    generator_node_t::do_create_params();

    std::auto_ptr<color_param_t> c( new color_param_t( "Color"));
    c->set_id( "color");
    c->set_default_value( Imath::Color4f( 0, 0, 0, 0));
    add_param( c);
}

void color_node_t::do_process( const render::render_context_t& context)
{
    Imath::Color4f c( get_value<Imath::Color4f>( param( "color")));

    image::pixel_t p;
    boost::gil::get_color( p, boost::gil::red_t())   = c.r;
    boost::gil::get_color( p, boost::gil::green_t()) = c.g;
    boost::gil::get_color( p, boost::gil::blue_t())  = c.b;
    boost::gil::get_color( p, boost::gil::alpha_t()) = c.a;
    boost::gil::fill_pixels( image_view(), p);
}

void color_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

void color_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( generator_node_t);
}

// factory
node_t *create_color_node() { return new color_node_t();}

node_info_t color_node_info()
{
    node_info_t info;
    info.id = "builtin.color";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Input";
    info.menu_item = "Color";
    info.create = &create_color_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( color_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::color_node_t)
