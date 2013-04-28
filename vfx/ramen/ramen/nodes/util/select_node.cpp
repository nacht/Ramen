// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/util/select_node.hpp>

#include<ramen/params/float_param.hpp>

#include<iostream>

namespace ramen
{

select_node_t::select_node_t() : multi_input_node_t()
{
    set_name( "select");
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"]));
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"]), true);
    add_output_plug();
}

void select_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Select"));
    p->set_id( "select");
    p->set_default_value( 1);
    p->set_range( 1, 1);
    p->set_static( true);
    add_param( p);
}

void select_node_t::do_connected( node_t *src, int port)
{
    // if its the last port, add a new one
    if( port == num_inputs()-1)
    {
	add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"]), true);
	float_param_t *p = dynamic_cast<float_param_t*>( &param( "select"));
	p->set_range( 1, num_inputs());
    }

    reconnect_node();
}

bool select_node_t::do_is_valid() const
{
    float num = get_value<float>( param( "select")) - 1;

    if( num > num_inputs())
	return false;

    return input( num) != 0;
}

bool select_node_t::do_include_input_in_hash( int i) const
{
    float num = get_value<float>( param( "select")) - 1;
    return i == num;
}

void select_node_t::do_calc_domain( const render::render_context_t& context)
{
    set_domain( input( get_value<float>( param( "select")) - 1)->domain());
}

void select_node_t::do_calc_bounds( const render::render_context_t& context)
{
    set_bounds( input( get_value<float>( param( "select")) - 1)->bounds());
}

void select_node_t::do_calc_defined( const render::render_context_t& context)
{
    set_defined( input( get_value<float>( param( "select")) - 1)->defined());
}

void select_node_t::do_recursive_process( const render::render_context_t& context)
{
    float num = get_value<float>( param( "select")) - 1;
    input( num)->recursive_process( context);

    alloc_image();
    boost::gil::fill_pixels( image_view(), image::pixel_t( 0, 0, 0, 0));

    process( context);
}

void select_node_t::do_process( const render::render_context_t& context)
{
    float num = get_value<float>( param( "select"));
    node_t *src = input( num - 1);
    Imath::Box2i area( intersect( defined(), src->defined()));
    boost::gil::copy_pixels( src->const_subimage_view( area), subimage_view( area));
}

void select_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    int num_iplugs;
    ar & BOOST_SERIALIZATION_NVP( num_iplugs);

    select_node_t *this_ = const_cast<select_node_t*>( this);

    for( int i = 2; i < num_iplugs; ++i)
	this_->add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"]), true);

    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( multi_input_node_t);
}

void select_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    int num_iplugs = num_inputs();
    ar & BOOST_SERIALIZATION_NVP( num_iplugs);
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( multi_input_node_t);
}

// factory
node_t *create_select_node() { return new select_node_t();}

node_info_t select_node_info()
{
	node_info_t info;
	info.id = "builtin.select";
	info.major_version = 1;
	info.minor_version = 0;
	info.submenu = "Util";
	info.menu_item = "Select";
	info.create = &create_select_node;
	return info;
}

static bool registered = node_factory_t::Instance().register_node( select_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::select_node_t)
