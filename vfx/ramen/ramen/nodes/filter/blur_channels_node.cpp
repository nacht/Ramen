// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/filter/blur_channels_node.hpp>

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

blur_channels_node_t::blur_channels_node_t() : areaop_node_t() { set_name( "blur_ch");}

void blur_channels_node_t::do_create_params()
{
    std::auto_ptr<float2_param_t> q( new float2_param_t( "R Radius"));
    q->set_id( "r_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "G Radius"));
    q->set_id( "g_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "B Radius"));
    q->set_id( "b_radius");
    q->set_range( 0, 200);
    q->set_step( 0.25);
    q->set_default_value( Imath::V2f( 0, 0));
    q->set_proportional( true);
    add_param( q);

    q.reset( new float2_param_t( "A Radius"));
    q->set_id( "a_radius");
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

void blur_channels_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    Imath::V2f r_radius = get_value<Imath::V2f>( param( "r_radius"));
    Imath::V2f g_radius = get_value<Imath::V2f>( param( "g_radius"));
    Imath::V2f b_radius = get_value<Imath::V2f>( param( "b_radius"));
    Imath::V2f a_radius = get_value<Imath::V2f>( param( "a_radius"));

    Imath::V2f radius( std::max( r_radius.x, std::max( g_radius.x, std::max( b_radius.x, a_radius.x))),
                        std::max( r_radius.y, std::max( g_radius.y, std::max( b_radius.y, a_radius.y))));

    int iters = get_value<int>( param( "iters"));

    hradius = std::ceil( radius.x + 1) * iters;
    vradius = std::ceil( radius.y + 1) * iters;
}

bool blur_channels_node_t::expand_defined() const { return true;}

void blur_channels_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    int bmode = get_value<int>( param( "border"));

    Imath::V2f r_radius = get_value<Imath::V2f>( param( "r_radius")) / context.subsample;
    Imath::V2f g_radius = get_value<Imath::V2f>( param( "g_radius")) / context.subsample;
    Imath::V2f b_radius = get_value<Imath::V2f>( param( "b_radius")) / context.subsample;
    Imath::V2f a_radius = get_value<Imath::V2f>( param( "a_radius")) / context.subsample;

    int iters = get_value<int>( param( "iters"));

    boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));
    blur_channel( 0, area, r_radius, iters, bmode);
    blur_channel( 1, area, g_radius, iters, bmode);
    blur_channel( 2, area, b_radius, iters, bmode);
    blur_channel( 3, area, a_radius, iters, bmode);
}

void blur_channels_node_t::blur_channel( int ch, const Imath::Box2i& area, const Imath::V2f& radius, int iters, int border)
{
    if( radius.x == 0.0f && radius.y == 0.0f)
        return;

    if( border != border_black)
    {
        int hradius = std::ceil( radius.x + 1) * iters;
        int vradius = std::ceil( radius.y + 1) * iters;

        int border_x0 = std::min( area.min.x - defined().min.x, hradius);
        int border_y0 = std::min( area.min.y - defined().min.y, vradius);
        int border_x1 = std::min( defined().max.x - area.max.x, hradius);
        int border_y1 = std::min( defined().max.y - area.max.y, vradius);

        Imath::Box2i box( area);
        box.min.x -= border_x0; box.min.y -= border_y0;
        box.max.x += border_x1; box.max.y += border_y1;

        if( border == border_repeat)
          boost::gil::repeat_border_pixels( boost::gil::nth_channel_view( subimage_view( box), ch), border_x0, border_y0, border_x1, border_y1);
        else
          boost::gil::reflect_border_pixels( boost::gil::nth_channel_view( subimage_view( box), ch), border_x0, border_y0, border_x1, border_y1);
    }

    image::box_blur_channel( boost::gil::nth_channel_view( const_image_view(), ch), boost::gil::nth_channel_view( image_view(), ch), radius.x, radius.y, iters);
}

void blur_channels_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

void blur_channels_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

// factory
node_t *create_blur_channels_node() { return new blur_channels_node_t();}

node_info_t blur_channels_node_info()
{
    node_info_t info;
    info.id = "builtin.blur_channels";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Filter";
    info.menu_item = "Blur Channels";
    info.create = &create_blur_channels_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( blur_channels_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::blur_channels_node_t)
