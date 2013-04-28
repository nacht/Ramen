// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/transform/move2d_node.hpp>

#include<ramen/params/transform2_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/separator_param.hpp>
#include<ramen/params/button_param.hpp>

#include<ramen/manipulators/transform2_manipulator.hpp>

namespace ramen
{

move2d_node_t::move2d_node_t() : image_xform_node_t() { set_name("move2d");}

void move2d_node_t::do_create_params()
{
    std::auto_ptr<transform2_param_t> xp( new transform2_param_t( "xform", "xf"));
    add_param( xp);

    std::auto_ptr<popup_param_t> r( new popup_param_t( "Filter"));
    r->set_id( "filter");
    r->menu_items() += "Point", "Bilinear", "Bicubic";
    r->set_default_value( 2);
    add_param( r);

    std::auto_ptr<button_param_t> b( new button_param_t( "Apply Track..."));
    b->set_id( "apply_track");
    b->set_enabled( false);
    add_param( b);

    std::auto_ptr<separator_param_t> sep( new separator_param_t());
    add_param( sep);

    std::auto_ptr<motion_blur_param_t> mb( new motion_blur_param_t( "Motion Blur"));
    mb->set_id( "motion_blur");
    add_param( mb);
}

void move2d_node_t::do_create_manipulators()
{
    transform2_param_t *p = dynamic_cast<transform2_param_t*>( &param( "xf"));
    assert( p);

    std::auto_ptr<transform2_manipulator_t> m( new transform2_manipulator_t( p));
    add_manipulator( m);
}

boost::optional<Imath::M33d> move2d_node_t::do_calc_transform_matrix_at_time( float time, int subsample) const
{
    const transform2_param_t *p = dynamic_cast<const transform2_param_t*>( &param( "xf"));
    assert( p);
    return p->matrix_at_time( time, subsample);
}

void move2d_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_xform_node_t);
}

void move2d_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_xform_node_t);
}

// factory

node_t *create_move2d_node() { return new move2d_node_t();}

node_info_t move2d_node_info()
{
    node_info_t info;
    info.id = "builtin.move2d";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Transform";
    info.menu_item = "Move2D";
    info.create = &create_move2d_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( move2d_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::move2d_node_t)
