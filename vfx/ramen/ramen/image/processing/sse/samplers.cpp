// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/sse/samplers.hpp>

#include<cmath>

#include<xmmintrin.h>

namespace ramen
{
namespace image
{
namespace sse
{

bilinear_sampler_t::bilinear_sampler_t( const const_image_view_t& src) : generic::bilinear_sampler_t( src) {}
bilinear_sampler_t::bilinear_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : generic::bilinear_sampler_t( src_area, src) {}

pixel_t bilinear_sampler_t::operator()( const Imath::V2d& p) const
{
    if( !is_inside( p))
        return pixel_t( 0, 0, 0, 0);

    return sample( p);
}

pixel_t bilinear_sampler_t::sample( const Imath::V2d& p) const
{
    int x = std::max( std::min( IECore::fastFloatFloor( p.x), src_area_.max.x - 1), src_area_.min.x);
    int y = std::max( std::min( IECore::fastFloatFloor( p.y), src_area_.max.y - 1), src_area_.min.y);
    return sample( x, y, p.x - x, p.y - y);
}

pixel_t bilinear_sampler_t::sample( int x, int y, float t, float s) const
{
    float it = 1.0f - t;
    float is = 1.0f - s;

    __m128 tx = _mm_set_ps( t, t, t, t);
    __m128 ix = _mm_set_ps( it, it, it, it);

    const_image_view_t::xy_locator src_loc( src_.xy_at( x - src_area_.min.x, y - src_area_.min.y));

    __m128 a = _mm_load_ps( reinterpret_cast<const float*>( &*src_loc));
    __m128 b = _mm_load_ps( reinterpret_cast<const float*>( &src_loc[tr_]));
    __m128 top = _mm_add_ps( _mm_mul_ps( a, ix), _mm_mul_ps( b, tx));

    a = _mm_load_ps( reinterpret_cast<const float*>( &src_loc[bl_]));
    b = _mm_load_ps( reinterpret_cast<const float*>( &src_loc[br_]));
    __m128 bot = _mm_add_ps( _mm_mul_ps( a, ix), _mm_mul_ps( b, tx));

    tx = _mm_set_ps( s, s, s, s);
    ix = _mm_set_ps( is, is, is, is);
    __m128 r = _mm_add_ps( _mm_mul_ps( top, ix), _mm_mul_ps( bot, tx));

    float *q = reinterpret_cast<float*>( &r);
    return pixel_t( q[0], q[1], q[2], q[3]);
}

bicubic_sampler_t::bicubic_sampler_t( const const_image_view_t& src) : generic::bicubic_sampler_t( src) {}
bicubic_sampler_t::bicubic_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : generic::bicubic_sampler_t( src_area, src) {}

pixel_t bicubic_sampler_t::operator()( const Imath::V2d& p) const
{
    if( !is_inside( p))
        return pixel_t( 0, 0, 0, 0);

    return sample( p);
}

pixel_t bicubic_sampler_t::sample(const Imath::V2d& p) const
{
    int x = IECore::fastFloatFloor( p.x);
    int y = IECore::fastFloatFloor( p.y);

    if( ( x < src_area_.min.x + 1) || ( x > src_area_.max.x - 2) || ( y < src_area_.min.y + 1) || ( y > src_area_.max.y - 2))
        return generic::bilinear_sampler_t::sample( p);

    float fractx = p.x - x;
    float fracty = p.y - y;

    float xweights[4], yweights[4];

    weights( fractx, xweights);
    weights( fracty, yweights);

    __m128 result = _mm_set_ps( 0, 0, 0, 0);

    for( int j = -1; j <= 2; ++j)
    {
	image::const_image_view_t::x_iterator src_it( src_.row_begin( y - src_area_.min.y + j));
	src_it += x - src_area_.min.x - 1;

	__m128 yw = _mm_set_ps( yweights[j+1], yweights[j+1], yweights[j+1], yweights[j+1]);
	__m128 row = _mm_set_ps( 0, 0, 0, 0);

	for( int i = 0; i < 4; ++i)
	{
	    __m128 xw = _mm_set_ps( xweights[i], xweights[i], xweights[i], xweights[i]);
	    __m128 pix = _mm_load_ps( reinterpret_cast<const float*>( &(*src_it)));
	    row = _mm_add_ps( row, _mm_mul_ps( pix, xw));
	    ++src_it;
	}

	result = _mm_add_ps( result, _mm_mul_ps( row, yw));
    }

    float *q = reinterpret_cast<float*>( &result);
    return pixel_t( q[0], q[1], q[2], q[3]);
}


// non black border modes
bilinear_tile_sampler_t::bilinear_tile_sampler_t( const const_image_view_t& src) : sse::bilinear_sampler_t( src) {}
bilinear_tile_sampler_t::bilinear_tile_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : sse::bilinear_sampler_t( src_area, src) {}
pixel_t bilinear_tile_sampler_t::operator()( const Imath::V2d& p) const { return sample( tile_point( p));}

bilinear_mirror_sampler_t::bilinear_mirror_sampler_t( const const_image_view_t& src) : sse::bilinear_sampler_t( src) {}
bilinear_mirror_sampler_t::bilinear_mirror_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : sse::bilinear_sampler_t( src_area, src) {}
pixel_t bilinear_mirror_sampler_t::operator()( const Imath::V2d& p) const { return sample( mirror_point( p));}

} // namespace
} // namespace
} // namespace
