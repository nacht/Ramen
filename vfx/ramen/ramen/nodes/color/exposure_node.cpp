// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/exposure_node.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/image/processing.hpp>

namespace ramen
{

namespace
{

struct exposure_fun
{
    exposure_fun( float e) : m_( std::pow( 2, e)) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	return image::pixel_t( src[0] * m_, src[1] * m_, src[2] * m_, src[3]);
    }

    float m_;
};

}

exposure_node_t::exposure_node_t() : pointop_node_t()
{
    set_name("exposure");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["matte plug"], "Mask"), true);
}

void exposure_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Exp"));
    p->set_id( "exp");
    p->set_default_value( 0.0f);
    p->set_step( 0.1);
    add_param( p);
}

void exposure_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    image::const_image_view_t src_view;
    image::image_view_t dst_view;
    Imath::Box2i area;

    if( input(1))
    {
	boost::gil::copy_pixels( src, dst);
	area = intersect( input(1)->defined(), defined());

	if( area.isEmpty())
	    return;

	src_view = input(0)->const_subimage_view( area);
	dst_view = subimage_view( area);
    }
    else
    {
	src_view = src;
	dst_view = dst;
    }

    boost::gil::tbb_transform_pixels( src_view, dst_view, exposure_fun( get_value<float>( param( "exp"))));

    if( input(1))
        image::key_mix( src_view, dst_view, boost::gil::nth_channel_view( input(1)->const_subimage_view( area), 3), dst_view);
}

void exposure_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void exposure_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_exposure_node() { return new exposure_node_t();}

node_info_t exposure_node_info()
{
    node_info_t info;
    info.id = "builtin.exposure";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Exposure";
    info.create = &create_exposure_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( exposure_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::exposure_node_t)
