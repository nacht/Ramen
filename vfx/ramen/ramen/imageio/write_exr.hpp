// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_WRITE_EXR_HPP
#define	RAMEN_WRITE_EXR_HPP

#include<boost/filesystem/path.hpp>

#include<OpenEXR/ImfHeader.h>
#include<OpenEXR/ImfIO.h>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace imageio
{

void write_half_exr( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view,
		    bool write_alpha = true);

void write_half_alpha_exr( const boost::filesystem::path& p, Imf::Header& header, const image::const_image_view_t& view);

void write_half_rgb_exr( Imf::OStream& os, Imf::Header& header, const image::const_image_view_t& view);

} // namespace
} // namespace

#endif	/* _WRITE_EXR_HPP */
