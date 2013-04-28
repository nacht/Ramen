// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/anim/curve_to_half_lut.hpp>

#include<boost/bind.hpp>

namespace ramen
{
namespace anim
{

void curve_to_half_lut( const curve_t& c, half_lut_t& lut)
{
    lut.init( boost::bind( &curve_t::evaluate, &c, _1));
}

} // namespace
} // namespace
