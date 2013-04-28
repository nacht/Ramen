// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/processing/generic/key_mix.hpp>

namespace ramen
{
namespace image
{
namespace generic
{

namespace detail
{

struct key_mix_fun
{
    image::pixel_t operator()( const image::pixel_t& bg, const image::pixel_t& fg, const image::channel_pixel_t& key) const
    {
        return image::pixel_t(  lerp( fg[0], bg[0], key[0]),
                                lerp( fg[1], bg[1], key[0]),
                                lerp( fg[2], bg[2], key[0]),
                                lerp( fg[3], bg[3], key[0]));
    }

    float lerp( float a, float b, float t) const { return (a * t) + ((1.0f - t) * b);}
};

} // detail

void key_mix( const const_image_view_t& bg, const const_image_view_t& fg, const const_channel_view_t& key, const image_view_t& dst)
{
    detail::key_mix( bg, fg, key, dst, detail::key_mix_fun());
}

} // namespace
} // namespace
} // namespace
