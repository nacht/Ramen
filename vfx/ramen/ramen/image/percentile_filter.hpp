// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_PERCENTILE_FILTER_HPP
#define	RAMEN_IMAGE_PERCENTILE_FILTER_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void percentile_filter_channel( const const_channel_view_t& src, const channel_view_t& dst, int radius, float percent);

void percentile_filter_rgba( const const_image_view_t& src, const image_view_t& dst, int radius, float percent);
void percentile_filter_rgb( const const_image_view_t& src, const image_view_t& dst, int radius, float percent);
void percentile_filter_alpha( const const_image_view_t& src, const image_view_t& dst, int radius, float percent);

void dilate( const const_gray_image_view_t& src, const gray_image_view_t& dst, float radius);

} // image
} // ramen

#endif
