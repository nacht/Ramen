// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE_MATH_HPP
#define	RAMEN_IMAGE_SSE_MATH_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

void add_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result);
void sub_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result);
void mul_images( const const_image_view_t& a, const const_image_view_t& b, const image_view_t& result);
void mul_image_scalar( const const_image_view_t& a, float f, const image_view_t& result);
void madd_images( const const_image_view_t& a, const const_image_view_t& b, float f, const image_view_t& result);
void lerp_images( const const_image_view_t& a, const const_image_view_t& b, float t, const image_view_t& result);

} // namespace
} // namespace
} // namespace

#endif
