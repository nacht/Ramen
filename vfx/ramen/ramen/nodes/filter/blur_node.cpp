// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/filter/blur_node.hpp>

#include<gil/extension/border_algorithm.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur.hpp>
#include<ramen/image/box_blur_channel.hpp>

namespace ramen
{

namespace
{

enum
{
    border_black   = 0,
    border_repeat  = 1,
    border_reflect = 2
};

enum
{
    channels_rgba  = 0,
    channels_rgb   = 1,
    channels_alpha = 2
};

} // unnamed

blur_node_t::blur_node_t() : areaop_node_t() { set_name( "blur");}

void blur_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> r( new popup_param_t( "Channels"));
    r->set_id( "channels");
    r->menu_items() += "RGBA", "RGB", "Alpha";
    add_param( r);

    std::auto_ptr<float2_param_t> q( new float2_param_t( "Radius"));
    q->set_id( "radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    std::auto_ptr<float_param_t> p( new float_param_t( "Iters"));
    p->set_id( "iters");
    p->set_range( 1, 10);
    p->set_default_value( 1);
    p->set_static( true);
    p->set_round_to_int( true);
    add_param( p);

    std::auto_ptr<popup_param_t> b( new popup_param_t( "Border Mode"));
    b->set_id( "border");
    b->menu_items() += "Black", "Repeat", "Reflect";
    add_param( b);
}

void blur_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    Imath::V2f radius = get_value<Imath::V2f>( param( "radius"));
    int iters = get_value<int>( param( "iters"));

    hradius = std::ceil( radius.x + 1) * iters;
    vradius = std::ceil( radius.y + 1) * iters;
}

bool blur_node_t::expand_defined() const { return true;}

void blur_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));

    int bmode = get_value<int>( param( "border"));

    if( bmode != border_black)
    {
        int border_x0 = area.min.x - defined().min.x;
        int border_y0 = area.min.y - defined().min.y;

        int border_x1 = defined().max.x - area.max.x;
        int border_y1 = defined().max.y - area.max.y;

        if( bmode == border_repeat)
            boost::gil::repeat_border_pixels( image_view(), border_x0, border_y0, border_x1, border_y1);
        else
            boost::gil::reflect_border_pixels( image_view(), border_x0, border_y0, border_x1, border_y1);
    }

    int channels = get_value<int>( param( "channels"));
    Imath::V2f radius = get_value<Imath::V2f>( param( "radius"));
    int iters = get_value<int>( param( "iters"));

    radius.x  /= context.subsample;
    radius.y /= context.subsample;

    switch( channels)
    {
    case channels_rgba:
	image::box_blur_rgba( const_image_view(), image_view(), radius.x, radius.y, iters);
    break;

    case channels_rgb:
    {
	image::box_blur_rgba( const_image_view(), image_view(), radius.x, radius.y, iters);
        boost::gil::fill_pixels( boost::gil::nth_channel_view( image_view(), 3), boost::gil::gray32f_pixel_t( 0));
	boost::gil::copy_pixels( boost::gil::nth_channel_view( input()->const_subimage_view( area), 3),
				boost::gil::nth_channel_view( subimage_view( area), 3));
    }
    break;

    case channels_alpha:
    {
	image::box_blur_channel( boost::gil::nth_channel_view( const_image_view(), 3),
				boost::gil::nth_channel_view( image_view(), 3),
				radius.x, radius.y, iters);
    }
    break;
    }
}

void blur_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

void blur_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

// factory
node_t *create_blur_node() { return new blur_node_t();}

node_info_t blur_node_info()
{
    node_info_t info;
    info.id = "builtin.blur";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Filter";
    info.menu_item = "Blur";
    info.create = &create_blur_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( blur_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::blur_node_t)
