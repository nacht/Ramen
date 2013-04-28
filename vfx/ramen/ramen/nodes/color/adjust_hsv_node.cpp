// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/adjust_hsv_node.hpp>

#include<ramen/ImathExt/ImathMatrixAlgo.h>

#include<ramen/image/processing.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{

adjust_hsv_node_t::adjust_hsv_node_t() : pointop_node_t() { set_name("adjust_hsv");}

void adjust_hsv_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Hue rot"));
    p->set_id( "hue_rot");
    p->set_default_value( 0);
    p->set_step( 0.5f);
    add_param( p);

    p.reset( new float_param_t( "Saturation"));
    p->set_id( "sat");
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);

    p.reset( new float_param_t( "Value"));
    p->set_id( "value");
    p->set_min( 0);
    p->set_default_value( 1);
    p->set_step( 0.05f);
    add_param( p);
}

void adjust_hsv_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    Imath::M44f m = hueRotationMatrix( get_value<float>( param( "hue_rot"))) *
                    saturationMatrix( get_value<float>( param( "sat"))) *
                    gainMatrix( get_value<float>( param( "value")));

    image::apply_color_matrix( src, dst, m);
}

void adjust_hsv_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void adjust_hsv_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_adjust_hsv_node() { return new adjust_hsv_node_t();}

node_info_t adjust_hsv_node_info()
{
    node_info_t info;
    info.id = "builtin.adjust_hsv";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Adjust HSV";
    info.create = &create_adjust_hsv_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( adjust_hsv_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::adjust_hsv_node_t)
