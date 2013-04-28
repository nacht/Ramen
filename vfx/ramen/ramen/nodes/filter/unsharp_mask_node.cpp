// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/nodes/filter/unsharp_mask_node.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/params/float_param.hpp>
#include<ramen/params/popup_param.hpp>

#include<ramen/image/box_blur.hpp>

namespace ramen
{

namespace
{

struct unsharp_mask_fun
{
    unsharp_mask_fun( float amount, float thereshold) : amount_(amount), thereshold_( thereshold) {}

    image::pixel_t operator()( const image::pixel_t& src, const image::pixel_t& blured) const
    {
	image::pixel_t result;

	for( int i = 0; i < 3; ++i)
	{
	    float diff = src[i] - blured[i];

	    if( abs( 2 * diff) < thereshold_)
		diff = 0;

	    result[i] = src[i] + ( amount_ * diff);
	}

	result[3] = src[3];
	return result;
    }

    inline float abs( float x) const
    {
	if( x < 0)
	    return -x;

	return x;
    }

    float amount_;
    float thereshold_;
};

} // unnamed

unsharp_mask_node_t::unsharp_mask_node_t() : areaop_node_t()
{
    set_name( "unsharp");
}

void unsharp_mask_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Radius"));
    p->set_id( "radius");
    p->set_default_value( 3.0f);
    p->set_range( 0, 20);
    p->set_step( 0.5);
    add_param( p);

    p.reset( new float_param_t( "Amount"));
    p->set_id( "amount");
    p->set_default_value( 0.5f);
    p->set_range( 0, 3);
    p->set_step( 0.1);
    add_param( p);

    p.reset( new float_param_t( "Thereshold"));
    p->set_id( "theresh");
    p->set_default_value( 0.0f);
    p->set_range( 0, 3);
    p->set_step( 0.1);
    add_param( p);
}

void unsharp_mask_node_t::get_expand_radius( int& hradius, int& vradius) const
{
    float radius = get_value<float>( param( "radius"));
    radius /= 3.0f;

    hradius = std::ceil( radius + 1) * 3;
    vradius = std::ceil( radius + 1) * 3;
}

void unsharp_mask_node_t::do_process( const render::render_context_t& context)
{
    Imath::Box2i area( intersect( input()->defined(), defined()));

    if( area.isEmpty())
	return;

    image::const_image_view_t src( input()->const_subimage_view( area));
    image::image_view_t dst( subimage_view( area));

    float radius = get_value<float>( param( "radius"));
    radius /= 3.0f * context.subsample;

    image::box_blur_rgba( src, dst, radius, radius, 3);

    boost::gil::tbb_transform_pixels( src, dst, dst, unsharp_mask_fun( get_value<float>( param( "amount")),
									   get_value<float>( param( "theresh"))));

    boost::gil::copy_pixels( boost::gil::nth_channel_view( input()->const_subimage_view( area), 3),
				boost::gil::nth_channel_view( subimage_view( area), 3));
}

void unsharp_mask_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

void unsharp_mask_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( areaop_node_t);
}

// factory
node_t *create_unsharp_mask_node() { return new unsharp_mask_node_t();}

node_info_t unsharp_mask_node_info()
{
	node_info_t info;
	info.id = "builtin.unsharp_mask";
	info.major_version = 1;
	info.minor_version = 0;
	info.submenu = "Filter";
	info.menu_item = "Unsharp Mask";
	info.create = &create_unsharp_mask_node;
	return info;
}

static bool registered = node_factory_t::Instance().register_node( unsharp_mask_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::unsharp_mask_node_t)
