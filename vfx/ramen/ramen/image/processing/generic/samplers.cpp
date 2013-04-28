// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/samplers.hpp>

#include<cmath>

namespace ramen
{
namespace image
{
namespace generic
{

sampler_t::sampler_t( const const_image_view_t& src ) : src_(src)
{
    src_area_ = Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( src.width() - 1, src.height() - 1));
}

sampler_t::sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src ) : src_area_(src_area), src_(src) {}

point_sampler_t::point_sampler_t( const const_image_view_t& src) : sampler_t( src) {}
point_sampler_t::point_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : sampler_t( src_area, src) {}

pixel_t point_sampler_t::operator()( const Imath::V2d& p) const
{
    Imath::V2i pi( IECore::fastFloat2Int( p.x + ( p.x < 0.0f ? -0.5f : 0.5f)), IECore::fastFloat2Int( p.y + ( p.y < 0.0f ? -0.5f : 0.5f)));

    if( src_area_.intersects( pi))
	return src_( pi.x - src_area_.min.x, pi.y - src_area_.min.y);
    else
	return pixel_t( 0, 0, 0, 0);
}

bilinear_sampler_t::bilinear_sampler_t( const const_image_view_t& src) : sampler_t( src) {}
bilinear_sampler_t::bilinear_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : sampler_t( src_area, src)
{
    const_image_view_t::xy_locator src_loc = src_.xy_at(0,0);
    tr_ = src_loc.cache_location( 1, 0);
    bl_ = src_loc.cache_location( 0, 1);
    br_ = src_loc.cache_location( 1, 1);
}

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

    const_image_view_t::xy_locator src_loc( src_.xy_at( x - src_area_.min.x, y - src_area_.min.y));

    pixel_t a( *src_loc);
    pixel_t b( src_loc[tr_]);

    pixel_t top( (a[0]*it) + (b[0]*t), (a[1]*it) + (b[1]*t), (a[2]*it) + (b[2]*t), (a[3]*it) + (b[3]*t));

    a = src_loc[bl_];
    b = src_loc[br_];

    pixel_t bot( (a[0]*it + b[0]*t), (a[1]*it) + (b[1]*t), (a[2]*it) + (b[2]*t), (a[3]*it) + (b[3]*t));

    return pixel_t( (top[0]*is) + (bot[0]*s), (top[1]*is) + (bot[1]*s), (top[2]*is) + (bot[2]*s), (top[3]*is) + (bot[3]*s));
}

bicubic_sampler_t::bicubic_sampler_t( const const_image_view_t& src) : bilinear_sampler_t( src) {}
bicubic_sampler_t::bicubic_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : bilinear_sampler_t( src_area, src) {}

pixel_t bicubic_sampler_t::operator()( const Imath::V2d& p) const
{
    if( !is_inside( p))
        return pixel_t( 0, 0, 0, 0);

    return sample( p);
}

pixel_t bicubic_sampler_t::sample( const Imath::V2d& p) const
{
    int x = IECore::fastFloatFloor( p.x);
    int y = IECore::fastFloatFloor( p.y);

    if( ( x < src_area_.min.x + 1) || ( x > src_area_.max.x - 2) || ( y < src_area_.min.y + 1) || ( y > src_area_.max.y - 2))
        return bilinear_sampler_t::sample( p);

    float fractx = p.x - x;
    float fracty = p.y - y;

    float xweights[4], yweights[4];

    weights( fractx, xweights);
    weights( fracty, yweights);

    image::pixel_t result( 0, 0, 0, 0);

    for( int j = -1; j <= 2; ++j)
    {
	image::const_image_view_t::x_iterator src_it( src_.row_begin( y - src_area_.min.y + j));
	src_it += x - src_area_.min.x - 1;

	image::pixel_t row;

	for( int c = 0; c < 4; ++c)
	{
	    row[c]  = src_it[0][c] * xweights[0];
	    row[c] += src_it[1][c] * xweights[1];
	    row[c] += src_it[2][c] * xweights[2];
	    row[c] += src_it[3][c] * xweights[3];
	    result[c] += row[c] * yweights[j+1];
	}
    }

    return result;
}

void bicubic_sampler_t::weights( float t, float w[4]) const
{
    float t2 = t * t, t3 = t2 * t;
    w[0] = ( -t3 + t2 + t2 - t) * 0.5f;
    w[1] = ( 3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
    w[2] = ( -3.0f * t3 + 4.0f * t2 + t) * 0.5f;
    w[3] = ( t3 - t2) * 0.5f;
}

// non black border modes
bilinear_tile_sampler_t::bilinear_tile_sampler_t( const const_image_view_t& src) : bilinear_sampler_t( src) {}
bilinear_tile_sampler_t::bilinear_tile_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : bilinear_sampler_t( src_area, src) {}
pixel_t bilinear_tile_sampler_t::operator()( const Imath::V2d& p) const { return sample( tile_point( p));}

bilinear_mirror_sampler_t::bilinear_mirror_sampler_t( const const_image_view_t& src) : bilinear_sampler_t( src) {}
bilinear_mirror_sampler_t::bilinear_mirror_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src) : bilinear_sampler_t( src_area, src) {}
pixel_t bilinear_mirror_sampler_t::operator()( const Imath::V2d& p) const { return sample( mirror_point( p));}

} // namespace
} // namespace
} // namespace
