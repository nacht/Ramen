// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/color_correct_node.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform2.hpp>
#include<gil/extension/algorithm/tbb/tbb_transform3.hpp>

#include<ramen/nodes/color/color_correct.hpp>

#include<ramen/params/float3_param.hpp>
#include<ramen/params/float_param.hpp>
#include<ramen/params/tab_group_param.hpp>
#include<ramen/params/group_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{

color_correct_node_t::color_correct_node_t() : pointop_node_t()
{
    set_name( "correct");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["matte plug"], "Mask"), true);
}

void color_correct_node_t::do_create_params()
{
    std::auto_ptr<tab_group_param_t> p( new tab_group_param_t());
    p->set_id( "cc");
    p->add_param( create_param_tab( "Master"    , "master", ""));
    p->add_param( create_param_tab( "Shadows"   , "shadows", "sh_"));
    p->add_param( create_param_tab( "Midtones"  , "midtones", "mid_"));
    p->add_param( create_param_tab( "Highlights", "highlights", "high_"));
    add_param( p);
}

void color_correct_node_t::create_params_inside_param( composite_param_t *g, const std::string& id_prefix)
{
    std::auto_ptr<float3_param_t> p( new float3_param_t( "Gamma"));
    p->set_id( id_prefix + "gamma");
    p->set_range( 0.1, 20);
    p->set_step( 0.01);
    p->set_default_value( Imath::V3f( 1, 1, 1));
    p->set_proportional( true);
    g->add_param( p);

    p.reset( new float3_param_t( "Gain"));
    p->set_id( id_prefix + "gain");
    p->set_range( 0, 20);
    p->set_step( 0.01);
    p->set_default_value( Imath::V3f( 1, 1, 1));
    p->set_proportional( true);
    g->add_param( p);

    p.reset( new float3_param_t( "Offset"));
    p->set_id( id_prefix + "offset");
    p->set_range( -100, 100);
    p->set_step( 0.01);
    p->set_default_value( Imath::V3f( 0, 0, 0));
    p->set_proportional( true);
    g->add_param( p);

    std::auto_ptr<float_param_t> q( new float_param_t( "Saturation"));
    q->set_id( id_prefix + "saturation");
    q->set_range( 0, 10);
    q->set_step( 0.01);
    q->set_default_value( 1);
    g->add_param( q);

    q.reset( new float_param_t( "Contrast"));
    q->set_id( id_prefix + "contrast");
    q->set_range( 0, 10);
    q->set_step( 0.01);
    q->set_default_value( 1);
    g->add_param( q);
}

std::auto_ptr<composite_param_t> color_correct_node_t::create_param_tab( const std::string& name, const std::string& id, const std::string& id_prefix)
{
    std::auto_ptr<composite_param_t> g( new composite_param_t( name));
    g->set_id( id);
    create_params_inside_param( g.get(), id_prefix);
    return g;
}

void color_correct_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
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

	src_view = input(0)->const_subimage_view( area),
	dst_view = subimage_view( area);
    }
    else
    {
	src_view = src;
	dst_view = dst;
    }

    half_lut_t r_lut, g_lut, b_lut;

    Imath::V3f gamma  = get_value<Imath::V3f>( param( "gamma"));
    Imath::V3f gain   = get_value<Imath::V3f>( param( "gain"));
    Imath::V3f offset = get_value<Imath::V3f>( param( "offset"));

    color_correct_fun<> master_ccf( 1.0f/gamma.x, 1.0f/gamma.y, 1.0f/gamma.z,
			  gain.x, gain.y, gain.z,
			  offset.x, offset.y, offset.z,
			  get_value<float>( param( "saturation")),
			  get_value<float>( param( "contrast")),
			  r_lut, g_lut, b_lut);

    boost::gil::tbb_transform_pixels( src_view, src_view, dst_view, master_ccf);

    // shadows
    gamma  = get_value<Imath::V3f>( param( "sh_gamma"));
    gain   = get_value<Imath::V3f>( param( "sh_gain"));
    offset = get_value<Imath::V3f>( param( "sh_offset"));

    color_correct_fun<image::shadow_blend_fun> shadow_ccf( 1.0f/gamma.x, 1.0f/gamma.y, 1.0f/gamma.z,
						    gain.x, gain.y, gain.z,
						    offset.x, offset.y, offset.z,
						    get_value<float>( param( "sh_saturation")),
						    get_value<float>( param( "sh_contrast")),
						    r_lut, g_lut, b_lut);

    boost::gil::tbb_transform_pixels( src_view, dst_view, dst_view, shadow_ccf);

    // midtones
    gamma  = get_value<Imath::V3f>( param( "mid_gamma"));
    gain   = get_value<Imath::V3f>( param( "mid_gain"));
    offset = get_value<Imath::V3f>( param( "mid_offset"));

    color_correct_fun<image::midtones_blend_fun> midtones_ccf( 1.0f/gamma.x, 1.0f/gamma.y, 1.0f/gamma.z,
						    gain.x, gain.y, gain.z,
						    offset.x, offset.y, offset.z,
						    get_value<float>( param( "mid_saturation")),
						    get_value<float>( param( "mid_contrast")),
						    r_lut, g_lut, b_lut);

    boost::gil::tbb_transform_pixels( src_view, dst_view, dst_view, midtones_ccf);

    // highs
    gamma  = get_value<Imath::V3f>( param( "high_gamma"));
    gain   = get_value<Imath::V3f>( param( "high_gain"));
    offset = get_value<Imath::V3f>( param( "high_offset"));

    color_correct_fun<image::highlights_blend_fun> highlights_ccf( 1.0f/gamma.x, 1.0f/gamma.y, 1.0f/gamma.z,
						      gain.x, gain.y, gain.z,
						    offset.x, offset.y, offset.z,
						    get_value<float>( param( "high_saturation")),
						    get_value<float>( param( "high_contrast")),
						    r_lut, g_lut, b_lut);

    boost::gil::tbb_transform_pixels( src_view, dst_view, dst_view, highlights_ccf);

    // mask
    if( input(1))
        image::key_mix( src_view, dst_view, boost::gil::nth_channel_view( input(1)->const_subimage_view( area), 3), dst_view);
}

void color_correct_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void color_correct_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// -----------------------------------------------
// version 2 node

void color_correct2_node_t::do_create_params()
{
    std::auto_ptr<group_param_t> g( new group_param_t( "Master"));
    create_params_inside_param( g.get(), "");
    add_param( g);

    g.reset( new group_param_t( "Shadows"));
    create_params_inside_param( g.get(), "sh_");
    add_param( g);

    g.reset( new group_param_t( "Midtones"));
    create_params_inside_param( g.get(), "mid_");
    add_param( g);

    g.reset( new group_param_t( "Highlights"));
    create_params_inside_param( g.get(), "high_");
    add_param( g);
}

void color_correct2_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( color_correct_node_t);
}

void color_correct2_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( color_correct_node_t);
}

// factory
node_t *create_color_correct_node() { return new color_correct2_node_t();}

node_info_t color_correct_node_info()
{
    node_info_t info;
    info.id = "builtin.color_correct";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Color Correct";
    info.create = &create_color_correct_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( color_correct_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::color_correct_node_t)
BOOST_CLASS_EXPORT( ramen::color_correct2_node_t)
