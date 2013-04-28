// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/transform/expand_node.hpp>

#include<ramen/params/float_param.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

namespace ramen
{

expand_node_t::expand_node_t() : image_node_t()
{
    set_name("expand");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

void expand_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Top"));
    p->set_id( "top");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Left"));
    p->set_id( "left");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Bottom"));
    p->set_id( "bottom");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_y, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);

    p.reset( new float_param_t( "Right"));
    p->set_id( "right");
    p->set_range( 0, 1);
    p->set_numeric_type( numeric_param_t::relative_size_x, 0);
    p->set_default_value( 0);
    p->set_static( true);
    add_param( p);
}

void expand_node_t::do_calc_domain( const render::render_context_t& context)
{
    Imath::Box2i box( input()->domain());
    int exp_t = get_absolute_value<float>( param( "top"));
    int exp_l = get_absolute_value<float>( param( "left"));
    int exp_b = get_absolute_value<float>( param( "bottom"));
    int exp_r = get_absolute_value<float>( param( "right"));
    set_domain( Imath::Box2i( Imath::V2i( box.min.x - exp_l, box.min.y - exp_t), Imath::V2i( box.max.x + exp_r, box.max.y + exp_b)));
}

void expand_node_t::do_calc_bounds( const render::render_context_t& context)
{
    Imath::Box2i bounds( intersect( input()->bounds(), domain()));
    set_bounds( bounds);
}

void expand_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    Imath::Box2i roi = intersect( interest(), domain());
    input()->add_interest( roi);
}

void expand_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i def( intersect( input()->defined(), intersect( domain(), interest())));
    set_defined( def);
}

void expand_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area = intersect( input()->defined(), defined());

    if( area.isEmpty())
	return;

    boost::gil::copy_pixels( input()->const_subimage_view( area), subimage_view( area));
}

void expand_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

void expand_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP(image_node_t);
}

// factory
image_node_t *create_expand_node() { return new expand_node_t();}

node_info_t expand_node_info()
{
    node_info_t info;
    info.id = "builtin.expand";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Transform";
    info.menu_item = "Expand";
    info.create = &create_expand_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node(expand_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::expand_node_t)
