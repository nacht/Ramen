// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE_LAYER_MODES_HPP
#define RAMEN_IMAGE_SSE_LAYER_MODES_HPP

#include<limits>

#include<xmmintrin.h>
#include<emmintrin.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

struct base_layer_mode_fun
{
public:

    base_layer_mode_fun( float opacity = 1.0f)
    {
        opacity_	= _mm_set_ps( opacity, opacity, opacity, opacity);
        inv_opacity_	= _mm_set_ps( 1.0f - opacity, 1.0f - opacity, 1.0f - opacity, 1.0f - opacity);
	alpha_max_	=  _mm_set_ps( 1.0f, std::numeric_limits<float>::max(),
					std::numeric_limits<float>::max(),
					std::numeric_limits<float>::max());

	alpha_min_	= _mm_set_ps( 0.0f, -std::numeric_limits<float>::max(),
					-std::numeric_limits<float>::max(),
					-std::numeric_limits<float>::max());
    }

protected:

    __m128 clamp_alpha( __m128 f) const
    {
	return _mm_min_ps( _mm_max_ps( f, alpha_min_), alpha_max_);
    }

    __m128 lerp( __m128 a, __m128 b) const
    {
	return _mm_add_ps( _mm_mul_ps( a, opacity_), _mm_mul_ps( b, inv_opacity_));
    }

    __m128 opacity_;
    __m128 inv_opacity_;
    __m128 alpha_min_;
    __m128 alpha_max_;
};

struct over_layer_mode_fun : public base_layer_mode_fun
{
    over_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

struct add_layer_mode_fun : public base_layer_mode_fun
{
    add_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

struct mul_layer_mode_fun : public base_layer_mode_fun
{
    mul_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

struct sub_layer_mode_fun : public base_layer_mode_fun
{
    sub_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

struct mix_layer_mode_fun : public base_layer_mode_fun
{
    mix_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

struct max_layer_mode_fun : public base_layer_mode_fun
{
    max_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

struct min_layer_mode_fun : public base_layer_mode_fun
{
    min_layer_mode_fun( float opacity = 1.0f) : base_layer_mode_fun( opacity) {}
    pixel_t operator()( const pixel_t& back, const pixel_t& front) const;
};

} // namespace
} // namespace
} // namespace

#endif
