// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/channel/copy_channels_node.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/params/popup_param.hpp>

namespace ramen
{

namespace
{

enum
{
    copy_source = 0,
    copy_red,
    copy_green,
    copy_blue,
    copy_alpha,
    set_one,
    set_zero
};

} // unnamed

const char *copy_channels_node_t::help_string() const
{
    return "Copy channels from the second input to the first";
}

copy_channels_node_t::copy_channels_node_t() : image_node_t()
{
    set_name( "copy ch");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Source"));
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Source 2"));
    add_output_plug();
}

void copy_channels_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> p( new popup_param_t( "Red"));
    p->set_id( "red");
    p->menu_items() += "Source", "Src2 Red", "Src2 Green", "Src2 Blue", "Src2 Alpha", "One", "Zero";
    add_param( p);

    p.reset( new popup_param_t( "Green"));
    p->set_id( "green");
    p->menu_items() += "Source", "Src2 Red", "Src2 Green", "Src2 Blue", "Src2 Alpha", "One", "Zero";
    add_param( p);

    p.reset( new popup_param_t( "Blue"));
    p->set_id( "blue");
    p->menu_items() += "Source", "Src2 Red", "Src2 Green", "Src2 Blue", "Src2 Alpha", "One", "Zero";
    add_param( p);

    p.reset( new popup_param_t( "Alpha"));
    p->set_id( "alpha");
    p->menu_items() += "Source", "Src2 Red", "Src2 Green", "Src2 Blue", "Src2 Alpha", "One", "Zero";
    add_param( p);
}

void copy_channels_node_t::do_calc_bounds( const render::render_context_t& context)
{
    Imath::Box2i rod1 = input(0)->bounds();
    Imath::Box2i rod2 = input(1)->bounds();

    int ch_r = get_value<int>( param( "red"));
    int ch_g = get_value<int>( param( "green"));
    int ch_b = get_value<int>( param( "blue"));
    int ch_a = get_value<int>( param( "alpha"));

    // use alpha from input 1
    if( ch_a == copy_source)
    {
	set_bounds( rod1);
	return;
    }

    // alpha comes from input2
    if( ch_a != set_one && ch_a != set_zero)
    {
	set_bounds( rod2);
	return;
    }

    // alpha is zero or one, look at the other channels
    Imath::Box2i rod;

    if( ch_r == copy_source)
	rod = rod1;
    else
    {
	if( ch_r != set_zero && ch_r != set_one)
	    rod = rod2;
    }

    if( ch_g == copy_source)
	rod.extendBy( rod1);
    else
    {
	if( ch_g != set_zero && ch_r != set_one)
	    rod.extendBy( rod2);
    }

    if( ch_b == copy_source)
	rod.extendBy( rod1);
    else
    {
	if( ch_b != set_zero && ch_r != set_one)
	    rod.extendBy( rod2);
    }

    if( rod.isEmpty())
	rod = rod1;

    set_bounds( rod);
}

void copy_channels_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i def1 = input(0)->defined();
    Imath::Box2i def2 = input(1)->defined();

    int ch_r = get_value<int>( param( "red"));
    int ch_g = get_value<int>( param( "green"));
    int ch_b = get_value<int>( param( "blue"));
    int ch_a = get_value<int>( param( "alpha"));

    // use alpha from input 1
    if( ch_a == copy_source)
    {
	set_defined( def1);
	return;
    }

    // alpha comes from input2
    if( ch_a != set_one && ch_a != set_zero)
    {
	set_defined( def2);
	return;
    }

    // alpha is zero or one, look at the other channels
    Imath::Box2i def;

    if( ch_r == copy_source)
	def = def1;
    else
    {
	if( ch_r != set_zero && ch_r != set_one)
	    def = def2;
    }

    if( ch_g == copy_source)
	def.extendBy( def1);
    else
    {
	if( ch_g != set_zero && ch_r != set_one)
	    def.extendBy( def2);
    }

    if( ch_b == copy_source)
	def.extendBy( def1);
    else
    {
	if( ch_b != set_zero && ch_r != set_one)
	    def.extendBy( def2);
    }

    if( def.isEmpty())
	def = def1;

    set_defined( def);
}

void copy_channels_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i src1_area( intersect( input(0)->defined(), defined()));
    image::const_image_view_t src1( input(0)->const_subimage_view( src1_area));

    Imath::Box2i src2_area( intersect( input(1)->defined(), defined()));
    image::const_image_view_t src2( input(1)->const_subimage_view( src2_area));

    image::image_view_t dst1( subimage_view( src1_area));
    image::image_view_t dst2( subimage_view( src2_area));

    int ch;

    ch = get_value<int>( param( "red"));
    switch( ch)
    {
    case copy_source:
	copy_channel( src1, 0, dst1, 0);
    break;

    default:
	copy_channel( src2, ch-1, dst2, 0);
    }

    ch = get_value<int>( param( "green"));
    switch( ch)
    {
    case copy_source:
	copy_channel( src1, 1, dst1, 1);
    break;

    default:
	copy_channel( src2, ch-1, dst2, 1);
    }

    ch = get_value<int>( param( "blue"));
    switch( ch)
    {
    case copy_source:
	copy_channel( src1, 2, dst1, 2);
    break;

    default:
	copy_channel( src2, ch-1, dst2, 2);
    }

    ch = get_value<int>( param( "alpha"));
    switch( ch)
    {
    case copy_source:
	copy_channel( src1, 3, dst1, 3);
    break;

    case set_one:
    case set_zero:
    {
	Imath::Box2i area( intersect( defined(), domain()));
	copy_channel( src1, ch-1, subimage_view( area), 3);
    }
    break;

    default:
	copy_channel( src2, ch-1, dst2, 3);
    }
}

void copy_channels_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void copy_channels_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void copy_channels_node_t::copy_channel( const image::const_image_view_t& src, int src_ch, const image::image_view_t& dst , int dst_ch)
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

// factory
image_node_t *create_copy_channels_node() { return new copy_channels_node_t();}

node_info_t copy_channels_node_info()
{
	node_info_t info;
	info.id = "builtin.copy_channels";
	info.major_version = 1;
	info.minor_version = 0;
	info.submenu = "Channel";
	info.menu_item = "Copy Channels";
	info.create = &create_copy_channels_node;
	return info;
}

static bool registered = node_factory_t::Instance().register_node( copy_channels_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::copy_channels_node_t)
