// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/init_image_processing.hpp>

#include<ramen/system/system.hpp>
#include<ramen/image/processing.hpp>

#ifdef RAMEN_SSE_SUPPORT
    #include<ramen/image/processing/sse/composite.hpp>
    #include<ramen/image/processing/sse/affine_warp.hpp>
    #include<ramen/image/processing/sse/key_mix.hpp>
    #include<ramen/image/processing/sse/math.hpp>
    #include<ramen/image/processing/sse/projective_warp.hpp>
    #include<ramen/image/processing/sse/resize.hpp>
#endif

namespace ramen
{
namespace image
{

void init_image_processing()
{
    #ifdef RAMEN_SSE_SUPPORT
	int simd = system::system_t::Instance().simd_type();

	if( simd & system::simd_sse)
	{
	    // replace function pointers here for sse versions

            // math
            add_images          = sse::add_images;
            sub_images          = sse::sub_images;
            mul_images          = sse::mul_images;
            mul_image_scalar    = sse::mul_image_scalar;
            madd_images         = sse::madd_images;
            lerp_images         = sse::lerp_images;

	    // warp / transform
	    affine_warp_bilinear = sse::affine_warp_bilinear;
	    affine_warp_bicubic  = sse::affine_warp_bicubic;

	    affine_warp_bilinear_tile   = sse::affine_warp_bilinear_tile;
	    affine_warp_bilinear_mirror = sse::affine_warp_bilinear_mirror;

            projective_warp_bilinear = sse::projective_warp_bilinear;
	    projective_warp_bicubic  = sse::projective_warp_bicubic;

            // resize
            resize_bilinear = sse::resize_bilinear;
            resize_bicubic  = sse::resize_bicubic;

	    // layer
	    composite_over  = sse::composite_over;
	    composite_add   = sse::composite_add;
	    composite_mul   = sse::composite_mul;
	    composite_sub   = sse::composite_sub;
	    composite_mix   = sse::composite_mix;
	    composite_max   = sse::composite_max;
	    composite_min   = sse::composite_min;

            // key_mix
            key_mix = sse::key_mix;
	}
    #endif
}

} // namespace
} // namespace
