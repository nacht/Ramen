// Copyright (c) 2010 Esteban Tovagliari
// Licensed under the terms of the CDDL License.
// See CDDL_LICENSE.txt for a copy of the license.

#include<ramen/imageio/oiio/oiio_writer.hpp>

#include<memory>

#include<OpenImageIO/typedesc.h>

namespace ramen
{
namespace imageio
{

void oiio_writer_t::do_write( const boost::filesystem::path& p, const OpenImageIO::ImageSpec& spec,
				const image::const_image_view_t& view)
{
    std::auto_ptr<OpenImageIO::ImageOutput> out( OpenImageIO::ImageOutput::create( p.external_file_string().c_str()));

    if( !out.get())
	throw std::runtime_error( "Write image: Can't find a format");

    if( !out->open( p.external_file_string().c_str(), spec))
	throw std::runtime_error( "Write image: Can't open file");

    char *pixels = (char *) boost::gil::interleaved_view_get_raw_data( view);
    
    if( spec.nchannels == 1) // save alpha channel
	pixels += 3;

    std::size_t xstride = 4 * sizeof( float);
    std::size_t ystride = xstride * view.width();

    if( !out->write_image( TypeDesc::FLOAT, pixels, xstride, ystride, OpenImageIO::AutoStride))
	throw( std::runtime_error( "Write image: Can't write pixels"));

    if( !out->close())
	throw std::runtime_error( "Write image: Can't close file");
}
    
} // namespace
} // namespace
