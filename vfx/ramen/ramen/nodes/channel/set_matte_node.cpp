// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/channel/set_matte_node.hpp>

#include<ramen/params/bool_param.hpp>
#include<ramen/image/processing.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<iostream>

namespace ramen
{

namespace
{

struct copy_rgb_and_clear_alpha
{
    image::pixel_t operator()( const image::pixel_t& src) const
    {
	image::pixel_t dst( src);
	dst[3] = 0;
	return dst;
    }
};

} // detail

const char *set_matte_node_t::help_string() const
{
    return "Replace the alpha channel of the first input by the alpha channel of the second input.";
}

set_matte_node_t::set_matte_node_t() : image_node_t()
{
    set_name( "set_matte");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["matte plug"], "Matte"));
    add_output_plug();
}

void set_matte_node_t::do_create_params()
{
    std::auto_ptr<bool_param_t> q( new bool_param_t( "Premultiply"));
    q->set_id( "premultiply");
    q->set_default_value( true);
    add_param( q);
}

void set_matte_node_t::do_calc_bounds( const render::render_context_t& context)
{
    if( get_value<bool>( param( "premultiply")))
	set_bounds( intersect( input(0)->bounds(), input(1)->bounds()));
    else
	set_bounds( input(0)->bounds());
}

void set_matte_node_t::do_calc_defined( const render::render_context_t& context)
{
    if( get_value<bool>( param( "premultiply")))
	set_defined( intersect( input(0)->defined(), intersect( input(1)->defined(), interest())));
    else
	set_defined( input(0)->defined());
}

void set_matte_node_t::do_process( const render::render_context_t& context)
{
    if( defined().isEmpty())
	return;

    boost::gil::tbb_transform_pixels( input(0)->const_subimage_view( defined()), image_view(),
					  copy_rgb_and_clear_alpha());

    Imath::Box2i area = intersect( defined(), input(1)->defined());

    if( !area.isEmpty())
	boost::gil::tbb_copy_pixels( boost::gil::nth_channel_view( input(1)->const_subimage_view( area), 3),
					    boost::gil::nth_channel_view( subimage_view( area), 3));

    if( get_value<bool>( param( "premultiply")))
	image::premultiply( image_view(), image_view());
}

void set_matte_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void set_matte_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

// factory
image_node_t *create_set_matte_node() { return new set_matte_node_t();}

node_info_t set_matte_node_info()
{
	node_info_t info;
	info.id = "builtin.set_matte";
	info.major_version = 1;
	info.minor_version = 0;
	info.submenu = "Channel";
	info.menu_item = "Set Matte";
	info.create = &create_set_matte_node;
	return info;
}

static bool registered = node_factory_t::Instance().register_node( set_matte_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::set_matte_node_t)
