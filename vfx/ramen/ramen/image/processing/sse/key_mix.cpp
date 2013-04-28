// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/sse/key_mix.hpp>

#include<xmmintrin.h>
#include<emmintrin.h>

namespace ramen
{
namespace image
{
namespace sse
{

namespace detail
{

struct sse_key_mix_fun
{
    image::pixel_t operator()( const image::pixel_t& bg, const image::pixel_t& fg, const image::channel_pixel_t& key) const
    {
        __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &bg));
        __m128 f = _mm_load_ps( reinterpret_cast<const float*>( &fg));

        __m128 a  = _mm_set_ps( key[0], key[0], key[0], key[0]);
        __m128 ai = _mm_set_ps( 1.0f - key[0], 1.0f - key[0], 1.0f - key[0], 1.0f - key[0]);

        f = _mm_mul_ps( f, a);
        b = _mm_mul_ps( b, ai);
        f = _mm_add_ps( f, b);

        float *p = reinterpret_cast<float*>( &f);
        return pixel_t( p[0], p[1], p[2], p[3]);
    }
};

} // detail

void key_mix( const const_image_view_t& bg, const const_image_view_t& fg, const const_channel_view_t& key, const image_view_t& dst)
{
    generic::detail::key_mix( bg, fg, key, dst, detail::sse_key_mix_fun());
}

} // namespace
} // namespace
} // namespace
