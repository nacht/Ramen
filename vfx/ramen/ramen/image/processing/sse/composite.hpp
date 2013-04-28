// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE_COMPOSITE_HPP
#define	RAMEN_IMAGE_SSE_COMPOSITE_HPP

#include<ramen/image/processing/generic/composite.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

void composite_over( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_add( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_mul( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_sub( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);

void composite_mix( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);

void composite_max( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_min( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);

} // namespace
} // namespace
} // namespace

#endif
