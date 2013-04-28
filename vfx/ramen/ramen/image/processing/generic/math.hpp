// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_GENERIC_MATH_HPP
#define	RAMEN_IMAGE_GENERIC_MATH_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

// images
void clear_image( const image_view_t& v);
void add_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result);
void sub_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result);
void mul_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result);
void mul_image_scalar( const const_image_view_t& a, float f, const image_view_t& result);
void madd_images( const const_image_view_t& a, const const_image_view_t& b, float f, const image_view_t& result);
void lerp_images( const const_image_view_t& a, const const_image_view_t& b, float t, const image_view_t& result);

void clear_gray_image( const gray_image_view_t& v);
void add_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result);
void sub_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result);
void mul_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, const gray_image_view_t& result);
void mul_gray_image_scalar( const const_gray_image_view_t& a, float f, const gray_image_view_t& result);
void madd_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, float f, const gray_image_view_t& result);
void lerp_gray_images( const const_gray_image_view_t& a, const const_gray_image_view_t& b, float t, const gray_image_view_t& result);

// channels
void clear_channel( const channel_view_t& v);
void add_channels( const const_channel_view_t& a, const const_channel_view_t& b, const channel_view_t& result);
void sub_channels( const const_channel_view_t& a, const const_channel_view_t& b, const channel_view_t& result);
void mul_channels( const const_channel_view_t& a, const const_channel_view_t& b, const channel_view_t& result);
void madd_channels( const const_channel_view_t& a, const const_channel_view_t& b, float f, const channel_view_t& result);

} // namespace
} // namespace
} // namespace

#endif
