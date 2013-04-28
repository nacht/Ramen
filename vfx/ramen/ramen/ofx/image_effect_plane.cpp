// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ofx/image_effect_plane.hpp>

#include<string.h>

#include<ramen/ofx/host.hpp>

namespace ramen
{
namespace ofx
{

OfxStatus clip_get_image_plane_v1( OfxImageClipHandle clip, OfxTime time, const char *plane, OfxRectD *region, OfxPropertySetHandle *imageHandle)
{
    if( strcmp( plane, kFnOfxImagePlaneColour) == 0)
    {
	OfxImageEffectSuiteV1 *img_suite = reinterpret_cast<OfxImageEffectSuiteV1*>( host_t::Instance().fetchSuite( kOfxImageEffectSuite, 1));
	return img_suite->clipGetImage( clip, time, region, imageHandle);
	return kOfxStatOK;
    }
    else
	return kOfxStatFailed;
}

OfxStatus clip_get_image_plane_v2( OfxImageClipHandle clip, OfxTime time, int view, const char *plane, OfxRectD *region, OfxPropertySetHandle *imageHandle)
{
    return kOfxStatFailed;
}

OfxStatus clip_get_region_of_definition( OfxImageClipHandle clip, OfxTime time, int view, OfxRectD *bounds)
{
    return kOfxStatFailed;
}

OfxStatus get_view_name( OfxImageEffectHandle effect, int view, char **viewName)
{
    return kOfxStatFailed;
}

OfxStatus get_view_count( OfxImageEffectHandle effect, int *nViews)
{
    return -1;
}


} // namespace
} // namespace
