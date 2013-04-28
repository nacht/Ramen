// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE_SAMPLERS_HPP
#define	RAMEN_IMAGE_SSE_SAMPLERS_HPP

#include<ramen/image/processing/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

class bilinear_sampler_t : public generic::bilinear_sampler_t
{
public:

    bilinear_sampler_t( const const_image_view_t& src);
    bilinear_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const Imath::V2d& p) const;
    pixel_t sample( const Imath::V2d& p) const;
    pixel_t sample( int x, int y, float t, float s) const;
};

class bicubic_sampler_t : public generic::bicubic_sampler_t
{
public:

    bicubic_sampler_t( const const_image_view_t& src);
    bicubic_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);

    pixel_t operator()( const Imath::V2d& p) const;
    pixel_t sample( const Imath::V2d& p) const;
};

// non black border modes

class bilinear_tile_sampler_t : public sse::bilinear_sampler_t
{
public:

    bilinear_tile_sampler_t( const const_image_view_t& src);
    bilinear_tile_sampler_t( const Imath::Box2i& src_area, const const_image_view_t& src);
    pixel_t operator()( const Imath::V2d& p) const;
};

class bilinear_mirror_sampler_t : public sse::bilinear_sampler_t
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

