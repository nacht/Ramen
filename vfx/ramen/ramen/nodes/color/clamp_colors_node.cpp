// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/color/clamp_colors_node.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>

namespace ramen
{

namespace
{

struct clamp_colors_fun
{
    clamp_colors_fun( float lo, float hi) : lo_(lo), hi_(hi) {}

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	float r = boost::gil::get_color( src, boost::gil::red_t());
	float g = boost::gil::get_color( src, boost::gil::green_t());
	float b = boost::gil::get_color( src, boost::gil::blue_t());

	if( r < lo_) r = 0;
	if( g < lo_) g = 0;
	if( b < lo_) b = 0;

	if( r > hi_) r = hi_;
	if( g > hi_) g = hi_;
	if( b > hi_) b = hi_;

	return image::pixel_t( r, g, b, boost::gil::get_color( src, boost::gil::alpha_t()));
    }

    float lo_, hi_;
};

}

clamp_colors_node_t::clamp_colors_node_t() : pointop_node_t() { set_name("clamp_col");}

void clamp_colors_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Min"));
    p->set_id( "min");
    p->set_default_value( 0.0f);
    p->set_step( 0.01f);
    add_param( p);

    p.reset( new float_param_t( "Max"));
    p->set_id( "max");
    p->set_default_value( 1.0f);
    p->set_step( 0.01f);
    add_param( p);
}

void clamp_colors_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    boost::gil::tbb_transform_pixels( src, dst, clamp_colors_fun( get_value<float>( param( "min")), get_value<float>( param( "max"))));
}

void clamp_colors_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void clamp_colors_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_clamp_colors_node() { return new clamp_colors_node_t();}

node_info_t clamp_colors_node_info()
{
    node_info_t info;
    info.id = "builtin.clamp_colors";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Color";
    info.menu_item = "Clamp Colors";
    info.create = &create_clamp_colors_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( clamp_colors_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::clamp_colors_node_t)
