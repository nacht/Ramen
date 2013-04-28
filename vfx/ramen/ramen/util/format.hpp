/**
 * @file
 * @brief Image size format
 */

// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.
#ifndef RAMEN_FORMAT_HPP
#define RAMEN_FORMAT_HPP

#include<boost/serialization/serialization.hpp>

#include<OpenEXR/ImathBox.h>

namespace ramen
{

/**
 * @class format_t
 * @brief format_t represents an image size.
 */

struct format_t
{
    format_t();
    format_t( std::size_t w, std::size_t h);

    Imath::Box2i area() const;

    bool operator==( const format_t& other) const;
    bool operator!=( const format_t& other) const;

    std::size_t width, height;

    template<class Archive>
    void serialize( Archive & ar, const unsigned int version)
    {
	ar & boost::serialization::make_nvp( "width", width);
	ar & boost::serialization::make_nvp( "height", height);
    }
};

std::ostream& operator<<( std::ostream& os, const format_t& f);

} // namespace

#endif
