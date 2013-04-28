// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGE_TYPEDEFS_HPP
#define RAMEN_IMAGE_TYPEDEFS_HPP

#include<boost/gil/gil_all.hpp>

#include<gil/extension/half.hpp>

namespace ramen
{
namespace image
{

typedef boost::gil::bits32f	channel_t;
typedef channel_t*		channel_ptr_t;
typedef const channel_t*	const_channel_ptr_t;

typedef boost::gil::rgba_layout_t layout_t;
typedef boost::gil::gray_layout_t gray_layout_t;

typedef boost::gil::pixel<channel_t, layout_t>		pixel_t;
typedef boost::gil::pixel<channel_t, gray_layout_t>	gray_pixel_t;

typedef const boost::gil::pixel<channel_t, layout_t>        const_pixel_t;
typedef const boost::gil::pixel<channel_t, gray_layout_t>   const_gray_pixel_t;

typedef boost::gil::pixel<channel_t, layout_t>&		pixel_ref_t;
typedef boost::gil::pixel<channel_t, gray_layout_t>&	gray_pixel_ref_t;

typedef const boost::gil::pixel<channel_t, layout_t>&       const_pixel_ref_t;
typedef const boost::gil::pixel<channel_t, gray_layout_t>&  const_gray_pixel_ref_t;

typedef boost::gil::pixel<channel_t, layout_t>*		pixel_ptr_t;
typedef boost::gil::pixel<channel_t, gray_layout_t>*	gray_pixel_ptr_t;

typedef const boost::gil::pixel<channel_t, layout_t>*       const_pixel_ptr_t;
typedef const boost::gil::pixel<channel_t, gray_layout_t>*  const_gray_pixel_ptr_t;

typedef boost::gil::memory_based_step_iterator<pixel_ptr_t>		pixel_step_ptr_t;
typedef boost::gil::memory_based_step_iterator<gray_pixel_ptr_t>	gray_pixel_step_ptr_t;

typedef boost::gil::memory_based_step_iterator<const_pixel_ptr_t>	const_pixel_step_ptr_t;
typedef boost::gil::memory_based_step_iterator<const_gray_pixel_ptr_t>	const_gray_pixel_step_ptr_t;

typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<pixel_ptr_t> >	locator_t;
typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<gray_pixel_ptr_t> >	gray_locator_t;

typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<const_pixel_ptr_t> >         const_locator_t;
typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<const_gray_pixel_ptr_t> >	const_gray_locator_t;

typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<pixel_step_ptr_t> >		step_locator_t;
typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<gray_pixel_step_ptr_t> >	gray_step_locator_t;

typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<const_pixel_step_ptr_t> >        const_step_locator_t;
typedef boost::gil::memory_based_2d_locator<boost::gil::memory_based_step_iterator<const_gray_pixel_step_ptr_t> >   const_gray_step_locator_t;

typedef boost::gil::image_view<locator_t>	image_view_t;
typedef boost::gil::image_view<gray_locator_t>	gray_image_view_t;

typedef boost::gil::image_view<const_locator_t>         const_image_view_t;
typedef boost::gil::image_view<const_gray_locator_t>	const_gray_image_view_t;

typedef boost::gil::image_view<step_locator_t>		image_step_view_t;
typedef boost::gil::image_view<gray_step_locator_t>	gray_image_step_view_t;

typedef boost::gil::image_view<const_step_locator_t>        const_image_step_view_t;
typedef boost::gil::image_view<const_gray_step_locator_t>   const_gray_image_step_view_t;

// channel views
typedef gray_layout_t       channel_layout_t;
typedef gray_pixel_t        channel_pixel_t;
typedef const_gray_pixel_t  const_channel_pixel_t;

typedef boost::gil::nth_channel_view_type<image_view_t>::type		channel_view_t;
typedef boost::gil::nth_channel_view_type<const_image_view_t>::type	const_channel_view_t;

// images
typedef boost::gil::image<pixel_t, false> image_t;
typedef boost::gil::image<gray_pixel_t, false> gray_image_t;

} // namespace
} // namespace

#endif
