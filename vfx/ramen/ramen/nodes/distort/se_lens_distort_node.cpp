// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#include<ramen/nodes/distort/se_lens_distort_node.hpp>

#include<boost/tr1/tuple.hpp>
#include<boost/scoped_array.hpp>
#include<boost/math/tools/roots.hpp>

#include<OpenEXR/ImathFun.h>

#include<ramen/params/float_param.hpp>
#include<ramen/params/bool_param.hpp>

#include<ramen/image/processing.hpp>

namespace ramen
{

namespace
{

// this code comes from ampas CTL

inline void indicesAndWeights (float r, int iMax, int &i, int &i1, float &u, float &u1)
{
    if (r >= 0)
    {
	if (r < iMax)
	{
	    //
	    // r is finite and in the interval [0, iMax[
	    //

	    i = int (r);
	    i1 = i + 1;
	    u  = r - i;
	}
	else
	{
	    //
	    // r is greater than or equal to iMax
	    //

	    i = i1 = iMax;
	    u = 1;
	}
    }
    else
    {
	//
	// r is either NaN or less than 0
	//

	i = i1 = 0;
	u = 1;
    }

    u1 = 1 - u;
}

float
lookup1D
    (const float table[],
     int size,
     float pMin,
     float pMax,
     float p)
{
    int iMax = size - 1;
    float r = ( Imath::clamp (p, pMin, pMax) - pMin) / (pMax - pMin) * iMax;

    int i, i1;
    float u, u1;
    indicesAndWeights (r, iMax, i, i1, u, u1);

    return table[i] * u1 + table[i1] * u;
}


float
lookupCubic1D
    (const float table[],
     int size,
     float pMin,
     float pMax,
     float p)
{
    if (size < 3)
	return lookup1D (table, size, pMin, pMax, p);

    int iMax = size - 1;
    float r = (Imath::clamp (p, pMin, pMax) - pMin) / (pMax - pMin) * iMax;
    int i;

    if (r >= 0 && r < iMax)
    {
	//
	// r is finite and in the interval [0, iMax[
	//

	i = int (r);
    }
    else if (r >= iMax)
    {
	//
	// r is greater than or equal to iMax
	//

	return table[iMax];
    }
    else
    {
	//
	// r is either NaN or less than 0
	//

	return table[0];
    }

    float dy = (table[i+1] - table[i]);
    float m0, m1;

    if (i > 0)
	m0 = (dy + (table[i] - table[i-1])) * 0.5f;

    if (i < size - 2)
	m1 = (dy + (table[i+2] - table[i+1])) * 0.5f;

    if (i <= 0)
	m0 = (3 * dy - m1) * 0.5f;

    if (i >= size - 2)
	m1 = (3 * dy - m0) * 0.5f;

    float t = r - i;
    float t2 = t * t;
    float t3 = t2 * t;

    return table[i] * (2 * t3 - 3 * t2 + 1) +
           m0 * (t3 - 2 * t2 + t) +
	   table[i+1] * (-2 * t3 + 3 * t2) +
	   m1 * (t3 - t2);
}

// end of ampas CTL code

struct se_lens_warp_fun
{
    se_lens_warp_fun( float k, float k3, const Imath::Box2i& domain, float zoom) : domain_( domain)
    {
	k_ = k;
	k3_ = k3;

	center_.x = ( domain_.max.x + domain_.min.x) / 2.0f;
	center_.y = ( domain_.max.y + domain_.min.y) / 2.0f;

	if( domain_.size().x > domain_.size().y)
	{
	    xaspc_ = 1.0f;
	    yaspc_ = (float) domain_.size().y / domain_.size().x;
	}
	else
	{
	    yaspc_ = 1.0f;
	    xaspc_ = (float) domain_.size().x / domain_.size().y;
	}

	zoom_ = zoom;
    }

    Imath::V2f operator()( const Imath::V2f& p) const
    {
	// transform to 0..1
	float u = (p.x - domain_.min.x) / (float) domain_.size().x;
	float v = (p.y - domain_.min.y) / (float) domain_.size().y;

	// transform to -1..1 taking into acount the aspect ratio
	u = ( u - 0.5f) * 2.0f * xaspc_;
	v = ( v - 0.5f) * 2.0f * yaspc_;

	float r2 = ( u * u) + ( v * v);
	float r = std::sqrt( (double) r2);

	float f = 1.0f + r2 * ( k_ + k3_ * r);

	// add distortion and zoom
	u = ( f * u) / zoom_;
	v = ( f * v) / zoom_;

	// transform back to 0..1
	u = (u / xaspc_ / 2.0f) + 0.5f;
	v = (v / yaspc_ / 2.0f) + 0.5f;

	// and back to original image size
	return Imath::V2f( u * domain_.size().x + domain_.min.x,
			   v * domain_.size().y + domain_.min.y);
    }

private:

    float k_, k3_;
    Imath::Box2i domain_;
    Imath::V2f center_;
    float xaspc_, yaspc_;
    float zoom_;
};

struct se_lens_warp_lut_fun
{
public:

    se_lens_warp_lut_fun( float *lut, int lut_size, const Imath::Box2i& domain, float zoom) : lut_( lut), domain_( domain)
    {
	lut_size_ = lut_size;

	center_.x = ( domain_.max.x + domain_.min.x) / 2.0f;
	center_.y = ( domain_.max.y + domain_.min.y) / 2.0f;

	if( domain_.size().x > domain_.size().y)
	{
	    xaspc_ = 1.0f;
	    yaspc_ = (float) domain_.size().y / domain_.size().x;
	}
	else
	{
	    yaspc_ = 1.0f;
	    xaspc_ = (float) domain_.size().x / domain_.size().y;
	}

	zoom_ = zoom;
    }

    Imath::V2f operator()( const Imath::V2f& p) const
    {
	// transform to 0..1
	float u = (p.x - domain_.min.x) / (float) domain_.size().x;
	float v = (p.y - domain_.min.y) / (float) domain_.size().y;

	// transform to -1..1 taking into acount the aspect ratio
	u = ( u - 0.5f) * 2.0f * xaspc_;
	v = ( v - 0.5f) * 2.0f * yaspc_;

	float r2 = ( u * u) + ( v * v);

	// interpolate f from the values saved in the table
	//float f = lookupCubic1D( lut_, lut_size_, 0, 2, r2);
	float f = lookup1D( lut_, lut_size_, 0, 2, r2);

	// add distortion and zoom
	u = ( f * u) / zoom_;
	v = ( f * v) / zoom_;

	// transform back to 0..1
	u = (u / xaspc_ / 2.0f) + 0.5f;
	v = (v / yaspc_ / 2.0f) + 0.5f;

	// and back to original image size
	return Imath::V2f( u * domain_.size().x + domain_.min.x,
			   v * domain_.size().y + domain_.min.y);
    }

private:

    float *lut_;
    int lut_size_;
    Imath::Box2i domain_;
    Imath::V2f center_;
    float xaspc_, yaspc_;
    float zoom_;
};

// quartic equation and derivatives for root finding
struct distort_fun
{
public:

    distort_fun( float k, float k3, float c) : k_(k), k3_(k3), c_(c) {}

    std::tr1::tuple<float, float, float> operator()( float r) const
    {
	float r2 = r*r;
	float y	    = ( k3_ * r2 * r2) + ( k_ * r2 * r) + r - c_;
	float dy    = ( 4.0f * k3_ * r2 * r) + ( 3.0f * k_ * r2) + 1.0f;
	float ddy   = ( 12.0f * k3_ * r2) + ( 6.0f * k_ * r);
	return std::tr1::tuple<float,float,float>( y, dy, ddy);
    }

private:

    float k_, k3_, c_;
};

} // namespace

const char *se_lens_distort_node_t::help_string() const
{
    return "Adds or removes lens distortion from an image using SynthEyes's distortion model.";
}

se_lens_distort_node_t::se_lens_distort_node_t() : distort_node_t()
{
    set_name( "se_lens_dist");
}

void se_lens_distort_node_t::do_create_params()
{
    std::auto_ptr<float_param_t> p( new float_param_t( "Distortion"));
    p->set_id( "distort");
    p->set_default_value( 0.0f);
    p->set_range( -1, 1);
    p->set_step( 0.01);
    add_param( p);

    p.reset( new float_param_t( "Cubic Dist"));
    p->set_id( "cubic_dist");
    p->set_default_value( 0.0f);
    p->set_range( -1, 1);
    p->set_step( 0.01);
    add_param( p);

    std::auto_ptr<bool_param_t> q( new bool_param_t( "Invert"));
    q->set_id( "invert");
    q->set_default_value( false);
    add_param( q);

    p.reset( new float_param_t( "Zoom"));
    p->set_id( "zoom");
    p->set_default_value( 1.0f);
    p->set_range( .1, 10);
    p->set_step( 0.1);
    add_param( p);
}

void se_lens_distort_node_t::do_process( const render::render_context_t& context)
{
    bool inv = get_value<bool>( param( "invert"));

    if( inv)
    {
	int lut_size = ( std::max( domain().size().x, domain().size().y) / 2) + 1;
	boost::scoped_array<float> lut( new float[lut_size]);

	generate_undistort_table( lut.get(), lut_size);
	se_lens_warp_lut_fun f( lut.get(), lut_size, domain(), get_value<float>( param( "zoom")));

	image::warp_bicubic( input()->defined(), input()->const_image_view(), defined(), image_view(), f);
    }
    else
    {
	se_lens_warp_fun f( get_value<float>( param( "distort")),  get_value<float>( param( "cubic_dist")),
				domain(), get_value<float>( param( "zoom")));

	image::warp_bicubic( input()->defined(), input()->const_image_view(), defined(), image_view(), f);
    }
}

void se_lens_distort_node_t::generate_distort_table( float *lut, int lut_size)
{
    float k  = get_value<float>( param( "distort"));
    float k3 = get_value<float>( param( "cubic_dist"));

    for( int i = 0; i < lut_size; ++i)
    {
	float r2 = 2.0f * i / lut_size;
	float f = r2 * ( k + k3 * std::sqrt( (double) r2));
	lut[i] = f + 1.0f;
    }
}

void se_lens_distort_node_t::generate_undistort_table( float *lut, int lut_size)
{
    float k  = get_value<float>( param( "distort"));
    float k3 = get_value<float>( param( "cubic_dist"));

    int digits = std::numeric_limits<float>::digits / 2;
    float sqrt2 = std::sqrt( 2.0);

    // avoid division by zero
    lut[0] = 1.0f;

    for( int i = 1; i < lut_size; ++i)
    {
	float c = std::sqrt( (double) 2.0f * i / lut_size);
	float r = boost::math::tools::halley_iterate( distort_fun( k, k3, c), c, 0.0f, sqrt2, digits);
	lut[i] = r / c;
    }
}

void se_lens_distort_node_t::serialize( boost::archive::polymorphic_iarchive& ar, const unsigned int version)
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( distort_node_t);
}

void se_lens_distort_node_t::serialize( boost::archive::polymorphic_oarchive& ar, const unsigned int version) const
{
    ar & BOOST_SERIALIZATION_BASE_OBJECT_NVP( distort_node_t);
}

// factory
node_t *create_se_lens_distort_node() { return new se_lens_distort_node_t();}

node_info_t se_lens_distort_node_info()
{
	node_info_t info;
	info.id = "builtin.se_lens_distort";
	info.major_version = 1;
	info.minor_version = 0;
	info.submenu = "Distort";
	info.menu_item = "SynthEyes Lens Distort";
	info.create = &create_se_lens_distort_node;
	return info;
}

static bool registered = node_factory_t::Instance().register_node( se_lens_distort_node_info());

} // namespace

BOOST_CLASS_EXPORT( ramen::se_lens_distort_node_t)
