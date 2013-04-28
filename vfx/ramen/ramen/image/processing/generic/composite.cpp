// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/composite.hpp>

#include<gil/extension/algorithm/tbb/tbb_transform.hpp>

#include<ramen/image/processing/generic/layer_modes.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void composite_over( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, over_layer_mode_fun( opacity));
}

void composite_add( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, add_layer_mode_fun( opacity));
}

void composite_mul( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, mul_layer_mode_fun( opacity));
}

void composite_sub( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, sub_layer_mode_fun( opacity));
}

void composite_mix( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, mix_layer_mode_fun( opacity));
}

void composite_screen( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, screen_layer_mode_fun( opacity));
}

void composite_overlay( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, overlay_layer_mode_fun( opacity));
}

void composite_zero_overlay( const const_image_view_t& back, const image_view_t& dst, float opacity)
{
    boost::gil::tbb_transform_pixels( back, dst, zero_overlay_layer_mode_fun( opacity));
}

void composite_diff( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, diff_layer_mode_fun( opacity));
}

void composite_max( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, max_layer_mode_fun( opacity));
}

void composite_min( const const_image_view_t& back, const const_image_view_t& front, const image_view_t& dst, float opacity)
{
    composite( back, front, dst, min_layer_mode_fun( opacity));
}

} // namespace
} // namespace
} // namespace
