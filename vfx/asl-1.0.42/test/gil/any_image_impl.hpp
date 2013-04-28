/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://stlab.adobe.com/licenses.html)
*/

#ifndef GIL_ANY_IMAGE_IMPL_HPP
#define GIL_ANY_IMAGE_IMPL_HPP

#ifdef NO_ASL
#include <gil/core/typedefs.hpp>
#include <gil/extension/dynamic_image/any_image_factory.hpp>
#include <gil/extension/dynamic_image/any_image.hpp>
#else
#include <adobe/gil/core/typedefs.hpp>
#include <adobe/gil/extension/dynamic_image/any_image_factory.hpp>
#include <adobe/gil/extension/dynamic_image/any_image.hpp>
#endif

using namespace GIL;

///////////////////////////////////////////////////////////////////////////////////
///
/// Define the scope of a run-time images and image views
///
///////////////////////////////////////////////////////////////////////////////////

// This is how to define any_image_t as a simple enumeration of images
//typedef any_image<boost::mpl::vector<gray8_image_t, rgb8_image_t, rgb8_planar_image_t, rgba8_image_t> > any_image_t;

// This is how to define it as a cross-product of types
//typedef boost::mpl::vector<bits8, bits16> all_channels_vec_t;
typedef boost::mpl::vector<bits8> all_channels_vec_t;
//typedef any_image<cross_vector_image_types<all_channels_vec_t, boost::mpl::vector<gray_t, bgr_t>,kInterleavedOnly>::type> any_image_t;

// This is how to define it as a union or other types (which could be enumerations, cross products or other unions)
typedef boost::mpl::concat_vector <
   cross_vector_image_types<all_channels_vec_t, boost::mpl::vector<gray_t, bgr_t, argb_t>,     kInterleavedOnly     >::type,
   cross_vector_image_types<all_channels_vec_t, boost::mpl::vector<rgb_t, cmyk_t, rgba_t>,     kInterleavedAndPlanar>::type
   > any_image_types;
typedef any_image<any_image_types>             any_image_t;



// This is how to define any_image_view. Normally you don't need to do that. You can define any_image_t and then use any_image_t::view_t
/*
typedef boost::mpl::concat_vector<
   cross_vector_image_view_types<all_channels_vec_t, boost::mpl::vector<gray_t, bgr_t, argb_t>, kInterleavedOnly,      kNonStepAndStep, true>::type,
   cross_vector_image_view_types<all_channels_vec_t, boost::mpl::vector<rgb_t, cmyk_t, rgba_t>, kInterleavedAndPlanar, kNonStepAndStep, true>::type> any_image_view_types;
typedef any_image_view<any_image_view_types> any_image_view_t;
*/
#endif
