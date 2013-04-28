// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/ui/palette.hpp>

namespace ramen
{
namespace ui
{

palette_impl::palette_impl()
{
    colors_["background"]   = Imath::Color3c( 102, 102, 102);

    colors_["front plug"]   = Imath::Color3c( 255, 0, 0);
    colors_["back plug"]    = Imath::Color3c( 0, 255, 0);
    colors_["matte plug"]   = Imath::Color3c( 0, 0, 255);
    colors_["out plug"]	    = Imath::Color3c( 255, 255, 0);
}

palette_impl::~palette_impl() {}

const Imath::Color3c& palette_impl::operator[]( const std::string& k) const
{
    std::map<std::string, Imath::Color3c>::const_iterator it;
    it = colors_.find( k);

    // TODO: check for end iter and throw exception here

    return it->second;
}

} // namespace
} // namespace
