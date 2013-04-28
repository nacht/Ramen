// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.


#include<ramen/image/processing/sse/math.hpp>

#include<xmmintrin.h>
#include<emmintrin.h>

#include<gil/extension/algorithm/tbb/tbb.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

namespace detail
{

struct add_pixels
{
    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
        __m128 p0 = _mm_load_ps( reinterpret_cast<const float*>( &a));
        __m128 p1 = _mm_load_ps( reinterpret_cast<const float*>( &b));

        p1 = _mm_add_ps( p0, p1);

        float *p = reinterpret_cast<float*>( &p1);
        return pixel_t( p[0], p[1], p[2], p[3]);
    }
};

struct sub_pixels
{
    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
        __m128 p0 = _mm_load_ps( reinterpret_cast<const float*>( &a));
        __m128 p1 = _mm_load_ps( reinterpret_cast<const float*>( &b));

        p1 = _mm_sub_ps( p0, p1);

        float *p = reinterpret_cast<float*>( &p1);
        return pixel_t( p[0], p[1], p[2], p[3]);
    }
};

struct mul_pixels
{
    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
        __m128 p0 = _mm_load_ps( reinterpret_cast<const float*>( &a));
        __m128 p1 = _mm_load_ps( reinterpret_cast<const float*>( &b));

        p1 = _mm_mul_ps( p1, p0);

        float *p = reinterpret_cast<float*>( &p1);
        return pixel_t( p[0], p[1], p[2], p[3]);
    }
};

struct madd_pixels
{
    madd_pixels( float factor)
    {
        factor_ = _mm_set_ps( factor, factor, factor, factor);
    }

    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
        __m128 p0 = _mm_load_ps( reinterpret_cast<const float*>( &a));
        __m128 p1 = _mm_load_ps( reinterpret_cast<const float*>( &b));

        p1 = _mm_add_ps( p0, _mm_mul_ps( p1, factor_));

        float *p = reinterpret_cast<float*>( &p1);
        return pixel_t( p[0], p[1], p[2], p[3]);
    }

private:

    __m128 factor_;
};

struct mul_pixel_scalar
{
    mul_pixel_scalar( float factor)
    {
        factor_ = _mm_set_ps( factor, factor, factor, factor);
    }

    pixel_t operator()( const pixel_t& p) const
    {
        __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &p));
        f = _mm_mul_ps( f, factor_);

        float *r = reinterpret_cast<float*>( &f);
        return pixel_t( r[0], r[1], r[2], r[3]);
    }

private:

    __m128 factor_;
};

struct lerp_pixels
{
    lerp_pixels( float t)
    {
        t_	= _mm_set_ps( t, t, t, t);
        t_inv_	= _mm_set_ps( 1.0f - t, 1.0f - t, 1.0f - t, 1.0f - t);
    }

    pixel_t operator()( const pixel_t& a, const pixel_t& b) const
    {
        __m128 p0 = _mm_load_ps( reinterpret_cast<const float*>( &a));
        __m128 p1 = _mm_load_ps( reinterpret_cast<const float*>( &b));

        p0 = _mm_mul_ps( p0, t_);
        p1 = _mm_mul_ps( p1, t_inv_);
        p1 = _mm_add_ps( p1, p0);

        float *r = reinterpret_cast<float*>( &p1);
        return pixel_t( r[0], r[1], r[2], r[3]);
    }

private:

    __m128 t_, t_inv_;
};

} // detail

// images

void add_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::add_pixels());
}

void sub_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::sub_pixels());
}

void mul_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::mul_pixels());
}

void mul_image_scalar( const const_image_view_t& a, float f, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, result, detail::mul_pixel_scalar( f));
}

void madd_images( const const_image_view_t& a, const const_image_view_t& b, float f, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::madd_pixels( f));
}

void lerp_images( const const_image_view_t& a, const const_image_view_t& b, float t, const image_view_t& result)
{
    boost::gil::tbb_transform_pixels( a, b, result, detail::lerp_pixels( t));
}

} // namespace
} // namespace
} // namespace
