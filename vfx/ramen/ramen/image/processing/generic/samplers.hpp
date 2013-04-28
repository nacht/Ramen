// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_SAMPLERS_HPP
#define	RAMEN_IMAGE_GENERIC_SAMPLERS_HPP

#include<OpenEXR/ImathBox.h>

#include<cmath>

#include<algorithm>

#include<boost/scoped_array.hpp>

#include<adobe/algorithm/clamp.hpp>

#include<IECore/FastFloat.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

class sampler_t
{
public:

    sampler_t( const const_image_view_t& src);
    sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

protected:

    bool is_inside( const Imath::V2d& p) const
    {
        Imath::V2i pi( IECore::fastFloat2Int( p.x + ( p.x < 0.0f ? -0.5f : 0.5f)), IECore::fastFloat2Int( p.y + ( p.y < 0.0f ? -0.5f : 0.5f)));
        return src_area_.intersects( pi);
    }

    Imath::V2d clamp_point( const Imath::V2d& p) const
    {
        return Imath::V2d( adobe::clamp( p.x, (double) src_area_.min.x, (double) src_area_.max.x),
                           adobe::clamp( p.y, (double) src_area_.min.y, (double) src_area_.max.y));
    }

    Imath::V2d tile_point( const Imath::V2d& p) const
    {
        Imath::V2i size( src_area_.size());

        double ix, iy;
        double x = std::modf( (double) ( p.x - src_area_.min.x) / size.x, &ix);
        double y = std::modf( (double) ( p.y - src_area_.min.y) / size.y, &iy);

        if( x < 0) x = 1.0 + x;
        if( y < 0) y = 1.0 + y;

        return Imath::V2d( x * size.x + src_area_.min.x, y * size.y + src_area_.min.y);
    }

    Imath::V2d mirror_point( const Imath::V2d& p) const
    {
        Imath::V2i size( src_area_.size());

        double ix, iy;
        double x = std::modf( std::fabs( (double) ( p.x - src_area_.min.x) / size.x), &ix);
        double y = std::modf( std::fabs( (double) ( p.y - src_area_.min.y) / size.y), &iy);

        if( (int) ix & 1) x = 1.0f - x;
        if( (int) iy & 1) y = 1.0f - y;

        return Imath::V2d( x * size.x + src_area_.min.x, y * size.y + src_area_.min.y);
    }

    Imath::Box2i src_area_;
    const const_image_view_t& src_;
};

class point_sampler_t : public sampler_t
{
public:

    point_sampler_t( const const_image_view_t& src);
    point_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const Imath::V2d& p) const;
};

class bilinear_sampler_t : public sampler_t
{
public:

    bilinear_sampler_t( const const_image_view_t& src);
    bilinear_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const Imath::V2d& p) const;
    pixel_t sample( const Imath::V2d& p) const;

    pixel_t sample( int x, int y, float t, float s) const;

protected:

    const_image_view_t::xy_locator::cached_location_t tr_, bl_, br_;
};

class bicubic_sampler_t : public bilinear_sampler_t
{
public:

    bicubic_sampler_t( const const_image_view_t& src);
    bicubic_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const Imath::V2d& p) const;
    pixel_t sample( const Imath::V2d& p) const;

protected:

    void weights( float t, float w[4]) const;
};

// non black border modes
class bilinear_tile_sampler_t : public bilinear_sampler_t
{
public:

    bilinear_tile_sampler_t( const const_image_view_t& src);
    bilinear_tile_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);
    pixel_t operator()( const Imath::V2d& p) const;
};

class bilinear_mirror_sampler_t : public bilinear_sampler_t
{
public:

    bilinear_mirror_sampler_t( const const_image_view_t& src);
    bilinear_mirror_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);
    pixel_t operator()( const Imath::V2d& p) const;
};

} // namespace
} // namespace
} // namespace

#endif
