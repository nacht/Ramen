// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/sse/layer_modes.hpp>

#include<algorithm>

#include<adobe/algorithm/clamp.hpp>

using namespace boost::gil;

namespace ramen
{
namespace image
{
namespace sse
{

pixel_t over_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));
    f = _mm_mul_ps( f, opacity_);

    float *p = reinterpret_cast<float*>( &f);

    float alpha = p[3];
    float alpha_inv = 1.0f - alpha;

    __m128 a = _mm_set_ps( 1.0f, alpha_inv, alpha_inv, alpha_inv);

    b = _mm_mul_ps( b, a);
    f = _mm_add_ps( f, b);
    f = clamp_alpha( f);

    p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);
}

pixel_t add_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));

    f = _mm_mul_ps( f, opacity_);
    f = _mm_add_ps( f, b);
    f = clamp_alpha( f);

    float *p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);
}

pixel_t mul_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));

    f = lerp( _mm_mul_ps( f, b), b);
    float *p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);
}

pixel_t sub_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));

    f = _mm_mul_ps( f, opacity_);
    f = _mm_sub_ps( b, f);
    f = clamp_alpha( f);

    float *p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);}

pixel_t mix_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));

    f = lerp( f, b);
    float *p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);
}

pixel_t max_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));

    f = lerp( _mm_max_ps( f, b), b);
    float *p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);
}



pixel_t min_layer_mode_fun::operator()( const pixel_t& back, const pixel_t& front) const
{
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &back));
    __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &front));

    f = lerp( _mm_min_ps( f, b), b);
    float *p = reinterpret_cast<float*>( &f);
    return pixel_t( p[0], p[1], p[2], p[3]);
}

} // namespace
} // namespace
} // namespace
