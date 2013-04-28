// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/resize.hpp>

#include<ramen/image/processing/generic/samplers.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void resize_nearest( const const_image_view_t& src, const image_view_t& dst)
{
    resize<point_sampler_t>( src, dst);
}

void resize_bilinear( const const_image_view_t& src, const image_view_t& dst)
{
    resize<bilinear_sampler_t>( src, dst);
}

void resize_bicubic( const const_image_view_t& src, const image_view_t& dst)
{
    resize<bicubic_sampler_t>( src, dst);
}

} // namespace
} // namespace
} // namespace
