// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_OFX_IMAGE_EFFECT_PLANE_HPP
#define	RAMEN_OFX_IMAGE_EFFECT_PLANE_HPP

#include"ofxCore.h"
#include"fnOfxExtensions.h"

namespace ramen
{
namespace ofx
{

OfxStatus clip_get_image_plane_v1( OfxImageClipHandle clip, OfxTime time, const char *plane, OfxRectD *region, OfxPropertySetHandle *imageHandle);

OfxStatus clip_get_image_plane_v2( OfxImageClipHandle clip, OfxTime time, int view, const char *plane, OfxRectD *region, OfxPropertySetHandle *imageHandle);
OfxStatus clip_get_region_of_definition( OfxImageClipHandle clip, OfxTime time, int view, OfxRectD *bounds);
OfxStatus get_view_name( OfxImageEffectHandle effect, int view, char **viewName);
OfxStatus get_view_count( OfxImageEffectHandle effect, int *nViews);

} // namespace
} // namespace

#endif
