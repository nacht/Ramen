// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/base/rescale_node.hpp>

#include<ramen/params/combo_group_param.hpp>
#include<ramen/params/float2_param.hpp>
#include<ramen/params/format_param.hpp>

namespace ramen
{

rescale_node_t::rescale_node_t() : image_node_t()
{
    add_input_plug( input_plug_info_t( ui::palette_t::Instance()["front plug"], "Front"));
    add_output_plug();
}

void rescale_node_t::do_create_params()
{
    std::auto_ptr<combo_group_param_t> top( new combo_group_param_t( "Method"));
    top->set_id( "method");

    // Absolute
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Size"));
	group->set_id( "size_group");

	std::auto_ptr<format_param_t> fp( new format_param_t( "Size"));
	fp->set_id( "size");
	group->add_param( fp);
	top->add_param( group);
    }

    // Scale
    {
	std::auto_ptr<composite_param_t> group( new composite_param_t( "Scale"));
	group->set_id( "scale_group");

	std::auto_ptr<float2_param_t> fp( new float2_param_t( "Scale"));
	fp->set_id( "scale");
	fp->set_min( 0);
	fp->set_step( 0.05);
	fp->set_default_value( Imath::V2f( 1, 1));
	fp->set_proportional( true);
	fp->set_static( true);
	group->add_param( fp);
	top->add_param( group);
    }

    add_param( top);
}

void rescale_node_t::do_calc_domain( const render::render_context_t& context)
{
    if( get_value<int>( param( "method")) == 0)
    {
	Imath::Box2i box( get_value<format_t>( param( "size")).area());
	scale_x_ = (float) ( box.max.x - 1) / input()->domain().size().x;
	scale_y_ = (float) ( box.max.y - 1) / input()->domain().size().y;
    }
    else
    {
	Imath::V2f scale = get_value<Imath::V2f>( param( "scale"));
	scale_x_ = scale.x;
	scale_y_ = scale.y;
    }

    set_domain( scale( input()->domain(), scale_x_, scale_y_, input()->domain().min.x, input()->domain().min.y));
}

void rescale_node_t::do_calc_bounds( const render::render_context_t& context)
{
    if( get_value<int>( param( "method")) == 0)
    {
	Imath::Box2i box( get_value<format_t>( param( "size")).area());
	scale_x_ = (float) ( box.max.x - 1) / input()->domain().size().x;
	scale_y_ = (float) ( box.max.y - 1) / input()->domain().size().y;
    }
    else
    {
	Imath::V2f scale = get_value<Imath::V2f>( param( "scale"));
	scale_x_ = scale.x;
	scale_y_ = scale.y;
    }

    set_bounds( scale( input()->bounds(), scale_x_, scale_y_, input()->domain().min.x, input()->domain().min.y));
}

void rescale_node_t::do_calc_inputs_interest( const render::render_context_t& context)
{
    input()->add_interest( scale( interest(), 1.0f / scale_x_, 1.0f / scale_y_, input()->domain().min.x, input()->domain().min.y));
}

void rescale_node_t::do_calc_defined( const render::render_context_t& context)
{
    Imath::Box2i in_xf_defined( scale( input()->defined(), scale_x_, scale_y_, input()->domain().min.x, input()->domain().min.y));
    set_defined( intersect( in_xf_defined, intersect( domain(), interest())));
}

Imath::Box2i rescale_node_t::needed_area() const
{
    Imath::Box2i needed_area = scale( defined(), 1.0f / scale_x_, 1.0f / scale_y_, input()->domain().min.x, input()->domain().min.y);
    return intersect( needed_area, input()->defined());
}

void rescale_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

void rescale_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( image_node_t);
}

} // namespace

BOOST_CLASS_EXPORT( ramen::rescale_node_t)
