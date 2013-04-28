
// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
#include<ramen/util/format.hpp>

namespace ramen
{

format_t::format_t() : width(768), height(576) {}

format_t::format_t( std::size_t w, std::size_t h) : width(w), height(h) {}

Imath::Box2i format_t::area() const { return Imath::Box2i( Imath::V2i( 0, 0), Imath::V2i( width, height));}

bool format_t::operator==( const format_t& other) const
{ 
    return width == other.width && height == other.height;
}

bool format_t::operator!=( const format_t& other) const
{ 
    return width != other.width || height != other.height;
}

std::ostream& operator<<( std::ostream& os, const format_t& f)
{
    os << "[" << f.width << "," << f.height << "]";
    return os;
}

} // namespace
