// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#ifndef RAMEN_READ_EXR_HPP
#define RAMEN_READ_EXR_HPP

#include<boost/filesystem/path.hpp>

#include<OpenEXR/ImathBox.h>
#include<OpenEXR/ImfIO.h>

#include<gil/extension/half.hpp>

#include<ramen/image/typedefs.hpp>

namespace ramen
{
namespace imageio
{

void read_exr_image( const boost::filesystem::path& p, const image::image_view_t& result_view,
		    const Imath::Box2i& crop, std::size_t subsample = 1);

void read_exr_image( const boost::filesystem::path& p, const image::image_view_t& result_view,
		    const Imath::Box2i& crop, const char *rchannel, const char *gchannel,
		    const char *bchannel, const char *achannel, std::size_t subsample = 1);

void read_exr_luma_chroma_image( const boost::filesystem::path& p, const image::image_view_t& result_view,
				    const Imath::Box2i& crop, std::size_t subsample = 1);

void read_exr_image( Imf::IStream& is, boost::gil::rgba16f_image_t& image);

void read_exr_image_proxy( const boost::filesystem::path& p, const image::image_view_t& result_view,
			    const Imath::Box2i& crop, std::size_t subsample = 1);

void read_exr_image_proxy( const boost::filesystem::path& p, const image::image_view_t& result_view,
			    const Imath::Box2i& crop, const char *rchannel, const char *gchannel,
			    const char *bchannel, const char *achannel, std::size_t subsample = 1);

} // namespace
} // namespace

#endif
