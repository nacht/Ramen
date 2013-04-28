// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/channel/invert_node.hpp>

#include<ramen/params/popup_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{

const char *invert_node_t::help_string() const
{
    return "Inverts the RGB or alpha channel of the input";
}

invert_node_t::invert_node_t() : pointop_node_t()
{
    set_name( "invert");
}

void invert_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Channels"));
    p->set_id( "channels");
    p->add_menu_item( "RGB");
    p->add_menu_item( "Alpha");
    add_param( p);
}

void invert_node_t::do_calc_defined( const render::render_context_t& context)
{
    int alpha = get_value<int>( param( "channels"));

    if( alpha)
	set_defined( domain());
    else
	set_defined( input()->defined());
}

void invert_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    if( get_value<int>( param( "channels")))
	boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 1));

    do_process( input()->const_subimage_view( area), subimage_view( area), context);
}

void invert_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    int alpha = get_value<int>( param( "channels"));

    if( alpha)
	image::invert_alpha( src, dst);
    else
	image::invert_rgb( src, dst);
}

void invert_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void invert_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_invert_node() { return new invert_node_t();}

node_info_t invert_node_info()
{
    node_info_t info;
    info.id = "builtin.invert";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Channel";
    info.menu_item = "Invert";
    info.create = &create_invert_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( invert_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::invert_node_t)
