// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_COLOR_CORRECT_HPP
#define	RAMEN_COLOR_CORRECT_HPP

#include<cmath>

#include<algorithm>

#include<boost/shared_ptr.hpp>
#include<boost/bind.hpp>

#include<half/half_lut.hpp>

#include<ramen/image/typedefs.hpp>
#include<ramen/image/color.hpp>

namespace ramen
{

namespace
{

struct always_one
{
    template<class T>
    T operator()( T x) const { return 1;}
};

float lerp( float a, float b, float t) { return (a * t) + ((1.0f - t) * b);}

} // unnamed

template<class BlendFun = always_one>
class color_correct_fun
{
public:

    color_correct_fun( float r_gamma, float g_gamma, float b_gamma,
		    float r_gain, float g_gain, float b_gain,
		    float r_offset, float g_offset, float b_offset,
		    float saturation, float contrast,
		    half_lut_t& rlut, half_lut_t& glut, half_lut_t& blut,
		    BlendFun f = BlendFun()) : f_(f), r_lut_( rlut), g_lut_(glut), b_lut_(blut)
    {
	r_gamma_ = r_gamma;
	g_gamma_ = g_gamma;
	b_gamma_ = b_gamma;
	r_gain_  = r_gain;
	g_gain_  = g_gain;
	b_gain_  = b_gain;
	r_offset_= r_offset;
	g_offset_= g_offset;
	b_offset_= b_offset;
	saturation_ = saturation;
	contrast_ = contrast;

	identity_ = (r_gamma_ == 1.0f)    && (g_gamma_ == 1.0f)  && (b_gamma_ == 1.0f)  &&
		    (r_gain_ == 1.0f)     && (g_gain_ == 1.0f)   && (b_gain_ == 1.0f)   &&
		    (r_offset_ == 0.0f)   && (g_offset_ == 0.0f) && (b_offset_ == 0.0f) &&
		    (saturation_ == 1.0f) && (contrast_ == 1.0f);

	if( !identity_)
	{
	    r_lut_.init( boost::bind( &color_correct_fun::cc_lut_fun, this, _1, r_gamma_, r_gain_, r_offset_, contrast_), 0, HALF_MAX);
	    g_lut_.init( boost::bind( &color_correct_fun::cc_lut_fun, this, _1, g_gamma_, g_gain_, g_offset_, contrast_), 0, HALF_MAX);
	    b_lut_.init( boost::bind( &color_correct_fun::cc_lut_fun, this, _1, b_gamma_, b_gain_, b_offset_, contrast_), 0, HALF_MAX);
	}
    }

    image::pixel_t operator()( const image::pixel_t& src1, const image::pixel_t& src2) const
    {
	if( identity_)
	    return src2;

	float y = image::luminance( src1);

	float fract = f_( y);

	if( fract == 0.0f)
	    return src2;

	image::pixel_t dst( lerp( src2[0], y, saturation_),
                            lerp( src2[1], y, saturation_),
                            lerp( src2[2], y, saturation_),
                            src2[3]);

	dst[0] = (float) r_lut_( (float) dst[0]);
	dst[1] = (float) g_lut_( (float) dst[1]);
	dst[2] = (float) b_lut_( (float) dst[2]);

	if( fract < 1.0f)
	{
	    dst[0] = lerp( dst[0], src2[0], fract);
	    dst[1] = lerp( dst[1], src2[1], fract);
	    dst[2] = lerp( dst[2], src2[2], fract);
	}

	// clamp negative values
	dst[0] = std::max( (float) dst[0], 0.0f);
	dst[1] = std::max( (float) dst[1], 0.0f);
	dst[2] = std::max( (float) dst[2], 0.0f);
	return dst;
    }

private:

    half cc_lut_fun( half x, float gamma, float gain, float offset, float contrast) const
    {
	float y = std::pow( x, gamma);
	y *= gain;
	y += offset;
	return lerp( y, 0.18f, contrast);
    }

    float r_gamma_, g_gamma_, b_gamma_;
    float r_gain_, g_gain_, b_gain_;
    float r_offset_, g_offset_, b_offset_;
    float saturation_;
    float contrast_;
    BlendFun f_;

    half_lut_t& r_lut_;
    half_lut_t& g_lut_;
    half_lut_t& b_lut_;

    bool identity_;
};

} // namespace

#endif

