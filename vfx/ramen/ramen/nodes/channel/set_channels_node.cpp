// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/channel/set_channels_node.hpp>

#include<boost/assign/std/vector.hpp>
using namespace boost::assign;

#include<adobe/algorithm/clamp.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/params/popup_param.hpp>

namespace ramen
{

namespace
{

enum
{
    copy_red = 0,
    copy_green,
    copy_blue,
    copy_alpha,
    set_one,
    set_zero
};

struct clamp_alpha_channel
{
    image::pixel_t operator()( const image::pixel_t& p) const
    {
	float a = adobe::clamp( (float) boost::gil::get_color( p, boost::gil::alpha_t()), 0.0f, 1.0f);
	image::pixel_t result( p);
	boost::gil::get_color( result, boost::gil::alpha_t()) = a;
	return result;
    }
};

} // unnamed

const char *set_channels_node_t::help_string() const
{
    return "Reorders the channels of the input image.";
}

set_channels_node_t::set_channels_node_t() : pointop_node_t() { set_name( "set ch");}

void set_channels_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Red"));
    p->set_id( "red");
    p->set_default_value( (int) copy_red);
    p->menu_items() += "Red", "Green", "Blue", "Alpha", "One", "Zero";
    add_param( p);

    p.reset( new popup_param_t( "Green"));
    p->set_id( "green");
    p->set_default_value( (int) copy_green);
    p->menu_items() += "Red", "Green", "Blue", "Alpha", "One", "Zero";
    add_param( p);

    p.reset( new popup_param_t( "Blue"));
    p->set_id( "blue");
    p->set_default_value( (int) copy_blue);
    p->menu_items() += "Red", "Green", "Blue", "Alpha", "One", "Zero";
    add_param( p);

    p.reset( new popup_param_t( "Alpha"));
    p->set_id( "alpha");
    p->set_default_value( (int) copy_alpha);
    p->menu_items() += "Red", "Green", "Blue", "Alpha", "One", "Zero";
    add_param( p);
}

void set_channels_node_t::do_calc_bounds( const render::render_context_t& context)
{
    int ch_a = get_value<int>( param( "alpha"));

    if( ch_a == set_one)
	set_bounds( domain());
    else
        set_bounds( input()->bounds());
}

void set_channels_node_t::do_calc_defined( const render::render_context_t& context)
{
    int ch_a = get_value<int>( param( "alpha"));

    if( ch_a == set_one)
	set_defined( intersect( domain(), interest()));
    else
        set_defined( input()->defined());
}

void set_channels_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
        return;

    copy_channel( input()->const_subimage_view( area), get_value<int>( param( "red"))  , subimage_view( area), 0);
    copy_channel( input()->const_subimage_view( area), get_value<int>( param( "green")), subimage_view( area), 1);
    copy_channel( input()->const_subimage_view( area), get_value<int>( param( "blue")) , subimage_view( area), 2);

    int alpha_op = get_value<int>( param( "alpha"));

    // the alpha channel is special if we fill with 0 or 1.
    if( alpha_op == set_one || alpha_op == set_zero)
	copy_channel( src, get_value<int>( param( "alpha")), image_view(), 3);
    else
    {
	copy_channel( src, get_value<int>( param( "alpha")) , dst, 3);
	boost::gil::tbb_transform_pixels( dst, dst, clamp_alpha_channel());
    }
}

void set_channels_node_t::copy_channel( const image::const_image_view_t& src, int src_ch, const image::image_view_t& dst , int dst_ch)
{
    if( src_ch == set_zero)
	boost::gil::fill_pixels( boost::gil::nth_channel_view( dst, dst_ch), boost::gil::gray32f_pixel_t( 0.0f));
    else
    {
	if( src_ch == set_one)
	   boost::gil::fill_pixels( boost::gil::nth_channel_view( dst, dst_ch), boost::gil::gray32f_pixel_t( 1.0f));
	else
	    boost::gil::copy_pixels( boost::gil::nth_channel_view( src, src_ch), boost::gil::nth_channel_view( dst, dst_ch));
    }
}

void set_channels_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void set_channels_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_set_channels_node() { return new set_channels_node_t();}

node_info_t set_channels_node_info()
{
    node_info_t info;
    info.id = "builtin.set_channels";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Channel";
    info.menu_item = "Set Channels";
    info.create = &create_set_channels_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( set_channels_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::set_channels_node_t)
