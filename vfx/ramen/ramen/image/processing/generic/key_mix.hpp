// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_PROCESSING_GENERIC_KEY_MIX_HPP
#define RAMEN_IMAGE_PROCESSING_GENERIC_KEY_MIX_HPP

#include<gil/extension/algorithm/tbb/tbb_transform3.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

template<class Fun>
void key_mix(  const const_image_view_t& bg, const const_image_view_t& fg, const const_channel_view_t& key, const image_view_t& dst, Fun f)
{
    boost::gil::tbb_transform_pixels( bg, fg, key, dst, f);
}

} // detail

void key_mix( const const_image_view_t& bg, const const_image_view_t& fg, const const_channel_view_t& key, const image_view_t& dst);

} // namespace
} // namespace
} // namespace

#endif
