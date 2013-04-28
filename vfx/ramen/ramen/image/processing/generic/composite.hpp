// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_COMPOSITE_HPP
#define	RAMEN_IMAGE_GENERIC_COMPOSITE_HPP

#include<gil/extension/algorithm/tbb/tbb_transform2.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

template<class LayerModeFun>
void composite( const const_image_view_t& back, const const_image_view_t& front,
	       const image_view_t& dst, LayerModeFun f)
{
    boost::gil::tbb_transform_pixels( back, front, dst, f);
}

void composite_over( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_add( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_mul( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_sub( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_mix( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_screen( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_overlay( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_zero_overlay( const const_image_view_t& back, const image_view_t& dst, float opacity);
void composite_diff( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_max( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);
void composite_min( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity);

} // namespace
} // namespace
} // namespace

#endif
