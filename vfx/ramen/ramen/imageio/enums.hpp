// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_ENUMS_HPP
#define RAMEN_IMAGEIO_ENUMS_HPP

namespace ramen
{
namespace imageio
{

enum channel_type
{
    half_channel = 0,
    float_channel,
    ubyte_channel,
    ushort_channel
};

enum channels
{
    rgba_channels = 0,
    rgb_channels,
    alpha_channel
};

enum compression_type
{
    none_comp = 0,
    rle_comp,
    zips_comp,
    zip_comp,
    piz_comp,
    pxr24_comp,
    b44_comp,
    b44a_comp
};

enum linearity
{
    linear = 0,
    gamma_corrected,
    srgb
};

} // namespace
} // namespace

#endif
