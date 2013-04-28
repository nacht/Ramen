// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_IMAGEIO_EXCEPTIONS_HPP
#define	RAMEN_IMAGEIO_EXCEPTIONS_HPP

#include<stdexcept>

namespace ramen
{
namespace imageio
{

struct unknown_image_format_error : public std::runtime_error
{
    unknown_image_format_error() : std::runtime_error( "Unknown image file format") {}
};

struct unsupported_image_error : public std::runtime_error
{
    unsupported_image_error() : std::runtime_error( "Unsupported image type") {}
};

} // namespace
} // namespace

#endif	/* _EXCEPTIONS_HPP */
