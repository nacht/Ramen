// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the MPL License.
// See LICENSE.txt for a copy of the license

#ifndef RAMEN_IMAGE_GENERIC_WARP_HPP
#define RAMEN_IMAGE_GENERIC_WARP_HPP

#include<boost/function.hpp>

#include<OpenEXR/ImathBox.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

void warp_bilinear( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const boost::function<Imath::V2f ( const Imath::V2f&)>& wfun);

void warp_bicubic( const Imath::Box2i& src_area, const const_image_view_t& src,
				const Imath::Box2i& dst_area, const image_view_t& dst,
				const boost::function<Imath::V2f ( const Imath::V2f&)>& wfun);
} // namespace
} // namespace
} // namespace

#endif
