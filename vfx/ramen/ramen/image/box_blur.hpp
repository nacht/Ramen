// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_BOX_BLUR_HPP
#define RAMEN_IMAGE_BOX_BLUR_HPP

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{

void box_blur_rgba( const const_image_view_t& src, const image_view_t& dst, float hradius, float vradius, int iters = 1);

} // namespace
} // namespace

#endif
