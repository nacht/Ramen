// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/image/channel_range.hpp>

#include<adobe/algorithm/clamp.hpp>

namespace ramen
{
namespace image
{

float channel_range_t::operator()( float x) const
{
    float a, b;

    if( x >= center)
    {
        if( x <= center + tol_hi)
            return 1.0f;

        a = center + tol_hi;
        b = a + soft_hi;

        if( x > b)
            return 0.0f;

        if( a == b)
            return 0.0f;

        return 1.0f - adobe::clamp( ( x - a) / ( b - a), 0.0f, 1.0f);
    }
    else
    {
        if( x > center - tol_lo)
            return 1.0f;

        b = center - tol_lo;
        a = b - soft_lo;

        if( x < a)
            return 0.0f;

        if( a == b)
            return 0.0f;

        return adobe::clamp( ( x - a) / ( b - a), 0.0f, 1.0f);
    }
}

} // namespace
} // namespace
