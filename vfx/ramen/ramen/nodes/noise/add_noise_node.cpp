// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/noise/add_noise_node.hpp>

#include<boost/lexical_cast.hpp>
#include<boost/random/mersenne_twister.hpp>
#include<boost/random/normal_distribution.hpp>
#include<boost/random/uniform_01.hpp>
#include<boost/random/variate_generator.hpp>

#include<gil/extension/algorithm/tbb/tbb.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>
#include<ramen/params/bool_param.hpp>

namespace ramen
{

namespace
{

boost::mt19937 engine;
boost::normal_distribution<> normal_distribution;

boost::uniform_01<boost::mt19937, float>					uniform_rand( engine);
boost::variate_generator<boost::mt19937&, boost::normal_distribution<> >	normal_rand( engine, normal_distribution);

struct add_uniform_col_noise_fun
{
    add_uniform_col_noise_fun( float amount) : amount_( amount)
    {
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	return image::pixel_t( boost::gil::get_color( src, boost::gil::red_t())	    + (2.0f * amount_ * ( uniform_rand() - 0.5f)),
				boost::gil::get_color( src, boost::gil::green_t())  + (2.0f * amount_ * ( uniform_rand() - 0.5f)),
				boost::gil::get_color( src, boost::gil::blue_t())   + (2.0f * amount_ * ( uniform_rand() - 0.5f)),
				boost::gil::get_color( src, boost::gil::alpha_t()));
    }

private:

    float amount_;
};

struct add_normal_col_noise_fun
{
    add_normal_col_noise_fun( float amount) : amount_( amount)
    {
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	return image::pixel_t( boost::gil::get_color( src, boost::gil::red_t())	    + (2.0f * amount_ * ( normal_rand() - 0.5f)),
				boost::gil::get_color( src, boost::gil::green_t())  + (2.0f * amount_ * ( normal_rand() - 0.5f)),
				boost::gil::get_color( src, boost::gil::blue_t())   + (2.0f * amount_ * ( normal_rand() - 0.5f)),
				boost::gil::get_color( src, boost::gil::alpha_t()));
    }

private:

    float amount_;
};

struct add_uniform_mono_noise_fun
{
    add_uniform_mono_noise_fun( float amount) : amount_( amount)
    {
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	float r = 2.0f * amount_ * ( uniform_rand() - 0.5f);
	return image::pixel_t( boost::gil::get_color( src, boost::gil::red_t())	    + r,
				boost::gil::get_color( src, boost::gil::green_t())  + r,
				boost::gil::get_color( src, boost::gil::blue_t())   + r,
				boost::gil::get_color( src, boost::gil::alpha_t()));
    }

private:

    float amount_;
};

struct add_normal_mono_noise_fun
{
    add_normal_mono_noise_fun( float amount) : amount_( amount)
    {
    }

    image::pixel_t operator()( const image::pixel_t& src) const
    {
	float r = 2.0f * amount_ * ( normal_rand() - 0.5f);
	return image::pixel_t( boost::gil::get_color( src, boost::gil::red_t())	    + r,
				boost::gil::get_color( src, boost::gil::green_t())  + r,
				boost::gil::get_color( src, boost::gil::blue_t())   + r,
				boost::gil::get_color( src, boost::gil::alpha_t()));
    }

private:

    float amount_;
};

}

add_noise_node_t::add_noise_node_t() : pointop_node_t() { set_name("add_noise");}

void add_noise_node_t::do_create_params()
{
    std::auto_ptr<popup_param_t> q( new popup_param_t( "Type"));
    q->set_id( "type");
    q->menu_items() += "Uniform Color", "Gaussian Color", "Uniform Mono", "Gaussian Mono";
    add_param( q);

    std::auto_ptr<float_param_t> p( new float_param_t( "Amount"));
    p->set_id( "amount");
    p->set_default_value( 0.0f);
    p->set_range( 0, 100);
    p->set_step( 0.05);
    add_param( p);
}

void add_noise_node_t::do_calc_hash( const render::render_context_t& context)
{
    std::string t = boost::lexical_cast<std::string>( uniform_rand());
    append_to_hash( t);
}

void add_noise_node_t::do_process( const image::const_image_view_t& src, const image::image_view_t& dst, const render::render_context_t& context)
{
    float amount = get_value<float>( param( "amount"));

    switch( get_value<int>( param( "type")))
    {
    case 0:
	boost::gil::transform_pixels( src, dst, add_uniform_col_noise_fun( amount));
    break;

    case 1:
	boost::gil::transform_pixels( src, dst, add_normal_col_noise_fun( amount));
    break;

    case 2:
	boost::gil::transform_pixels( src, dst, add_uniform_mono_noise_fun( amount));
    break;

    case 3:
	boost::gil::transform_pixels( src, dst, add_normal_mono_noise_fun( amount));
    break;

    default:
	assert( 0);
    }
}

void add_noise_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

void add_noise_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( pointop_node_t);
}

// factory
node_t *create_add_noise_node() { return new add_noise_node_t();}

node_info_t add_noise_node_info()
{
    node_info_t info;
    info.id = "builtin.add_noise";
    info.major_version = 1;
    info.minor_version = 0;
    info.submenu = "Noise";
    info.menu_item = "Add Noise";
    info.create = &create_add_noise_node;
    return info;
}

static bool registered = node_factory_t::Instance().register_node( add_noise_node_info());

} // namespace

BOOST_CLASS_EXPORT(ramen::add_noise_node_t)
