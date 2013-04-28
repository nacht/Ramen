// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_SSE_RESIZE_HPP
#define RAMEN_IMAGE_SSE_RESIZE_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

void resize_bilinear( const const_image_view_t& src, const image_view_t& dst);
void resize_bicubic( const const_image_view_t& src, const image_view_t& dst);

} // namespace
} // namespace
} // namespace

#endif

