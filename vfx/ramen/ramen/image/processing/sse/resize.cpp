// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/sse/resize.hpp>

#include<ramen/image/processing/generic/resize.hpp>

#include<ramen/image/processing/sse/samplers.hpp>

namespace ramen
{
namespace image
{
namespace sse
{

void resize_bilinear( const const_image_view_t& src, const image_view_t& dst)
{
    generic::resize<sse::bilinear_sampler_t>( src, dst);
}

void resize_bicubic( const const_image_view_t& src, const image_view_t& dst)
{
    generic::resize<sse::bicubic_sampler_t>( src, dst);
}

} // namespace
} // namespace
} // namespace
