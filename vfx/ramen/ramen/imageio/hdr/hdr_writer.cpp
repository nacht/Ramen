// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/hdr/hdr_writer.hpp>

namespace ramen
{
namespace imageio
{

void hdr_writer_t::do_write_image( const boost::filesystem::path& p,
				const image::const_image_view_t& view,
				const adobe::dictionary_t& params) const
{
    std::auto_ptr<OpenImageIO::ImageOutput> out( OpenImageIO::ImageOutput::create( p.external_file_string()));

    if( !out.get())
	throw std::runtime_error( "Write HDR: Can't open output file");

    OpenImageIO::ImageSpec spec( view.width(), view.height(), 3, TypeDesc::FLOAT);

    if( !out->open( p.external_file_string(), spec))
	throw std::runtime_error( "Write HDR: Can't open output file");

    char *pixels = (char *) boost::gil::interleaved_view_get_raw_data( view);
    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();

    if( !out->write_image( TypeDesc::FLOAT, pixels, xstride, ystride, OpenImageIO::AutoStride))
	throw( std::runtime_error( "Write HDR: Can't write pixels"));

    if( !out->close())
	throw std::runtime_error( "Write HDR: Can't close file");
}

} // namespace
} // namespace
